/**
 * @file tarent.h
 * Header to tarent.c
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  Header to "tarent.h"
 *
 *  Last Modified: Sat Feb 26 04:35:29 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */

#ifndef TARENT_H
#define TARENT_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tar.h"
#include <arpa/inet.h>

/** @def MAX_UID defines the maximum storeable user/group id */
#define MAX_UID 07777777

#define VALIDTAR( condition )\
  if( condition ) {\
    (*tardout) = NULL;\
    return NOTTAR;}

/** @struct tarent
 * Acts as an interface for writing tar entries into the tar file
 */
typedef struct tarent
{
  struct TAR* tar;
  char name[FW_NAME];
  int mode;
  int uid;
  int gid;
  int size;
  time_t mtime;
  int chksum;
  char typeflag;
  char linkname[FW_LINKNAME];
  off_t pos;
  char gname[FW_GRPNAME];
  char uname[FW_OWNNAME];
  int devmaj;
  int devmin;
  char prefix[FW_PREFIX];
  char strict;
} tarent;

/** error codes specific to tar **/
typedef enum TERROR { 
  SUCCESS = 0, /* ok! */
  SYSERR=1,   /* system error */
  NOTTAR=2,    /* non-tar format */
  NOTSUP=4,   /*file type not supported */
  NAMELONG= 8, /* file name too long */
  BADERR = 16,
  UIDERR = 32
} TERROR;

/** returns  next tar entry from a tar**/
TERROR readtar( TAR* tar, tarent ** tard);
/** copies the file in the tar entry to fd */ 
int readfile( tarent * tard, int fd );
/** writes the tarentry and filedes to the tar */
int writefile( tarent *tard, int filedes);
/** returns the check sum value of an entire block */
int getchksum ( char * block );
/** writes the tar header to the tar file */
int writeheader( tarent * tard );
/** returns the tar file type code */
char getfmt( mode_t mode );
/** splits a path name into name and prefix */
int split( char * src, char* name, char* prefix );
/** inserts spcial user/group ids into a string */
int insert_special_int(char *where, size_t size, int32_t val);
/** extracts special user/group ids out of a string */
uint32_t extract_special_int(char *where, int len);
/** catentates the name and filename */
char * combine( char * name, char * prefix );
/** converts an octal ascii string to int */
int convert( const char * str, int len, int *out);
#endif

