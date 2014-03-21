/**
 * @file tarUtility.c
 * main operation functions
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  Functions for the main operation of the tar program 
 *
 *  Last Modified: Sat Feb 26 04:58:18 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <utime.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include "tar.h"
#include "tarent.h"
#include "tarUtility.h"
#include "debug.h"

#include "dirtree.h"
#define NAME_MAX 256
#define TIME_SIZE 17

/* for masking the permissions of a file */
const int tpermbit [] = { TSUID, TSGID, TSVTX, TUREAD, TUWRITE, TUEXEC,
  TGREAD, TGWRITE, TGEXEC, TOREAD, TOWRITE, TOEXEC };
/* for translating the position to the approriate char symbol*/
const char tpermch [] = { 0,0,0,'r','w','x','r','w','x','r','w','x' };
/* for determining the proper positioning of a tpermch */
const int tpermpos [] = { 0,0,0,1,2,3,4,5,6,7,8,9 };

/** inserts the file in path into the tar file 
 * @param path path to the file to be inserted
 * @param sbuf lstat information
 * @param tar tar file to write to
 * @param opt any specific operation modifiers (verbose )
 * @return  a tar error code or SUCCESS
 */
TERROR compfile( char* path, struct stat* sbuf, TAR* tar, int opt )
{
  tarent tard;
  ssize_t size;
  struct group* g;
  struct passwd* pwd;
  int fd;
  tard.strict = tar->strict;
  tard.tar = tar;
  memset( tard.name, 0, FW_NAME);
  memset( tard.prefix,0,FW_PREFIX);
  if( split(path, tard.name, tard.prefix ) == -1 )
    return NAMELONG;
  
  if( tar->strict )
  {
    if( sbuf->st_uid > MAX_UID || sbuf->st_gid > MAX_UID )
      return UIDERR;
  }
  tard.uid = sbuf->st_uid;
  tard.gid = sbuf->st_gid;
  
  
  tard.mtime = sbuf->st_mtime;
  tard.mode = sbuf->st_mode & (~ S_IFMT); /* get permissions only */
  tard.typeflag = getfmt( sbuf->st_mode);

  /* extra ustar junk */
  if( (g = getgrgid( sbuf->st_gid )) == NULL)
  {
    return SYSERR;
  }
  memset( tard.gname, 0, FW_GRPNAME);
  strcpy( tard.gname, g->gr_name ); /* might need some protection */

  if( (pwd = getpwuid( sbuf->st_uid ) ) == NULL )
  {
    return SYSERR;
  }
  memset( tard.uname, 0, FW_OWNNAME);
  strcpy( tard.uname, pwd->pw_name ); /* might need some protection */
  tard.devmaj = tard.devmin = 0;

  if( S_ISLNK(sbuf->st_mode ) )
  {
    /* store atmost a link of size one less then the field width */
    if( (size = readlink( path, tard.linkname,FW_LINKNAME -1 ) ) == -1 )
    {
      return SYSERR;
    }
    /* append a null byte, not provided by readlink */
    tard.linkname[size] = 0;
  }
  else
  {
    memset( tard.linkname, 0, FW_LINKNAME );
  }

  if( S_ISREG( sbuf->st_mode ) )
  {
    tard.size = sbuf->st_size;
    /* write the header */
    if( writeheader( &tard ) == -1 )
      return SYSERR;
    /* read the file into */
    if( ( fd = open( path, O_RDONLY ) ) == -1 )
      return SYSERR;    
    if( writefile(&tard, fd ) == -1 )
      return SYSERR;
  }
  else
  {
    tard.size = 0;
    /* write the header */
    if( writeheader( &tard ) == -1 )
      return SYSERR;
  }

  if( opt & TVERBOSE )
  {
    if( fprintf(stdout, "%s\n", combine( tard.name, tard.prefix ) ) == EOF )
      return SYSERR;
  }
  return SUCCESS;

}
/** inserts the path and any sub-paths into a tar file 
 * @param path the root of the path to be inserted
 * @param tar the tar file to write to
 * @param opt any operation modifiers (verbose)
 * @return tar error code or SUCCESS
 */
