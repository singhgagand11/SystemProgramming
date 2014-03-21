/**
 *  @file
 *  Header for functions in proj2.c.
 *  <pre>CPE 357 - 01
 *  Project02 - Word counter
 *  -------------------
 *  Header for proj2.c</pre>
 *  @author Gagandeep Singh
 */


#ifndef PROJ2
#define PROJ2

/** converts longer strings to 20 characters followed by
 * 6 digit string length and adds it to the hash table.
 * @param table hash table to add word to.
 * @param string the string to be added.
 */
void changeLen(Table *table, char *string);
#endif

