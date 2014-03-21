/**
 *  @file
 *  Header for functions in myhash.c.
 *  <pre>CPE 357 - 01
 *  Project02 - Word counter
 *  -------------------
 *  Header for myhash.c</pre>
 *  @author Gagandeep Singh
 */


#ifndef MYHASH
#define MYHASH

#define TABLESIZE  491

typedef struct linkList {
   char word[26];
   int  count;
   struct linkList *next;
}Node; 

typedef struct bucket{
   Node *head;
   int size;
}Table;

/** adds the word into the hash table.
 * @param table pointer to the hashtable for adding words to.
 * @param name word to be added into the hash table.
 */
void add(Table *table, char *name);

/** hash key creator.
 * @param str word used for keying.
 * @returns the key.
 */
int hash(char *str);

/** prints the hash table.
 * @param table pointer to the hashtable.
 */
void printTable(Table *table);

/** deallocates Nodes in the hash table.
 * @param table pointer to the hashtable .
 */
void freeTable(Table *table);

/** prints the words in shorted using qsort.
 * @param table pointer to the hashtable for sorting.
 * @param size total number of elements in the hashtable.
 * @param n limit on how many lines to print out.
 */
void nodeArr(Table *table, int size, int n);

/** compare function for qsort.
 * @param a first element to compare.
 * @param b second element to compare.
 * @returns 1 if a is greater than b, -1 if b is greater, or 0 if both element are equal.
 */
int cmpr(const void *a, const void *b);
#endif

