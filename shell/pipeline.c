/*
 * a set of functions for manipulating pipeline structures.
 *
 * Author: Phillip Nico
 * Author: Kevin O'Gorman (POSIX compliance tweak)
 *
 * $Log: pipeline.c,v $
 * Revision 1.7  2003-04-15 18:11:34-07  pnico
 * Checkpointing before distribution
 *
 * Revision 1.6  2003-04-12 10:58:10-07  pnico
 * (oops) checked it in before compiling.  Fixed a typo.
 *
 * Revision 1.5  2003-04-12 10:56:53-07  pnico
 * added isempty() to check for NULL pipelines
 *
 * Revision 1.4  2003-04-12 10:37:05-07  pnico
 * *** empty log message ***
 *
 * Revision 1.3  2003-04-12 10:17:37-07  pnico
 * Cleaned up some declarations for porting to MINIX
 *
 */
#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "pipeline.h"
#include "parser.h"

/* Crack into the internals to clean up memory.  Thanks to Joe Thomas. */
extern int yylex_destroy(void);

pipeline crack_pipeline(char *line) {
  pipeline pl;
  int rval;

  clerror=0;                    /* clear the error flag            */
  parseresult = NULL;           /* catch the case where parseresult is
                                                                       * not set due to errors
                                                                       *                                  */

  set_scanstring(line);         /* point the scanner at the line   */
  rval=yyparse();               /* parse the command line          */
  pl=parseresult;               /* retreive the resulting pipeline */

  if ( clerror ) {
    /* there was a parse error in the command line.  Just free it,
     * return NULL, and move on.
     * errors will have been reported in the parser.
     */
    free_pipeline(pl);
    pl=NULL;
  } else {
    pl->cline = line;            /* attach the command line */
  }

  #ifdef DEBUG_MICE
  fprintf(stdout,"\nyyparse() returned: %d clerror=%d\n",rval,clerror);
  #endif

  return pl;
}


void print_pipeline(FILE *where, pipeline pl){
  int i,j,argc;
  /* print out the resulting pipeline */
  if ( pl->cline ) {
    fprintf(where,"\n");
    fprintf(where,"--------\n");
    fprintf(where,"Command Line: \"%s\"\n",pl->cline);
    fprintf(where,"--------\n");
  }
  for(i=0;i<pl->length;i++) {
    fprintf(where,"\n");
    fprintf(where,"--------\n");
    fprintf(where,"Stage %d: \n",i);
    fprintf(where,"--------\n");

    fprintf(where,"     input: %s\n",
            pl->stage[i].inname?pl->stage[i].inname:"(null)");
    fprintf(where,"    output: %s\n",
            pl->stage[i].outname?pl->stage[i].outname:"(null)");

    fprintf(where,"      argc: %d\n",pl->stage[i].argc);
    fprintf(where,"      argv: ");
    argc = pl->stage[i].argc;
    for(j=0;j<argc;j++) {
      fprintf(where,"\"%s\"%s",pl->stage[i].argv[j],
              (j<argc-1)?",":"");
    }
    fprintf(where,"\n");
  }
}

void free_pipeline(pipeline pl){
  /* free a pipeline, including all its stages */
  int i,j;
  if ( pl ) {
    /* don't free cline, because it was not allocated in make_pipeline, */
    /* meaning that the allocating function probably still has a pointer */
    /* to it that we don't want to invalidate.  Let's just clean up our */
    /* own messes. */
    if ( pl->stage ) {
      for(i=0;i<pl->length;i++) {
        if ( pl->stage[i].argv ) {
          /* free each string of argv, then argv, if present */
          for(j=0;j<pl->stage[i].argc;j++)
            if ( pl->stage[i].argv[j] )
              free ( pl->stage[i].argv[j] );
          free(pl->stage[i].argv );
        }
        /* 357: also free inname and outname (MEMLEAK) (Thanks to Joe Thomas */
        free(pl->stage[i].inname);
        free(pl->stage[i].outname);
      }
      free(pl->stage);        /* finally, toss the stage array */

    }
    free(pl);
  }
  /* Free the buffers (Thanks to Joe Thomas) */
  yylex_destroy();
}

