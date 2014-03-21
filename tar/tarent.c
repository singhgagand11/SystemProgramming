/**
 * @file tarent.c
 * Tar entry creation
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  For the creation of tar entries, used in tandem with tar.h
 *  to create a complete tarfile.
 *
 *  Last Modified: Sat Feb 26 04:23:13 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */
#include "tarent.h"
#include "debug.h"
#include <string.h>
#include <limits.h>
#include <errno.h>
/**
 * converts a string of an octal numbers into an integer
 * @param str octal string to be converted
 * @param len length of the string to be converted
 * @param out the converted integer value 
 * @return -1 on error, 0 on success
 */
int convert( const char * str, int len, int *out)
{
  char * endptr;
  *out = strtol(str, &endptr,OCT);
  if( (errno == ERANGE && (*out == LONG_MAX || LONG_MIN) )
      || (errno != 0 && *out == 0 )
      || ( endptr == str && len != 0 )
      || ( *endptr != '\0' ))
  {
    return -1;   
  }
  return 0;
}

/** reads a single tar entry from the tar file. 
 * @param tar tarfile to be read from
 * @param tardout the tar entry that was read
 * @return a Tar error code
 */
TERROR readtar( TAR* tar, tarent **tardout )
{
  static tarent tard;
  int val;
  struct posix_header ph;
  /*TODO: add error */
  
  if( tarseek(tar,tar->next, SEEK_SET) == -1 )
  {
    return SYSERR;
  }

  tard.strict = tar->strict;
  tard.pos = tar->next;
  tard.tar = tar;
  
  if( readblock(tar) == -1 )/*reached an eof or err, both shouldnt happen */
  {
    if( terror( tar ))
      return SYSERR;
    else
      return NOTTAR;
  }

  if( isEmpty(tar) )  /*if the block is empty possible eof */
  {
    if( readblock(tar) != -1 && isEmpty(tar) )
    {
      (*tardout) = NULL;
      return SUCCESS;
    }
    else
    {
      /*corrupted*/
      tar->err = -1;
      (*tardout) = NULL;
      return NOTTAR;
    }
  }
  /* note: some implementations will use a space 
   * instead of a null to end a entry */
  memcpy( (char*)&ph, tar->block, sizeof( ph ) ); 
  memcpy(tard.name, ph.name, FW_NAME);
  memcpy( tard.uname, ph.uname, FW_OWNNAME );
  memcpy( tard.gname, ph.gname, FW_GRPNAME );
  memcpy( tard.prefix, ph.prefix, FW_PREFIX );
  memcpy( tard.linkname, ph.linkname, FW_LINKNAME );
  
  VALIDTAR( convert( ph.mode, FW_MODE,&tard.mode ) == -1 );
  if( (val = extract_special_int( ph.uid, FW_UID -1 ) ) == -1 )
  {
    VALIDTAR( convert( ph.uid, FW_UID,&tard.uid ) == -1 );
  }
  else
  {
    tard.uid = val;
  }
  if( (val = extract_special_int( ph.gid, FW_GID -1 ) ) == -1 )
  {
    VALIDTAR( convert( ph.gid, FW_GID,&tard.gid ) == -1 );
  }
  else
  {
    tard.gid = val;
  }
  
  VALIDTAR( convert( ph.mtime, FW_MTIME, (int *)&tard.mtime ) == -1 );
  VALIDTAR( convert( ph.chksum, FW_CHKSUM, &tard.chksum ) == -1 );
  /*if( tar->strict )
  {
    VALIDTAR( convert( ph.devmajor, FW_DEVMAJ, &tard.devmaj) == -1);
    VALIDTAR( convert( ph.devminor, FW_DEVMIN, &tard.devmin) == -1);
  } */
  VALIDTAR( convert( ph.size, FW_SIZE,&tard.size ) == -1 )
  tard.typeflag = ph.typeflag;
  /*  validate the checksum before proceeding */
  VALIDTAR( tard.chksum != getchksum( tar->buffer ) );

  
  /*validate version and magic #*/
  if( tar->strict )
  { /*using the ustar format */
    VALIDTAR( strncmp( SMAGIC, ph.magic, SMAGLEN ) != 0 );
  }
  else
  { /* accepting the oldgnu */
    VALIDTAR( strncmp( TMAGIC, ph.magic, TMAGLEN ) != 0 );
  }
  /*prepare the tar for the next entry */
  tar->next =tard.pos +TAR_BLOCK;
  if( tard.size % TAR_BLOCK == 0 ) 
    tar->next += tard.size;
  else
    tar->next += ( tard.size / TAR_BLOCK + 1 ) * TAR_BLOCK ;

  *tardout = &tard;
  return SUCCESS;
}

