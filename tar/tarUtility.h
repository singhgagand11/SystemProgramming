/**
 * @file tarUtility.h
 * Header to tarUtility.c
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *
 *  Last Modified: Sat Feb 26 04:53:31 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */

#ifndef _TARUTILITY_H_
#define _TARUTILITY_H_


#include "tar.h"
#include "tarent.h" 
#include "dirtree.h"
#define SECURITY 0777
#define OPSTRING "ctxvSf:"
#define OPTVERS "--version"
#define OPTHELP "--help"
#define OPTUSE "--usage"  

/** @enum OPT_OP 
 * option code mask values 
 */
typedef enum OPT_OP { TCREATE = 1, TLIST = 2, TEXTRACT = 4, 
              TVERBOSE = 8, TSTRICT = 16, TARCHIVE = 32, 
              TARVERSION = 64, TARHELP = 128, TARUSAGE = 256} OPT_OP;

/* Creation */

/* inserts a file/directory/linl into tar */ 
TERROR compfile( char* path, struct stat* sbuf, TAR* tar, int opt );
/* inserts a the path and all sub-paths into a day */
TERROR compress( TAR* tar, char* path, int opt  );

/* Listing */

/* list all entries in a tar, provides for specific entry search */
TERROR listFiles(TAR* tar, int opt, char ** search, int serc);
/** translates file modes into a readable string */
void getPerm( int mode, char filetype, char * dest, int size );

/* Extraction */

/* extracts a tarfile creating all entrys */
TERROR extract( TAR * tar, int opt, char ** search, int serc );
/* builds all dirtree nodes in root */
TERROR rebuild( TAR * tar, dirtree * root, char * path, int opt );
/* creates the tar entry into the filesystem */
TERROR mkentry( TAR * tar, tarent * tard, char * path, char type, int mode, int opt );

int Filter( char * key, char * str );
/* gets the command line options for execution */
int options( int argc, char * argv[], int *op, char ** tarfile);
/* prints the program usage information */
void UsageInfo();
/* prints the program help info */
void HelpInfo();
/* prints the program version */
void VersionInfo();
#endif

