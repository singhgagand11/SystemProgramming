/**
 * @file dirtree.h
 * Header to dirtree.c
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  for the creation of directory trees.
 *
 *  Last Modified: Sat Feb 26 04:30:44 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */

#ifndef DIRTREE_H 
#define DIRTREE_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "tarent.h"
/** @struct dirtree
 * directory tree structure to prep the extraction and 
 * creation of the tar entries
 */
typedef struct dirtree
{
  char name[100]; /* truncated name */
  size_t pos; /* pos of the entry in tar */
  time_t time;
  struct dirtree *sub; /* children file/dir */
  struct dirtree *next;
} dirtree;
/** adds the path to the directory tree
 */
dirtree* build( dirtree * root, const char * path, tarent * tard );
/** destroys a directory tree */
void destroyTree( dirtree * root );

#endif