/** copies the file data from a tar entry into the the file desriptor fd.
 * @param tard the tar entry to retrieve data from
 * @param fd file descriptor to copy output to
 * @return -1 on error, 0 on success
 */
int readfile( tarent * tard, int fd )
{
  off_t size = tard->size;
  if( tarseek(tard->tar, tard->pos + TAR_BLOCK, SEEK_SET ) == -1 )
  {
    return -1;
  }
  if( readbuffer( tard->tar ) == -1 )
  {
    return -1;
  }
  while( size > TAR_BUFFER )
  {

    if( write( fd, tard->tar->buffer, TAR_BUFFER ) < TAR_BUFFER )
    {
      return -1;
    }
    size -= TAR_BUFFER;
    if( readbuffer( tard->tar ) == -1 )
    {
      return -1;
    }
  }
  if( size > 0 )
  {
    if( write( fd, tard->tar->buffer, size ) < size )
    {
      return -1;
    }
  }
  return 0;
}
/** gets the check sum of an entire block by adding up all unsigned entries
 * and treating the 8 bytes where the checksum should be as spaces 
 * @param block a 512 block to be summed
 * @return the check sum of the block
 */
int getchksum ( char * block )
{
  int i = 0, sum = 0;
 for ( ; i < FO_CHKSUM; ++i)
   sum += (unsigned char) block[i];
 for( i = 0; i < FW_CHKSUM; ++i) /* eigth chksum bytes should be counted as spaces */
   sum += ' ';
 for( i = FO_CHKSUM + FW_CHKSUM; i < TAR_BLOCK; ++i)
   sum += (unsigned char) block[i];

  return sum;
}
/** copies creates a header file based on the entries in tard into 
 * the character buffer, of atleast 512 bytes, buf.
 * @param tard the entry with the loaded data to be written
 * @param buf the buffer to write teh header to
 * @return -1 on error, 0 on success
 */
