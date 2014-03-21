/*
 * cpl:  A shell pipeline cracker
 *
 * Author: Dr. Phillip Nico
 *         Department of Computer Science
 *         California Polytechnic State University
 *         One Grand Avenue.
 *         San Luis Obispo, CA  93407  USA
 *
 * Email:  pnico@csc.calpoly.edu
 *
 * Author: Dr. Kevin O'Gorman
 *         for tweaks, memory leaks, and POSIX compliance (all minor)
 *
 * Revision History:
 *         $Log: main.c,v $
 *         Revision 1.4  2003-04-15 18:11:34-07  pnico
 *         Checkpointing before distribution
 *
 *         Revision 1.3  2003-04-12 10:37:22-07  pnico
 *         added config.h to list of inclusions
 *
 *         Revision 1.2  2003-04-11 08:38:55-07  pnico
 *         Ready for first release of mice
 *
 *         Revision 1.1  2003-04-10 19:36:24-07  pnico
 *         Initial revision
 *
 *
 */
#include "config.h"

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pipeline.h"
#include "parser.h"
#include "myline.h"
#include "miceUtility.h"
#include <limits.h>

#define PROMPT "Squeak? "

static void printusage(char *name);
static void prompt(char *prompt);

int main(int argc, char *argv[]){
  char *line = NULL;
  size_t linelen = 0;
  pipeline pl;
  int run;
  int result, error;
  char *promptstr;

  int fileInput, exitStatus = EXIT_SUCCESS;
 
  clstage currStage;


  /* check for the right number of arguments */
  if ( argc > 2 ) {
    printusage(argv[0]);
    exit(EXIT_FAILURE);
  }


   /* Input file */
  if( argc == 2)
  {
    if( (fileInput = open (argv[1], O_RDONLY)) == -1)
    {
      perror( argv[1] );
      exit (EXIT_FAILURE); 
    }
    dup2( fileInput, STDIN_FILENO );
    close( fileInput);
  }


  /* set prompt */
  if( (promptstr = getenv( "MICEPROMPT")) == NULL )
    promptstr = PROMPT;


  /* set up the default SIGINT to clear screen */
   signal(SIGINT, clear_terminal);
  
 
  run = TRUE;
  prompt(promptstr);
  /* run */
  while ( run ) {


    if ( 0 > (result = myline(&line, &linelen, stdin))) {
      if ( feof(stdin) )
        run = FALSE;
    } else {
      /* We got a line, send it off to the pipeline cracker and
       * launch it
       */
      pl = crack_pipeline(line);


      /*
       * Show that it worked.  This is where you're going to do
       * something radically different: rather than printing the
       * pipeline, you're going to execute it.
       */
      if ( pl != NULL )
      {
        /* print_pipeline(stdout,pl); */                                              /* print it. */
        currStage = pl->stage;
        if( currStage->argc != 0)  /* check we didn't get a return char, if we did ignore all this */
        {
          if( strcmp( "cd" , currStage->argv[0] ) == 0 )               /* build in command */
          {
            error = cd_Dir( currStage );               /*check error, clean up and move on */
            if( error == USAGE )
              fprintf( stderr, "cd: usage error, to many arguments\n");
            if( error == ENVRERR )
              fprintf(stderr, "cd: Environment variable 'HOME' missing\n");
            if( error == FAILURE )
              perror("cd");      /* unknown directory name */
          }

          else if( strcmp( "exit" , currStage->argv[0] ) == 0 )
          {
            /* check for argc */
            /* no arguments return previous stage/ pipe stages */
       
             
            if( currStage->argc > 2 ) /* to many arguments */
              fprintf( stderr, "exit: usage error, to many arguments\n");
            if(currStage->argc == 2 )
            {
              error = strtol( currStage->argv[1], NULL, DECIMAL );
              if( error != LONG_MIN || error != LONG_MAX ) /* overflow --NEED TO ASK PROFESSOR*/
                exitStatus = error ;
            }
            /* free pipe line and break out */
            free_pipeline(pl); 
            if (line)
               free(line);

          exit ( exitStatus );
      
          }
          else                     /* this is where the magic happens, func(s) @ miceUtility.c */
            exitStatus = process(  pl  );  
            
            free_pipeline(pl);                       /* will not free line -- that one is ours */
        }
      }
      else
        exitStatus = EXIT_FAILURE; /* bad input */

      lineno++;  /* readLongString trims newlines, so increment it manually */
    }

    /* clear the streams */
    if( ferror(stdout) == 0)
        clearerr(stdout);
    if ( run )                 /* assuming we still want to run */
      prompt(promptstr);
  }
  /* Only free this at the end, because re-using the buffer can avoid sone
   * realloc() calls in getline() or myline().
   */
  if (line)
    free(line);   /* We don't hold it so it won't be a leak at the end */
  if ( isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) )
  putchar('\n');  /* In case of control-D, get back to the left margin */
  return 0;
}

static void prompt(char *pr) {
  /* If this is an interactive shell, flush the output streams and
   * print a prompt
   */

  if ( isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) ) {
    printf("%s", pr);
    fflush(stdout);
  }
}

static void printusage(char *name){
  fprintf(stderr,"usage: %s\n",name);
}
