/**
 * @file
 *  <pre>CPE 357 Spring 2009
 *  -------------------
 *  Header for getline() clone
 *
 *  @author Kevin O'Gorman
 *  Last Modified: Fri Oct 23 15:40:27 PDT 2009
 */

#ifndef GETLINE_H
#define GETLINE_H

/* Input arbitrary-length text lines. */
ssize_t myline(char **lineptr, size_t *n, FILE *stream);

#endif
