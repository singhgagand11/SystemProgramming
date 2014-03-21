/**
 * @file dirtree.c
 * For the creation of directory trees
 * <pre> CPE 357 Spring 2011
 * -------------------
 *
 *  For the creation of directory trees
 *
 *  Last Modified: Sat Feb 26 04:30:57 PM PST 2011</pre>
 *  @author Matthew Tondreau
 *  @author Gagandeep Singh Kohli
 */
#include "dirtree.h"
#include <string.h>
#include <dirent.h>
#define NAME_MAX 256

#define ATTACHFILE( tard, root ) \
  if( root->sub == NULL ){\
    root->pos = tard->pos;\
    root->time = tard->mtime; }

/** adds the path to the directory tree, implicitly building anything
 * not explicitly list but needed to create any child dirs/files.
 * @param root root of the directory tree
 * @param path path to be inserted into the directory
 * @param tard tar entry to be created
 * @return the new root directory tree
 */
dirtree* build( dirtree * root, const char * path, tarent * tard )
{
  char * str;
  char copy[NAME_MAX];
  dirtree *next, *head = root;

  strcpy( copy, path );
  if( (str = strtok( copy, "/\0" ) ) == NULL ) /* gone one past */
    return root;

  if( root == NULL )
  {
    root = malloc( sizeof( dirtree ) );
    root->next = NULL;
    root->sub = NULL;
    root->pos = -1;
    root->time = 0;
    strcpy( root->name, str );
    root->sub = build( root->sub, path + strlen( str ) + 1, tard);
    head = root;
    ATTACHFILE( tard, root);
  }
  else
  {
    /* otherwise search for a match in root->next else append*/
    for( next = root; next != NULL; next = next->next )
    {
      if( strcmp( next->name, str ) == 0 )
      {
        next->sub = build( next->sub, path + strlen( str ) + 1, tard );
        /* only attach if newer version */
        if( next->sub == NULL && tard->mtime > next->time )
        {
          ATTACHFILE( tard, next );
        }
        return head;
      }
      if( next->next == NULL )
      {
        /*no matches were found in next so add it to next */
        next->next = malloc( sizeof( dirtree ) );
        next->next->next = NULL;
        next->next->sub = NULL;
        next->next->pos = -1;
        next->next->time = 0;
        strcpy( next->next->name, str );
        next->next->sub = build( next->next->sub, path + strlen( str ) + 1, tard);
        ATTACHFILE( tard, next->next );
      }
    }
  }
  return head;
}

/** destroys a directory tree
 * @param root directory tree root to be freed
 */
void destroyTree( dirtree * root )
{
  if ( root == NULL )
    return ;
  destroyTree( root->next );
  destroyTree( root->sub ); 
  free( root );
}