TERROR compress( TAR* tar, char* path, int opt  )
{
  struct stat sbuf;
  struct dirent * dirp;
  DIR * dir;
  char * ptr;
  int ret;
  if( lstat( path, &sbuf ) < 0 )
  {
    return SYSERR;
  }

  if( S_ISREG( sbuf.st_mode ) || S_ISLNK( sbuf.st_mode ) )
  {
    /* process regular file  and symlinks */
    if( ( ret = compfile( path, &sbuf, tar, opt )) != SUCCESS )
    {
      return ret;      
    }

  }
  else if( S_ISDIR( sbuf.st_mode ) )
  {
    /* process directory */
    ptr = path + strlen( path );
    *ptr++ = '/';
    *ptr = 0;
    if( ( ret = compfile( path, &sbuf, tar, opt )) != SUCCESS )
      return ret;      

    if( (dir = opendir( path ) ) == NULL )
      return SYSERR;

    while( ( dirp = readdir( dir ) ) != NULL )
    {
      if( strcmp( dirp->d_name, ".") == 0 ||
          strcmp( dirp->d_name, "..") == 0 )
        continue;
      strcpy(ptr, dirp->d_name );
      if( (ret = compress( tar, path, opt)) != SUCCESS )
      {
        if( ret == NAMELONG )
        {
          fprintf(stderr, "file %s cannot be stored, skipping...\n", path );
          continue;
        }
        else
          return ret;
      }
    }
    *(ptr - 1 ) = 0; 
    if( closedir( dir ) == -1 )
    {
      return SYSERR;
    }
  }
  else
  {
    return NOTSUP;
  }
  return SUCCESS;
}

/** translates a decimal file mode into a 10 bit ascii string 
 * @param mode the decimal file mode
 * @param filetype the file type, such as directory or symlink
 * @param dest the destination string to copy into 
 * @param size size of the string in dest 
 */
void getPerm( int mode, char filetype, char  dest[], int size )
{
  int i;
  memset( dest, '-', size );
  for( i = 3; i < sizeof( tpermbit) && tpermpos[i] < size; ++i )
  {
    if( mode & tpermbit[i] )
      dest[tpermpos[i]] = tpermch[i];
  }
  if(filetype == DIRTYPE )
    dest[0] = 'd';
  else if( filetype == SYMTYPE )
  {
    dest[0] = 'l';
  }
  dest[size -1] = '\0';
}
/** prints a single tar entry in either verbose, where addictional information
 * is printed, or without where only the file name is printed
 * @param tard the tar entry to print
 * @param opt any operation modifiers (verbose )
 * @return tar error code if any
 */
TERROR printEntry( tarent * tard, int opt )
{
  struct tm * tmp;
  char time[TIME_SIZE];
  char  perm [11] ;
  if( opt & TVERBOSE)
  {
    if( (tmp = localtime( &tard->mtime )) == NULL )
      return SYSERR;
    if( strftime( time, TIME_SIZE, "%Y-%m-%d %H:%M",tmp) == 0)
      return SYSERR;
    getPerm( tard->mode, tard->typeflag, perm, sizeof( perm ) );
    fprintf( stdout, "%s %s/%s %8d %s %s\n", 
        perm,
        tard->uname,
        tard->gname,
        tard->size,
        time,
        combine( tard->name, tard->prefix ));
  }
  else
    fprintf(stdout, "%s\n",combine( tard->name, tard->prefix ) ); 
  return SUCCESS;
}
/** list the files in a tar file. If search is non-null, then only
 * the files upto serc will be listed (if they exist).
 * @param tar tar file to list
 * @param opt operation modifiers
 * @param search any files to search for, otherwise pass NULL to list all
 * @param serc number of search entries
 * @return tar error code if any
 */
TERROR listFiles(TAR* tar, int opt, char ** search, int serc)
{
  int i;
  tarent *tard = NULL;
  int status;
  while( !(status = readtar( tar, &tard ) ) && (tard != NULL) )
  {
    
    if( search == NULL )
    {
      if( ( status = printEntry( tard, opt ) ) )
        return status;
    }
    else
    {
      for( i = 0; i < serc; ++i )
      {
        if( Filter( search[i], combine(tard->name, tard->prefix) )  )
        {
          if( (status = printEntry( tard, opt ) ) )
            return status;
          break;
        }
      }
    }
  }
  return status;
}

/** transtes an getopt char into the proper option flag
 * @param opt the option character from getopt
 * @return operation option flag
 */
int translateOpt( char opt )
{
  switch ( opt )
  {
  case 'c':
    return TCREATE;
  case 't':
    return TLIST;
  case 'x':
    return TEXTRACT;
  case 'v':
    return TVERBOSE;
  case 'S':
    return TSTRICT;
  case 'f':
    return TARCHIVE;
  default:
    return -1;
  }

}
/** parses the argv array for what command to execute 
 * @param argc mains argument count
 * @param argv mains argument vector
 * @param op destination to return the option flags
 * @param tarfile destination to store the targeted tar file
 * @return -1 on error, otherwise the position of the last non-option
 */
