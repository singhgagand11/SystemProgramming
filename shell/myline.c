/**
 *  @file
 *  <pre>CPE 357 Winter 2010
 *  -------------------
 *  myline() -- a clone of the GNU getline() function.
 *  See the GNU manpage for details.
 *
 *  @author Kevin O'Gorman
 *  Last Modified: Sat Feb 26 10:32:58 PST 2011
 */
 
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>

#include "myline.h"

/** Initial size */
#define BUFL 120

/**
 *    <p>myline() reads arbitrarily-long lines.  It can use a buffer supplied
 *    by the caller, or provide a new one.  In both cases, the memory must
 *    come from the heap (that is, from malloc(0), calloc(), realloc(),
 *    strdup() or something similar).  The myline() function itself never
 *    calls free() or malloc(), doing everything needed with realloc().
 *
 *    The user is responsible for the disposition of the buffer either by
 *    calling free() on it, or reusing it in a subsequent call to getline(),
 *    or in some other way ensuring the memory does not leak.</p>
 *
 *    <p>This function is "8-bit clean"; only newlines are treated specially
 *    and even NUL bytes are tolerated (and included in the output) of this
 *    function.  For this reason, the return value is more reliable than the
 *    results of strlen() on the buffer.</p>
 *
 *    <p>The program is careful to return any data that has been read
 *    before returning an error or end-of-file indication.</p>
 *
 *    @param lineptr in/out parameter: the buffer for the data.  If this is
 *     NULL, getline() will malloc some memory for starters.  On return,
 *     the pointer is updated.
 *    @param n       in/out parameter. The size of buffer.  If this is zero,
 *     getline() will ignore any address in lineptr, and allocate a new
 *     buffer.  On return, the number is updated.
 *    @param stream  the file stream to read from
 *
 *    @return
 *        On success, the number of characters read including any newline,
 *        but not including the terminating null byte.
 *        On failure (NULL parameters, end of file condition or error), -1.
 *
 */
ssize_t myline(char **lineptr, size_t *n, FILE *stream)
{
  char* buff;
  size_t buflen;
  int seen = 0;
  char ch;

  if (!lineptr || !n || !stream) {
    errno = EINVAL;
    return -1;
  } else {
    if (!*n) {  /* If the length is zero, the lineptr is ignored. NULL works. */
      *lineptr = NULL;
    }
    buff = *lineptr;
    buflen = *n;
    /* Start fresh; any important problems will show up again immediately */
    /*clearerr(stream);*/
  }

  if(!buff || buflen < BUFL) {
    buflen = BUFL;                  /*update size of buffer*/
    if ((buff = (char*)realloc(buff, buflen)) == 0) {
      return -1;
    }
    *lineptr = buff;
    *n = buflen;
  }

  while((ch = getc(stream)) != EOF) {
    /* ensure there's room for the data plus a NUL byte in case
     * there's end-of-file coming next.
     */
    if(seen >= (buflen - 1))
    {
      buflen = buflen * 2;
      buff = (char*)realloc(buff, buflen);
      if (buff == 0) {
        return -1;
      }
      *lineptr = buff;
    }
    buff[seen++] = ch;
    if(ch == '\n')
    {
      break;
    }
  }
  *lineptr = buff;
  *n = buflen;
  if (!seen) {
    /* Report errors or eof only on the first character, so as to preserve
     * any data not yet returned to the caller. */
    return -1;
  }

  /* Unlike the usual getline() function, this one ensures that each line
   * ends with a newline.  Even lines with an IO error.
   */
  if (seen == 0 || buff[seen - 1] != '\n') {
    if(seen >= (buflen - 2))
    {
      buflen = buflen * 2;
      buff = (char*)realloc(buff, buflen);
      if (buff == 0) {
        return -1;
      }
      *lineptr = buff;
      *n = buflen;
    }
    buff[seen++] = '\n';
  }
  buff[seen] = '\0';
  return seen;
}

/* vim: set et ai sts=2 sw=2: */
