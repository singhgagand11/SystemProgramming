/**
 *  @file
 *  Functions for hash table.
 *  <pre>CPE 357 - 01
 *  Project02 - Word counter
 *  -------------------
 *  contains function for hash table</pre>
 *  @author Gagandeep Singh
 */

 

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include "myhash.h" 




/** adds the word into the hash table.
 * @param table pointer to the hashtable for adding words to.
 * @param name word to be added into the hash table.
 */ 
void add(Table *table, char *name)
{
   int index;
   index = hash(name);
   /* create new node */
   if (table[index].head == NULL)
   {
     Node *temp = (Node*) malloc(sizeof(Node));
     strcpy(temp->word, name);
     temp->count = 1;
     temp->next = NULL;
     table[index].head = temp;
     table[0].size++;
   }
   else
   {
      Node *ptr = table[index].head;
      while ( (ptr->next != NULL) && (strcmp(ptr->word, name) != 0) )
      {
           ptr = ptr->next;
      }
      if(strcmp(ptr->word, name) == 0)
      {
         ptr->count++; 
      }
      else
      {
         Node *temp = (Node*) malloc(sizeof(Node));
         strcpy(temp->word, name);
         temp->count = 1;
         temp->next = NULL;
         ptr->next = temp;
         table[0].size++;
      }  
   }
}

/** deallocates Nodes in the hash table.
 * @param table pointer to the hashtable .
 */
void freeTable(Table *table)
{
   int i;
   if(table != NULL)
   { 
      for(i = 0; i < TABLESIZE; i ++)
      {
         Node *ptr, *temp;
         ptr = temp = table[i].head;
         while(ptr != NULL)
         {
              temp = ptr->next;
              free(ptr);
              ptr = temp;
         }
      } 
   }
}


/** hash key creator.
 * @param str word used for keying.
 * @returns the key.
 */
int hash(char *str)
{
int hashVal = 0;
int i = 0;
/* online */
   for(i = 0; i< strlen(str); i++)
      hashVal = 37*hashVal+str[i];

   hashVal %= TABLESIZE;
   if(hashVal<0)
      hashVal += TABLESIZE;

return hashVal;

}

/** prints the hash table.
 * @param table pointer to the hashtable.
 */
void printTable(Table *table)
{
   int i;
   int count = 1;
   Node *ptr;
   for(i = 0; i < TABLESIZE; i++)
   {  
      printf("%d ", i);
      ptr = table[i].head;
      if(table[i].head != NULL)
      {
         while(ptr != NULL)
         {
            printf("%s %d. ", ptr->word, ptr->count);
            ptr = ptr->next;
            count ++;
         }
      }
      printf("\n");
   }
}

/** prints the words in shorted using qsort.
 * @param table pointer to the hashtable for sorting.
 * @param size total number of elements in the hashtable.
 * @param n limit on how many lines to print out.
 */
void nodeArr(Table *table, int size, int n)
{
   Node *last = (Node*) malloc(sizeof(Node) * size);
   int i, j;
   Node *ptr;
   j = 0;
    
   for(i = 0; i < TABLESIZE; i++)
   {  
      ptr = table[i].head;
      if(table[i].head != NULL)
      {
         while(ptr != NULL)
         {
            strcpy(last[j].word, ptr->word);
            last[j].count =  ptr->count;
            j++;
            ptr = ptr->next;
         }
      }
   }
   
   if(n > size)
     n = size;
    /* qsort array of structures */
   qsort(last, size, sizeof(*last), cmpr);
  for(j = (size - n); j < size ; j++)
   {
      printf("%26s %d\n", last[j].word, last[j].count);
   }
   free(last);
}

/** compare function for qsort.
 * @param a first element to compare.
 * @param b second element to compare.
 * @returns 1 if a is greater than b, -1 if b is greater, or 0 if both element are equal.
 */
int cmpr(const void *a, const void *b) { 

   Node *one = (Node*)a;
   Node *two = (Node*)b;
   if(one->count > two->count)
   {
      return 1;
   } 
   else if((one-> count) <  (two->count))
   {
      return -1;
   }
   else
   {
      return (strcmp(one->word, two->word));
    
   }
}