int options( int argc, char * argv[], int *op, char ** tarfile)
{
  char * ptr;
  int opt;
  *tarfile = NULL;
  *op = 0;
  /* perform checks on long options */
  for( opt = 0; opt < argc; ++opt )
  {
    if( strcmp( argv[opt], OPTVERS ) == 0 )
    {
      *op = TARVERSION;
      return argc;
    }
    if( strcmp( argv[opt], OPTHELP ) == 0 )
    {
      *op = TARHELP;
      return argc;
    }
    if( strcmp( argv[opt], OPTUSE ) == 0 )
    {
      *op = TARUSAGE;
      return argc;
    }
  }
  
  while( ( opt = getopt( argc, argv, OPSTRING ) ) != -1 )
  {
    if( ( opt = translateOpt(opt) ) == -1 )
      return -1;
    else
      *op |= opt;
    if( opt == TARCHIVE )
    {
      *tarfile = optarg;
    }
  }

  /* check if the '-' was dropped*/
  if( optind < argc )
  {
    if( *op == 0 ) /* '-' was dropped on cmd line */
    {
      /* process first word as group of options */
      ptr =argv[optind++];
      while( *ptr != '\0' )
      {
        if( ( opt = translateOpt(*ptr) ) == -1 )
        {
          fprintf( stderr, "%s: invalid option -- '%c'\n",argv[0],*ptr );
          return -1;
        }
        else
          *op |= opt;
        if( opt == TARCHIVE )
        { 
          *tarfile = argv[optind++];
        }
        ptr++;
      }
    }
  }
  /* check ok if its ok to have no input files */
  if( optind == argc )
  {
    if( *op & TCREATE )
    {
      fprintf( stderr, "%s: Cowardly refusing to create an empty archive\n", argv[0]);
    }
  }

  /* check that we have a valid option */
  if( *op & TCREATE     )
  {
    if( *op & TLIST || *op & TEXTRACT )/* conflifts */
    {
      fprintf( stderr, "%s: you may not specify mor then one 'ctx' option\n", argv[0] );
      return -1;
    }
    else
      return optind;
  }    
  else if( *op & TLIST     )
  {
    if( *op & TEXTRACT )/* conflifts */
    {

      fprintf( stderr, "%s: you may not specify more then one 'ctx' option\n", argv[0] );
      return -1;
    }
    else
      return optind;
  }    
  else if( *op & TEXTRACT )
  {
    return optind;
  }
  else 
  { /* no-op specified */
    fprintf( stderr, "%s: you must specify one of the 'ctx' option\n", argv[0] );
    return -1;
  }

}

/** creates a file, directory, or symlink in the filesystem
 * @param tar tar file to read from
 * @param tard the tar entry to make
 * @param path the path to the file to create
 * @param type the type of file
 * @param mode of the file to create
 * @param opt any operation modifiers
 * @return tar error code if any error or 0 for SUCESS
 */
TERROR mkentry( TAR * tar, tarent * tard, char * path, char type, int mode, int opt )
{
  int fd;
  if( type == DIRTYPE )
  {
    if( mkdir( path, mode ) == -1 )
    {  if( errno != EEXIST )
        return SYSERR;
      else
        errno = 0;
    }
  }
  else if( type == LNKTYPE )
  {
    if( symlink( tard->linkname, tard->name ) == -1 )
    {
      if( errno != EEXIST )
        return SYSERR;
      else
      {
        errno = 0;
        unlink(path );
        if( symlink( tard->linkname, tard->name ) == -1 )
          return SYSERR;
      }
    }
  }
  else if( type == REGTYPE )
  {
    if( tar == NULL ) 
    {
      return BADERR;
    }
    else   /* apply mtime and read if file */
    {
      if( (fd = open( path, O_WRONLY | O_CREAT |  O_TRUNC, mode ) ) == -1 )
        return SYSERR;
      if( readfile( tard, fd ) == -1  )
        return SYSERR;
      if( close(fd) == -1 )
        return SYSERR;
    }
  }
  return SUCCESS;
}
/** builds all entries in a tar directory tree
 * @param tar tar file to read from
 * @param root directory tree root
 * @param path the current directory path
 * @param opt any operation modifiers
 * @return any tar error codes if any
 */
