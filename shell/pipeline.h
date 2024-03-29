/**
 * @file
 * Header for pipeline.c
 *
 * @author Philip Nico
 * @author Kevin O'Gorman
 */

#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdio.h>
#include <sys/types.h>
#include "stringlist.h"

typedef struct clstage *clstage;

struct clstage {
  char *inname;                 /* input filename (or NULL for stdin) */
  char *outname;                /* output filename (NULL for stdout)  */
  int  argc;                    /* argc and argv for the child        */
  char **argv;                  /* Array for argv                     */

  clstage next;                 /* link pointer for listing in the parser */
};

typedef struct pipeline {
  char           *cline;              /* the original command line  */
  int            length;              /* length of the pipeline     */
  struct clstage *stage;              /* descriptors for the stages */
} *pipeline;


/* prototypes for pipeline.c */
extern void     print_pipeline(FILE *where, pipeline cl);
extern void     free_pipeline(pipeline cl);
extern pipeline crack_pipeline(char *line);
extern clstage  make_stage(slist l);
extern void     free_stage(clstage s);
extern void     free_stagelist(clstage s);
extern clstage  append_stage(clstage s, clstage t);
extern pipeline make_pipeline(clstage stages);
extern int      check_pipeline(pipeline pl, int lineno);

#endif
