/**
 * @file miceUtility.c
 * <pre> CPE 357 WINTER 2011
 * -------------------
 *
 *  functions for "miceUtility.c"
 *
 *  @author Gagandeep Singh Kohli
 */

#include "miceUtility.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "debug.h"


/* 
 * Checks of a valid pipes and calls IPC to launch childs and commands
 *
 * @param pl pipeline created by the command line phraser 
 * @return exit status of the last child forked in the pipeline
 */
int process( pipeline pl )
{
  int totalP, status;
  Child *myChilds;      /* Child ARRAY */


  if(pl->cline == NULL) /* check if the cmd arg is not empty , should never happened by main */
    return -1;

  totalP = pl->length;

  /* perform the build in commands */


  /* children info array */
  FATALCALL( (myChilds = (Child *) malloc (sizeof(Child) * totalP)) == NULL, "Invalid Malloc");

  /* inialize child struct to EMPTY */
  FATAL( (newChild( myChilds, totalP)) == -1, "Error")

    /* create the pipes */
    status = IPC( pl, myChilds );

  /* free the id's and return status */
  free ( myChilds );
  return status;
}

/* 
 * Creates a pipeline, forks, and executes childrens
 * Note: pl will never be NULL, already checked on process
 * @param pl pipeline created by the command line phraser 
 * @param child array of child structure with fields for child id and return status 
 * @return exit status of the last child forked in the pipeline
 */
int IPC( pipeline pl, Child * childs )
{

  int pipefd[2];
  int prevIn = -1, i, child_T, file_In, file_Out;
  Child *currChild= NULL;
  struct clstage currStage;
  int status;
  sigset_t block;

  /* set total # of commands/CHILDS */
  child_T = pl->length;

  /* block the SIGINT */       
  sigemptyset(&block);
  sigaddset(&block, SIGINT);
  sigprocmask(SIG_SETMASK, &block, NULL);

  for(i = 0; i < child_T; i++)
  {
    FATALCALL( pipe(pipefd) == -1, "pipe");  /* create the pipe          */
    currChild = childs+i;                    /* current child to process */
    currStage = pl->stage[i];

    /*  SET UP THE PREVIOUS PIPE, CHECK FOR INDIRECTION OPEN FILE FD */
    if( (currChild->id = fork()) < 0)
      perror("Child");
    else if( currChild->id == 0)               /* child */
    {
      close(pipefd[IN]); /* will never use pipe stdin side, next child would */

      
      /* handle input */
      if(currStage.inname != NULL)
      {

        if ( (file_In = open( currStage.inname, O_RDONLY )) == -1)
        {
            perror ( currStage.inname );
            exit ( EXIT_FAILURE );
        }
        FATALCALL ( dup2(file_In, STDIN_FILENO) == -1, "DUP" );
        close(file_In);
      }
      else
      {
        if( i > 0)
        {
         dup2(prevIn, STDIN_FILENO); /* set up the stdin to be tty on first call and previous pipe-in(0) otherwise */         
         close ( prevIn );
        }
      }

         /* middle processes and last process */
        if(currStage.outname != NULL)
        {
          FATALCALL ( (file_Out = open( currStage.outname, FILETYPE, FILEMODE )) == -1, "Open Error");
          FATALCALL ( dup2(file_Out, STDOUT_FILENO) == -1, "DUP Error" );
          close( file_Out );
        }
        else
        {
            if( i != child_T -1 ) /* no file to output and not last child */
            {
               FATALCALL ( dup2(pipefd[OUT], STDOUT_FILENO) == -1 , "DUP Error" ); /* else set up last in put */  
            }
             /* else, last child prints to the terminal, which is inherited */
            close( pipefd[OUT]); 
        }
      
    
 
      /* unBLOCK the SIGNAL */
      signal(SIGINT, SIG_DFL);
      sigemptyset(&block);
      sigprocmask(SIG_SETMASK, &block, NULL);
       
       execvp(currStage.argv[0], currStage.argv);
       perror(currStage.argv[0]);

       /* exit with a failure */
       exit(EXIT_FAILURE);
    }
    else
    {
      /* parent */
      if( prevIn > -1 ) /* if prevIn wasn't the first child close it */
         close(prevIn);
      prevIn = pipefd[IN]; /* save In End of the last pipe, needed for next child */
      close(pipefd[OUT]);  /* close out end of the pipe */
 
      if( i == child_T-1)
        close( prevIn ); /* last unused end of the pipe */
    }
  }
 
 
  for(i = 0; i < child_T; i++)
  {
      /* wait and retrieve return status of childrens */
      FATALCALL ( wait ( &((childs+i)->status) ) == -1 , "WAIT" );  
  }
  
  /* unblock and reset to clear terminal */
  sigemptyset(&block);
  sigprocmask(SIG_SETMASK, &block, NULL);
   
  status = (childs+i-1)->status;

  /* flush out the data */
  fflush( stdout );
  /* exit staus checking */
  if ( WIFEXITED( status )) 
    return WEXITSTATUS( status );
  else if (WIFSIGNALED(status))
    return 100 + WTERMSIG( status );
  else
    return EXIT_FAILURE;
}


 
/* 
 * Initalized a child structure to id and status to -1 (similar to jave new)
 * @param child child structure to initalize
 * @param length total number of childrens (child stuctures) in child the array
 * @return 0 stating the child structure was initalzed otherwise -1
 */ 
int newChild( Child * childs, int length )
{
  int i = 0;
  if(childs == NULL)
    return -1;     /* EMPTY ARRAY */
  for(; i < length; i++)
  {
    (childs+i)->id = -1;
    (childs+i)->status = -1;
  }
  return 0;
}

/* 
 * Clears the buffers and supplies a newline when ctr+c signal is caught
 * @param signo signal number
 */
void clear_terminal(int signo)
{

  fflush(NULL);
  fprintf(stdout, "\n"); /* supply new line and return to prompt */
  signal(SIGINT, clear_terminal);
}

/* 
 * Changes the current working directory to the argument passed in otherwise HOME path is used
 * @param currStage clstage structure containing the cd command and arugemt used for changing
 *        directories.
 * @return enum DIRERROR indicating the error type or success of cd command
 */
DIRERROR cd_Dir( clstage currStage )
{
  char  *homePath;
  if(currStage->argc > 1) /* we got a dir name */
  {
    if( currStage->argc > 2)
      return USAGE;
    else
    {
      if( chdir(currStage->argv[1]) == -1 )
        return FAILURE;
    }
  }
  else /* no arguments */
  {
    if( ( homePath = getenv("HOME") ) == NULL )
      return ENVRERR;
    else
    {

      if( chdir( homePath ) == -1 )
        perror("cd");          /* shouldn't happen */
    }
  }
  return SUCCESS;
}

/* 
 * Prints out the child id's and return status...Used as a debugging tool
 * @param child array of child structure with fields for child id and return status 
 * @param length total number of childrens (child stuctures) in child the array
 * @return 0 if child array was successfully printed, otherwise -1
 */
int printChild( Child * childs, int length )
{
  int i = 0;
  if(childs == NULL)
    return -1;     /* EMPTY ARRAY */
  for(; i < length; i++)
  {
    fprintf( stdout, "[%d]: \tID : %d\tSTATUS : %d\n", \
        i , (childs+i)->id, (childs+i)->status);
  }
  return 0;
}
