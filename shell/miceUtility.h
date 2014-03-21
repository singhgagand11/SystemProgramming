/**
 * @file miceUtility.h
 * <pre> CPE 357 WINTER 2011
 * -------------------
 *
 *  Header to "miceUtility.h"
 *
 *  @author Gagandeep Singh Kohli
 */

#ifndef _MICEUTILITY_H_
#define _MICEUTILITY_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "config.h"
#include "pipeline.h"
#include "parser.h"
#include "myline.h"

/** @enum DIRERROR
 * my cd_Dir function return values 
 */
typedef enum DIRERROR{
   USAGE = 1,         /* usuage error */
   ENVRERR = 2,       /* Missing envirn Path HOME */ 
   SUCCESS = 3,       /* SUCCESS */
   FAILURE = -1       /* OTHER ERRORS */
}DIRERROR;

/** @struct Child
 *  Child process table with child id and return status 
 */
typedef struct Child{
   pid_t id;
   int status;
}Child;

/* @def OUT describe out end of the pipe */
#define OUT 1

/* @def IN describe in end of the pipe */
#define IN 0

/* @def DECIMAL for tokenizing the exit command argument */
#define DECIMAL 10 

/* @def FILETYPE for creating output files */
#define FILETYPE O_WRONLY | O_CREAT | O_TRUNC | O_APPEND

/* @def FILEMODE for creating output files */
#define FILEMODE 0666
 
/* 
 * Initalized a child structure to id and status to -1 (similar to jave new)
 * @param child child structure to initalize
 * @param length total number of childrens (child stuctures) in child the array
 * @return 0 stating the child structure was initalzed otherwise -1
 */ 
int newChild( Child * childs, int length );

/* 
 * Prints out the child id's and return status...Used as a debugging tool
 * @param child array of child structure with fields for child id and return status 
 * @param length total number of childrens (child stuctures) in child the array
 * @return 0 if child array was successfully printed, otherwise -1
 */
int printChild( Child * childs, int length );

/* 
 * Creates a pipeline, forks, and executes childrens
 * Note: pl will never be NULL, already checked on process
 * @param pl pipeline created by the command line phraser 
 * @param child array of child structure with fields for child id and return status 
 * @return exit status of the last child forked in the pipeline
 */
int IPC( pipeline pl, Child * childs);


/* 
 * Checks of a valid pipes and calls IPC to launch childs and commands
 *
 * @param pl pipeline created by the command line phraser 
 * @return exit status of the last child forked in the pipeline
 */
int process( pipeline pl );

/* 
 * Clears the buffers and supplies a newline when ctr+c signal is caught
 * @param signo signal number
 */
void clear_terminal(int signo);

/* 
 * Changes the current working directory to the argument passed in otherwise HOME path is used
 * @param currStage clstage structure containing the cd command and arugemt used for changing
 *        directories.
 * @return enum DIRERROR indicating the error type or success of cd command
 */
DIRERROR cd_Dir( clstage currStage );

 
#endif