clstage make_stage(slist l){
  /* take the given stringlist and make a pipeline stage
   * out of it.
   *
   * returns a struct, not a pointer.  (careful...)
   */
  clstage stage=NULL;
  int i;

  stage = (clstage) malloc(sizeof(struct clstage));  /* Gets lost on some syntax errors */
  if ( !stage )
    perror("malloc");
  else {
    stage->inname   = NULL;
    stage->outname  = NULL;
    stage->argc     = slist_length(l);
    stage->argv     = (char **) malloc((stage->argc+1)* sizeof(char *));
    if ( !stage->argv ) {
      perror("malloc");
      stage->argc = 0;             /* no room -> no commands */
    } else {
      for (i=0;l;l=l->next) {
        stage->argv[i] = (char*)malloc(strlen(l->str)+1);
        if ( !stage->argv[i] )
          perror("malloc");
        else
          strcpy(stage->argv[i++],l->str);
      }
      stage->argv[i]=NULL;              /* add terminal NULL */
    }
    stage->next = NULL;
  }

  return stage;
}

void free_stage(clstage stage){
  /* free all the parts of the given stage */
  int i;
  if ( stage ) {
    if (stage->inname)
      free(stage->inname);
    if (stage->outname)
      free(stage->outname);
    if ( stage->argv ) {
      for (i=0; i<stage->argc ; i++)
        free(stage->argv[i]);
    }
    free(stage);
  }
}

void free_stagelist(clstage stage){
  /* free the entire stagelist */
  clstage next;
  for(;stage;stage=next) {
    next = stage->next;
    free_stage(stage);
  }
}

extern clstage append_stage(clstage s, clstage t) {
  /* append the list to to the list s and return the result */
  clstage res=NULL;
  if ( !s )
    res = t;
  else
    res = s;
  if ( s ) {
    for ( ; s->next ; s = s->next )
      /* find the end */;
    s->next = t;                /* and add t */
  }
  return res;
}

static int count_stages(clstage l) {
  int len=0;
  for(;l;l=l->next)
    len++;
  return len;
}

pipeline make_pipeline(clstage stages) {
  /*
   * take the given stage list and make a proper pipeline structure
   * out of it.
   *
   * make_pipeline steals the strings from the stage list, buty
   * they're not long for this world anyhow.
   *
   * returns the pipeline on success, NULL on failure.
   */
  pipeline pl;
  int i;

  pl = (pipeline)malloc(sizeof(struct pipeline));
  if ( !pl ) {
    perror("malloc");
  } else {
    pl->cline  = NULL;
    pl->length = count_stages(stages);
    pl->stage  = (struct clstage *)malloc(pl->length * sizeof(struct clstage));
    if ( !pl->stage ) {
      perror("malloc");
      pl->length = 0;
    } else {
      for(i=0;i<pl->length;i++){
        pl->stage[i].inname  = stages->inname;
        pl->stage[i].outname = stages->outname;
        pl->stage[i].argc    = stages->argc;
        pl->stage[i].argv    = stages->argv;
        pl->stage[i].next    = NULL;

        stages->inname  = NULL; /* NULL these so they won't be freed */
        stages->outname = NULL;
        stages->argc    = 0;
        stages->argv    = NULL;

        stages = stages->next;
      }
    }

  }

  return pl;
}

extern int check_pipeline(pipeline pl, int lineno) {
  /* check the given pipeline for internal consistency
   * returns 0 on success, 1 on failure.
   *
   * check for the presence of a conflicting redirect and
   * pipe.  This is actually fairly simple:  If there's
   * a redirect in anything other than the first or last
   * stage of the pipeline, there's a problem.
   *
   */
  int i;
  int err=0;
  for(i=0;i<pl->length;i++) {
    if ( (i>0) && (pl->stage[i].inname) ) {
      fprintf(stderr,"Ambiguous input redirection, line %d.\n",lineno);
      err++;
    }
    if ( (i<pl->length-1) && (pl->stage[i].outname) ) {
      fprintf(stderr,"Ambiguous output redirection, line %d.\n",lineno);
      err++;
    }
    /* Fix from Billy McVicker, prevents a seg fault in the presence of "||" */
    if ((pl->length > 1) && (pl->stage[i].argc == 0)) {
      fprintf(stderr, "Syntax error near unexpected token '|', line %d.\n", lineno);
      err++;
    }
  }

  return err;
}
