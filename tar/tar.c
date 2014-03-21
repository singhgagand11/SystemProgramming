/**
 * @file tar.c
 * for the creation of a single tar file
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  Use to control the io specifically with a tar file
 *  effiently using nly system calls.
 *
 *  Last Modified: Sat Feb 26 04:22:30 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */
#include <stdio.h>
#include <unistd.h>
#include "tar.h"
#include "debug.h"
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>       
#define TAR_WRITE O_WRONLY | O_CREAT | O_TRUNC  
#define TAR_READ O_RDONLY

/*
 * opens a tar file to either be written to or read from
 * @param path path to the tar file
 * @param mode mode to open the file, either 'w' or  'r' for writing or reading
 * @return returns NULL on error otherwise the tar stream
 */
TAR* opentar( const char * path, char * mode ,int sec  )
{

  TAR *ret;
  int fd;
  int flags;
  errno = 0;
  switch( mode[0] )
  {
    case 'w': flags = TAR_WRITE; break; 
    case 'r': flags = TAR_READ; break;
    default: errno = EINVAL; 
             return NULL;
  }
    
  if( (fd = open( path , flags, sec) ) == -1 )
  {
    return NULL;
  }
  FATALCALL( (ret =(TAR*) malloc( sizeof( TAR ) )) == NULL,"Error:");
  ret->fd = fd;
  ret->flags = flags;
  ret->err = 0;
  ret->eof = 0;
  ret->bufpos = 0;
  ret->blockpos = 0;
  ret->next = 0;
  ret->strict = 0;
  ret->bytesRead = 0;
  return ret;
}
/* close a tar object and frees and associated files
 * @param tar tar stream to be closed
 */
void closetar( TAR* tar )
{
  /*flush the buffer on close*/
  if( tar->flags & O_WRONLY )
  {
    /*flush */
    write( tar->fd, tar->buffer, tar->blockpos );
    /*write the eof*/
    memset( tar->buffer, 0, TAR_BLOCK* 2);
    write( tar->fd, tar->buffer, TAR_BLOCK * 2 );

  }
  close( tar->fd );

  free( tar );
}

/* reads a large chunck of the tar file into the tar buffer
 * @param tar tar stream to be read from
 * return -1 on error or eof, otherwise 0
 */
int readbuffer( TAR* tar)
{
  if( tar->err || tar->eof)
  {
    return -1;
  }
  if( ( tar->bytesRead = read( tar->fd, tar->buffer, TAR_BUFFER ) ) < 0 )
  {
    tar->err = tar->eof = -1;
    return -1;
  }
  if( tar->bytesRead == 0 )
  {
    tar->eof = -1;
  }
  tar->bufpos += tar->bytesRead;
  tar->blockpos = 0;
  return 0;
}
/*
 * reads a one block from the tar buffer into the tar block stream
 * @param tar tar stream to read from
 * @return -1 on error
 */
int readblock( TAR* tar )
{
  if( tar->eof  )
  {
    return -1;
  }

  if( tar->blockpos + TAR_BLOCK > tar->bytesRead )
  {
    if( readbuffer(tar ) < 0 )
    {
      return -1;
    }
  }

  memcpy( tar->block, tar->buffer + tar->blockpos, TAR_BLOCK);
  tar->blockpos += TAR_BLOCK;
  return 0;
}
/* determines if the tar block is empty
 * @param tar stream to be checked
 * @return 1 if the block is all empty otherwise 0
 */
int isEmpty( TAR* tar )
{
  int i = 0;
  for(  ; i < TAR_BLOCK; ++i)
  {
    if( tar->block[i] != 0)
      return 0;
  }
  return 1;
}
/* changes the read pos of the tar stream. If the SEEK_CUR option is used
 * offset will be from the block pos inside the buffer not the
 * read position of the underlying file
 * @param tar tar stream to change the pos of
 * @param offset offset of the strean to set at
 * @param whence the base to offset from, expects SEEK_CUR, SEEK_END, SEEK_SET
 * @return returns the new read pos of the buffer
 */
off_t tarseek( TAR* tar, off_t offset, int whence)
{
  /* offset should be floored tot he nearest 512 divisor */
  if( offset %512 != 0 )
    offset = (offset / 512) * 512;
  if( whence == SEEK_CUR )
  {
    tar->bufpos = lseek( 
        tar->fd, tar->bufpos - tar->bytesRead + tar->blockpos + offset, 
        SEEK_SET );
  }
  else
    tar->bufpos = lseek( tar->fd, offset, whence);
  tar->blockpos = TAR_BUFFER;
  return tar->bufpos;


}
/* writes the tar buffer to the tar file
 * @param tar the tar stream to write to
 * @return -1 on error otherwise 0
 */
int writebuffer( TAR * tar )
{
  if( tar->err || tar->eof )
  {
    return -1;
  }
  if( write( tar->fd, tar->buffer, tar->blockpos ) == -1 )
  {
    tar->blockpos = 0;
    return tar->err = tar->eof = -1;
    
  }
  tar->bufpos += tar->blockpos;
  tar->blockpos = 0;
  return 0;
}
/**
 * writes a block of data to the char buffer. The size of buf need
 * not actually be a full block size. In such cases, the remainder of
 * the block is filled with null characters
 * @param tar tar to be written to
 * @param buf source block to read write from
 * @param size size of the buf
 * @return -1 on error
 */
int writeblock( TAR* tar, char * buf, int size )
{                 
  errno = 0;
  if( size > TAR_BLOCK )
  {
    errno = EINVAL;
    return -1;
  }
  if( tar->eof || tar->err )
  {
    return -1;
  }

  if( tar->blockpos + TAR_BLOCK > TAR_BUFFER )
  {
    if( writebuffer( tar ) == -1 )
    {
      return -1;
    }
  }
  memset( tar->buffer + tar->blockpos, 0, TAR_BLOCK );
  memcpy( tar->buffer + tar->blockpos, buf, size );
  tar->blockpos +=TAR_BLOCK;
  return 0;
}
/** returns whether or not an error has occured on the tar stream
 * @param tar stream to check
 * @return non-zero on error otherwise 0
 */
int terror( TAR* tar )
{
  return tar->err;
}
/** returns whether or not the tar files has reached eof
 * @param tar tar stream to check
 * @return non-zero if eof otherwise 0
 */
int teof( TAR* tar)
{
  return tar->eof;
}