int makeheader( tarent * tard, char * buf )
{
  int pos;
  errno = 0;
  /* initialize block to zero */
  memset( buf, 0, TAR_BLOCK );

  /*store name */
  if( (pos = strlen( tard->name) ) > FW_NAME )
    return -1;
  strcpy( buf, tard->name );
  /* name may exactly 100 chars!!!*/
  if( pos < FW_NAME )
    buf[pos] = 0;

  /* store mode */
  if(( pos =sprintf( buf + FO_MODE, "%07o", tard->mode )) != 7) 
  {
    errno = EINVAL;
    return -1;
  }
  buf[FO_MODE + FW_MODE - 1] = 0;
  /* store UID*/
  if( tard->uid > MAX_UID )
  {
     if( insert_special_int( buf + FO_UID, FW_UID -1, tard->uid) != 0 )
     {
        errno = EINVAL;
        return -1;
     }
  }
  else if( sprintf( buf + FO_UID, "%07o", tard->uid ) != 7 )
    {
      errno = EINVAL;
      return -1;
    }
  
  buf[FO_UID + FW_UID - 1] = 0;
  /* store GID */
  if( tard->gid > MAX_UID )
  {
     if( insert_special_int( buf + FO_GID, FW_GID -1, tard->gid ) != 0 )
     {
        errno = EINVAL;
        return -1;
     }
  }
  else if( sprintf( buf + FO_GID, "%07o", tard->gid ) != 7 ) 
  {
    errno = EINVAL;
    return -1;
  }
  buf[FO_GID + FW_GID - 1] = 0;
  /* store size */
  if( sprintf( buf + FO_SIZE, "%011o", tard->size ) != 11 )
  {
    errno = EINVAL;
    return -1;
  }
  buf[FO_SIZE + FW_SIZE - 1] = 0;
  /* store mtime */
  if( sprintf( buf + FO_MTIME, "%011o", (int) tard->mtime ) != 11 )
  {
    errno = EINVAL;
    return -1;
  }
  buf[FO_MTIME + FW_MTIME - 1] = 0;/*TODO: erro check here? */
  /* store filetype */
  buf[FO_TYPEFLAG] = tard->typeflag;/*TODO: erro check here? */
  /* store linked file */
  strncpy(buf + FO_LINKNAME, tard->linkname, FW_LINKNAME );/*TODO: erro check here? */
  /*store ustart*/
  strncpy( buf + FO_MAGIC, SMAGIC, SMAGLEN );/*TODO: erro check here? */
  buf[FO_MAGIC + TMAGLEN] = 0;

  /* ustar version */
  memcpy( buf + FO_UVERSION, TVERSION, TVERSLEN );

  memcpy(  buf + FO_OWNNAME,tard->uname, FW_OWNNAME );
  memcpy(  buf + FO_GRPNAME, tard->gname,FW_GRPNAME );
  memcpy(  buf + FO_PREFIX,tard->prefix, FW_PREFIX );

  /*if( tard->strict )
    {
    if( sprintf( buf + FO_DEVMAJ, "%07o", tard->devmaj ) != 7)
    {
    errno = EINVAL;
    return -1;
    }
    if( sprintf( buf + FO_DEVMIN, "%07o", tard->devmin ) != 7 )
    {
    errno = EINVAL;
    return -1;
    }
    } */
  /* store chksum */
  if( sprintf( buf + FO_CHKSUM, "%07o", getchksum(buf) ) != 7 )
  {
    errno = EINVAL;
    return -1;
  }
  buf[FO_CHKSUM + FW_CHKSUM - 1] = 0;


  return 0;
}
/** Writes the tard header to file 
 * @param tard the tar entry to be written
 * @return -1 on error, 0 on success
 */
int writeheader( tarent * tard )
{
  static char blk[TAR_BLOCK];
  if( makeheader( tard, blk ) == -1 )
    return -1;
  if( writeblock( tard->tar, blk, TAR_BLOCK ) == -1 )
  {
    return -1;
  }
  return 0;

}
/** writes the file filedes into a tar file under the tar entry tard
 * @return -1 on error, 0 on success
 */
int writefile( tarent *tard, int filedes)
{
  off_t pos ;
  int bytesRead;
  static char inbuf[TAR_BUFFER];
  /* write header */

  /*write file entry */
  while( ( bytesRead = read( filedes, inbuf, TAR_BUFFER) ) > 0 )
  {
    /* for(pos = 0 ; pos < bytesRead; pos += TAR_BLOCK )
       {
       if( writeblock( tard->tar, inbuf + pos, MIN( bytesRead, TAR_BLOCK) ) == -1 )
       return -1;
       }*/
    pos = 0;
    while( bytesRead >= TAR_BLOCK )
    {
      if( writeblock( tard->tar, inbuf + pos, TAR_BLOCK ) == -1 )
        return -1;
      pos += TAR_BLOCK;
      bytesRead -= TAR_BLOCK;
    }
    /* write the last block if it ends before a full block */
    if( bytesRead > 0 )
      if( writeblock( tard->tar, inbuf + pos, bytesRead ) == -1 )
        return -1;
  }
  if( bytesRead < 0 )
  {
    return -1;
  }

  return 0;

}
/** returns the file type code of a given lstat mode 
 * @param mode the integer mode
 * @return the tar character interpretation of the filetype, -1 on error */
