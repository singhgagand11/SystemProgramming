/**
 * @file mytar.c
 * Clone of tar working with ustar and oldgnu formats
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  Program main entry, acts as a clone of the tar utility, creating 
 *  and extracting files in ustar and  oldgnu style formats
 *
 *  Last Modified: Sat Feb 26 04:21:47 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "tarent.h"
#include "debug.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "tarUtility.h"



/** Main entry.
 * @param argc the number of tokens on the input line.
 * @param argv an array of tokens.
 * @return 0 on success, 1-255 on failure
 */
int
main(int argc, char *argv[])
{
  TAR * tar;
  int ret;
  int op, optind;
  char * tarfile;
  /** get options **/
  if( (optind = options( argc, argv, &op, &tarfile ) ) == -1 )
  {
    fprintf(stderr, "Try `%s --help' or `tar --usage' for more information.\n", argv[0]);
    
    return EXIT_FAILURE;
  }
  if( op & TARUSAGE )
  {
    UsageInfo();
    return EXIT_SUCCESS;
  }
  if( op & TARVERSION )
  {
    VersionInfo();
    return EXIT_SUCCESS;
  }
  if( op & TARHELP )
  {
    HelpInfo();
    return EXIT_SUCCESS;
  }
  /************************
   * if createing a tar 
   * *********************/
  if( op & TCREATE )
  {
    FATALCALL( ( tar = opentar(tarfile, "w",0660 ) ) == NULL, argv[0] );
    tar->strict = op & TSTRICT;
    /* create a tar file from all inputs an */
    for( ;optind < argc; optind++ )
    {
      if( ( ret =compress( tar, argv[optind], op ) )!= SUCCESS )
        break;
    }
  }
  /***********************
   * if listing a tar 
   * *********************/
  else if ( op & TLIST )
  {
    FATALCALL( ( tar = opentar(tarfile, "r",0660 ) ) == NULL, argv[0] );
    tar->strict = op & TSTRICT;
    if( optind < argc )
      ret = listFiles( tar, op, argv + optind, argc - optind );
    else
      ret = listFiles( tar, op, NULL, 0 );
  }
  /***********************
   * if extracting a tar 
   * *********************/
  else /* TEXTRACT */
  {
    FATALCALL( ( tar = opentar(tarfile, "r",0660 ) ) == NULL, argv[0] );
    tar->strict = op & TSTRICT;
    if( optind < argc )
      ret = extract( tar, op, argv + optind, argc - optind );
    else  
      ret = extract( tar, op, NULL , 0 );
  }
 /* error check */ 

    closetar( tar );
  if(  ret )
  {
    if( ret == SYSERR )
    {
      perror( argv[0] );
    }
    else if( ret == NOTTAR )
    {
      fprintf( stderr, "%s: non-ustar format.\n", argv[0] );
    }
    else if( ret == NOTSUP )
    {
      fprintf( stderr, "%s: file type not supported.\n", argv[0] );
    }
    else if( ret == NAMELONG)
    {
      fprintf( stderr, "%s: Filepath too long.\n", argv[0] );
    }
    else if( ret == UIDERR )
    {
      fprintf( stderr, "%s: UID/GID out of uid_t range 0..2097151\n", argv[0] );
    }

    exit( EXIT_FAILURE );
  }
  return EXIT_SUCCESS;
}