TERROR rebuild( TAR * tar, dirtree * root, char * path, int opt )
{
  char * tail; 
  struct utimbuf ftime;
  int end;
  int ret;
  tarent * tard;
  
  ftime.modtime =time(0);
  ftime.actime = time(0);
  if( root == NULL )
    return SUCCESS;

  tail = path + strlen( path ); 

  /* make the current node */
  if( root->pos != -1 ) /* entry exist */
  {
    tar->next = root->pos;
    if( (ret = readtar( tar, &tard) ) != SUCCESS )
      return ret;                      /* changed below */
    /* grab time info */
    ftime.modtime = tard->mtime;
    
    strcat( path, root->name );
    if( (ret = mkentry( tar, tard, path, tard->typeflag, tard->mode,opt ) ) != SUCCESS )
      fprintf( stderr, "error extracting file, skipping: %s", path);

  }
  else /*make directory with default settings  */
  {
    /* prepend path to root name */
    strcat( path, root->name );
    if((ret=mkentry( NULL, NULL, path, DIRTYPE, SECURITY,opt ) ) != SUCCESS )
      fprintf( stderr, "error extracting file, skipping: %s", path);
  }
  tail[0] = '\0';
  /* recurse to neigbors */
  if( ( ret =rebuild(tar, root->next, path, opt ) ) != SUCCESS )
    return SYSERR;

  /* recurse to children */
  strcat( path, root->name );
  end = strlen( path );
  path[end] = '/';
  path[end + 1] = '\0';
  if( ( ret =rebuild(tar, root->sub, path, opt ) ) != SUCCESS ) 
    return SYSERR;

  path[end] = '\0'; /* remove trailing '/' */
  if( utime( path, &ftime ) == -1 )
    return SYSERR;
  /* restore path */
  tail[0] = '\0'; /* remove name */
  return SUCCESS;
}
/** extracts all entries from a tar file 
 * @param tar the tar file to extract from
 * @param opt any operation modifers
 * @param search the filename to print in verbose mode
 * @param serc the number of iteams searched to print
 * @return any tar error codes if any, otherwise 0 for success
 */
TERROR extract( TAR * tar, int opt, char ** search, int serc )
{
  char buffer[ NAME_MAX ];
  int status, i;
  dirtree *root = NULL;
  tarent *tard = NULL;
  while(  (status = readtar( tar, &tard) ) == SUCCESS && ( tard != NULL ) )
  {
    
    memset(buffer,0, NAME_MAX);
    strcpy( buffer, combine( tard->name, tard->prefix ) );
    if( search != NULL )
    {
      for( i = 0; i < serc; ++i )
      {
        if( Filter( search[i], buffer) )
        {
          root = build( root, buffer, tard );
          if( opt & TVERBOSE )
            if( fprintf(stdout, "%s\n", buffer ) == EOF )
              return SYSERR;
          break;
        }
      }
    }
    else
    {
      root = build( root, buffer, tard );
      if( opt & TVERBOSE )
        if( fprintf(stdout, "%s\n", buffer ) == EOF )
          return SYSERR;
    }
  }
  buffer[0] = '\0';
  status = rebuild(tar,root,buffer, opt);
  destroyTree( root );
  return status;
}

int Filter( char * key, char * str )
{
  if( strlen( key ) > strlen( str ) )
    return 0;
  else
    if( strncmp( key, str, strlen( key) ) == 0 )
      return 1;
  return 0;
}

/** prints the program version info */
void VersionInfo()
{
  fprintf(stdout,  "\
      mytar (oldgnu/ustar) 1.0\n\
      Cal Poly Winter 2011 - CSC/CPE 357-01/02\n\
      Written by Matthew Tondreau and Gagandeep Singh\n");

}
/** prints the program help info */
void HelpInfo()
{
  UsageInfo();
  fprintf(stdout, "\
      `mytar' saves many files together in a single tape or disk archive\
      and can restore inidvidual files from an archive.\n\
      Examples:\n\n\
      mytar -cf archive.tar foo bar # Create archive.tar from files foo and bar.\n\
      mytar -tvf                    # List all files in archives.tar verbosely.\n\
      mytar -xf arhive.tar          # Extract all files from archive.tar.\n\n");

  fprintf(stdout,"\
      Main operation mode:\n\n\
      -c          create a new archive\n\
      -t          list the content of an archive\n\
      -x          extract files fron an archive\n\n\
      Operation modifiers:\n\n\
      -f          use archive file or device ARCHIVE (mandatory)\n\
      -S          strict interpretation of the standard\n\
      -v          verbosely list files processed\n\n\n\
      report bugs to <mmtondre@calpoly.edu> or <gasingh@calpoly.edu>\n\n");

}
/** prints the program version */
void UsageInfo()
{
  fprintf(stdout, "usage: mytar [-][ctxvS]f tarfile [path[...]]\n");
}