char getfmt( mode_t mode )
{
  switch( mode & S_IFMT )
  {
  case S_IFLNK:return '2';
  case S_IFREG:return '0';
  case S_IFBLK:return '4';
  case S_IFDIR:return '5';
  case S_IFCHR: return '3';
  case S_IFIFO: return '6';
  default: return -1;
  }

}
/** splits a path name into name and prefix, if neccesary.
 * @param src 256 byte pathname to be split
 * @param name file name portion to copy into
 * @param prefix file prefix portion to copy into
 * @return -1 on error, 0 on success
 */
int split( char * src, char* name, char* prefix  )
{
  int pos;
  int maxname = FW_NAME;
  char *ptr, *splice = NULL  ;
  char copy[NAME_MAX];
  pos = strlen( src );
  if( pos > FW_NAME + FW_PREFIX )
  {
    return -1;
  }
  strcpy( copy, src );
  if( copy[pos -1 ] == '/' )
  {
    copy[pos -1] = '\0';
    --maxname;
  }
  ptr = copy;
  if( pos > maxname )
  {
    /* process the name */
    while( (ptr = strchr( ptr, '/' ) ) != '\0' && (ptr - copy) < FW_PREFIX )
    {
      splice = ptr++;
    }
    if( splice == NULL )
      return -1;
    *splice = '\0';
    if( strlen( ++splice) > maxname )
      return -1;
    else if( strlen( splice ) == maxname )
    {
      strncpy( name, splice, maxname );
    }
    else
    {
      strcpy( name, splice );
    }
    strcpy( prefix, copy ); 
  }
  else if( pos == maxname )
  {
    strncpy( name, copy, maxname );
  }
  else
  {
    strcpy( name, copy );
  }
  if( maxname != FW_NAME )
  {
    if( splice == NULL )
    {
      name[ strlen( name ) ] = '/';
    }
    else
    {
      name[ strlen( splice ) ] = '/';
    }
  }
  return 0;
}
/** extracts the the user/group id if in the extended binary format.
 * required for extracting ids who cannon be represented in a 7 byte
 * octal ascii string
 * @param where uid/gid string
 * @param len length of the string
 * @return the decimal value of the id, -1 on error
 */
uint32_t extract_special_int( char * where, int len )
{
  int32_t val = -1;
  if( (len > sizeof( val )  ) && (where[0] & 0x80 ) )
  {
    val = * (int32_t *)(where+len-sizeof(val));
    val = ntohl(val);
  }
  return val;
}
/** inserts the extended binary representation of the user/group id.
 * required for ids that cannot be represented in a 7 bytes octal 
 * ascii string
 * @param where string to insert at
 * @param size size of the string where
 * @param val decimal id to insert
 * @return 1 on error, 0 on success
 */
int insert_special_int( char* where, size_t size, int32_t val )
{
  int err = 0;
  if( val < 0 || ( size < sizeof( val ) ) ) {
    err++;
  }
  else
  {
    memset( where, 0, size );
    *(int32_t*) ( where+size-sizeof(val) ) = htonl(val );
    *where |= 0x80;
  }
  return err;
}
/** prepends the string 'prefix' to the file name 'name.'
 * stores the result in an internal static buffer.
 * @param name file name portion
 * @param prefix file prefix portion
 * @return the combined catenation of prefix and name
 */
char * combine( char * name, char * prefix )
{
    static char buffer[NAME_MAX];
    memset(buffer, 0, NAME_MAX);
    if( prefix[0] != '\0')
    {
      /* catenate name to prefix */
      strcpy( buffer, prefix );
      buffer[ strlen(buffer)] = '/';
      buffer[ strlen(buffer)] = '\0';

      strncpy( buffer + strlen( buffer ), name, FW_NAME );
      buffer[NAME_MAX] = '\0';
    }
    else
    {
      strncpy( buffer, name, FW_NAME );
      buffer[FW_NAME] = '\0';
    }
    return buffer;

}

