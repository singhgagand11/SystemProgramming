/**
 * @file tar.h
 * Header to tar.c
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  Header to "tar.c"
 *
 *  Last Modified: Sat Feb 26 04:33:31 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */

#ifndef TAR_H
#define TAR_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define TSUID   04000
#define TSGID   02000
#define TSVTX   01000
#define TUREAD  00400
#define TUWRITE 00200
#define TUEXEC  00100
#define TGREAD  00040
#define TGWRITE 00020
#define TGEXEC  00010
#define TOREAD  00004
#define TOWRITE 00002
#define TOEXEC  00001


#define REGTYPE         '0'     /* Regular file (preferred code).  */
#define AREGTYPE        '\0'    /* Regular file (alternate code).  */
#define LNKTYPE         '1'     /* Hard link.  */
#define SYMTYPE         '2'     /* Symbolic link (hard if not supported).  */
#define CHRTYPE         '3'     /* Character special.  */
#define BLKTYPE         '4'     /* Block special.  */
#define DIRTYPE         '5'     /* Directory.  */
#define FIFOTYPE        '6'     /* Named pipe.  */
#define CONTTYPE        '7'     /* Contiguous file */

#define TMAGIC  "ustar"
#define TMAGLEN 5
#define SMAGIC "ustar\00000" /* 7 chars and a null */
#define SMAGLEN 8
#define TVERSION        "00"
#define TVERSLEN        2

#define FW_NAME     100
#define FW_MODE     8
#define FW_UID      8
#define FW_GID      8
#define FW_SIZE     12
#define FW_MTIME    12
#define FW_CHKSUM   8
#define FW_TYPEFLAG 1
#define FW_LINKNAME 100
#define FW_MAGIC    6
#define FW_UVERSION 2
#define FW_OWNNAME  32
#define FW_GRPNAME  32
#define FW_DEVMAJ   8
#define FW_DEVMIN   8
#define FW_PREFIX   155

#define FO_NAME     0
#define FO_MODE     FW_NAME
#define FO_UID      ( FO_MODE     + FW_MODE )
#define FO_GID      ( FO_UID      + FW_UID   )
#define FO_SIZE     ( FO_GID      + FW_GID   )
#define FO_MTIME    ( FO_SIZE     + FW_SIZE )
#define FO_CHKSUM   ( FO_MTIME    + FW_MTIME    )
#define FO_TYPEFLAG ( FO_CHKSUM   + FW_CHKSUM   )
#define FO_LINKNAME ( FO_TYPEFLAG + FW_TYPEFLAG )
#define FO_MAGIC    ( FO_LINKNAME + FW_LINKNAME )
#define FO_UVERSION ( FO_MAGIC + FW_MAGIC )
#define FO_OWNNAME  ( FO_UVERSION + FW_UVERSION )
#define FO_GRPNAME  ( FO_OWNNAME + FW_OWNNAME )
#define FO_DEVMAJ  ( FO_GRPNAME + FW_GRPNAME )
#define FO_DEVMIN   ( FO_DEVMAJ + FW_DEVMAJ )
#define FO_PREFIX   ( FO_DEVMIN + FW_DEVMIN )

#define OCT 8
#define TAR_BLOCK 512
#define TAR_BUFFER 10240

#define MIN( x, y ) ( x < y ? x : y )
struct posix_header
{     /* byte offset */
  char name[100];    /*   0 */
  char mode[8];      /* 100 */
  char uid[8];       /* 108 */
  char gid[8];       /* 116 */
  char size[12];     /* 124 */
  char mtime[12];    /* 136 */
  char chksum[8];    /* 148 */
  char typeflag;     /* 156 */
  char linkname[100];/* 157 */
  char magic[6];     /* 257 */
  char version[2];   /* 263 */
  char uname[32];    /* 265 */
  char gname[32];    /* 297 */
  char devmajor[8];  /* 329 */
  char devminor[8];  /* 337 */
  char prefix[155];  /* 345 */

};

/** @struct TAR 
 * Acts as a file stream for basic io in respect to handling
 * a tar file with mutiple entries
 */
typedef struct TAR
{
  int flags;

  char block[TAR_BLOCK];
  char buffer[TAR_BUFFER];
  off_t bufpos;
  int blockpos;
  int  fd;
  short err;
  short eof;
  int bytesRead;
  off_t next;
  char strict;
} TAR;

/** sets the offset inside the tar file **/
off_t tarseek( TAR* tar, off_t offset, int whence);
/** reads the file into the tar buffer **/
int readbuffer( TAR* tar);
/** reads a single tar block from the buffer  into the tar block buffer**/
int readblock( TAR* tar );

/** writes the buffer to the tar file **/
int writebuffer( TAR* tar );
/** writes a block to the buffer **/
int writeblock( TAR* tar, char * buf, int size );

/** opens a tar for read or write only**/
TAR* opentar(const char * path, char * mode, int security );
/** closes a tar file **/
void closetar( TAR * );
/** returns 1 if the tar block is empty/all 0**/
int isEmpty( TAR* tar );
/** return 1 if tar is in error state**/
int terror( TAR* tar);
/** return 1 if tar is at eof **/
int teof( TAR* tar );
#endif

