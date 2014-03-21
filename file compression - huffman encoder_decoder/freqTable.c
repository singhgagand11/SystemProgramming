/**
 *  @file
 *  Functions for Huffman Table, Tree, and List
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------</pre>
 *  @author Gagandeep Singh
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include "freqTable.h"

/** Sets the table passed elements in to be initalized to NULL or zero
 *
 *  @param table table structure to initalize.
 * @param size the size of the table.
 *  @returns 1 if the table was successfully initalizes, -1 otherwise. 
 */
int newTable(FreqTable *table, int size){
   int i;

   if(table == NULL)
      return -1;

  for(i = 0; i < size; i ++){
      table[i].count = 0;
      table[i].letter = '\0';
      table[i].binStr = NULL;
      table[i].end = 0;
   }
   return 1;
}

/** Prints the table passed in.
 *
 *  @param table table structure to print.
 *  @param size the size of the table.
*/
void printTable(FreqTable *table, int size){
   int i;
   if(table != NULL)
   {
       for(i = 0; i < size; i ++){
         printf("%d\t%0X %d %s\n", i, table[i].letter, table[i].count, table[i].binStr);
      }
   }
}

/** Reads the file and adds the character frequency to the table.
  *
  * @param table table structure to add character frequenct to.
  * @param size the size of the table.
  * @param fd open file discriptor to read words from
  * @param size the size of the table.
*/
void addFreq(FreqTable *table, int fd, int size){
   unsigned char buf[BUFF];
   int rlen = -1;
   int i;
   unsigned char charInt;
   while( (rlen = read(fd, buf, BUFF-1)) > 0)
   { 
         buf[rlen] = '\0';
         /* one to one mapping --*/ 
         for(i = 0; i < rlen; i++){
            charInt = buf[i]; 
            table[charInt].letter = charInt;
            table[charInt].count++;
        }
   }
   if(rlen == -1){
    printf("Error Reading File\n");
    exit(EXIT_FAILURE);
   }

}

/**
 * Acceding compare function for qsort.
 * @param a first element to compare.
 * @param b second element to compare.
 * @returns 1 if a is greater than b, -1 if b is greater, or 0 if both element are equal.
 */
int InverCmpr(const void *a, const void *b) { 

   FreqTable *one = (FreqTable*)a;
   FreqTable*two = (FreqTable*)b;
   if(one->count > two->count)
   {
      return -1;
   }
   else if((one-> count) <  (two->count))
   {
      return 1;
   }
   else{
      if(one->letter > two->letter)
      {
         return -1;
      }
      else if(one->letter <  two->letter)
      {
         return 1;
      }
      else
      {
         return 0;
      }
   }
}

/** Converts the ordered linked list and converts it to a tree following the huffman algorithm.
 *
 * @param list Link list to conver to huffman tree.
 */
void convToTree(LinkList *list){
   Node *first = NULL;
   Node *second = NULL;
   TreeNode *combine = NULL;

   while(list->size != 1){
      first = removeFirst(list);
      second = removeFirst(list);

      /* link the nodes */
     combine = (TreeNode*)malloc(sizeof(TreeNode));
     if(combine == NULL){
         fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
         exit(EXIT_FAILURE);
      }
      combine->count = abs(first->freq) + abs(second->freq);
      combine->leftChild = first->parent;
      combine->rightChild = second->parent;
      combine->letter = '\0';
      free(first);
      free(second);

      /* add the new treeNode */
      add(list, combine);
   }
   
}

/** Tranverse's the tree in postfix and if a leaf is found, a huffman code string
 * for that character is written to the table, used for encoding the file.
 *
 * @param table table structure to add huff tree code to.
 * @param root root of the tree to tranverse
 * @param code the tree code (contains left/right move made)
 * @param size the depth of tree tranvered to
 */
void binCode(FreqTable *table, TreeNode *root, int code, int size){
   unsigned char chr = 0;
   if(root->leftChild !=NULL)
      binCode(table, root->leftChild, code << 1, size + 1);
   if(root->rightChild != NULL)
      binCode(table, root->rightChild, code << 1 | 0x0001, size + 1);

   if(root->leftChild == NULL && root->rightChild == NULL){
      if((root->end) == 1){
       table[TABLESIZE-1].binStr = binString(code, size);
      }
      else{
         chr = root->letter;
         table[chr].binStr = binString(code, size); 
      }
   }
}

/** Creates a huffman tree code given the depth of the tree and its 
 *
 * @param table table structure to add character frequenct to.
 * @param size the size of the table.
 * @param number the tree code (contains left/right move made)
 * @param width the depth of tree tranvered to used to creating the width of the string.
 *
 * @return address of the string created for the leaf's huffcode
 */
unsigned char *binString(int number, int width){
   unsigned char *myStr;
   int i;
   int num = number;

   myStr = (unsigned char*)malloc(sizeof(unsigned char) * (width+1));
   if(myStr == NULL){
      fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
      exit(EXIT_FAILURE);
   }

   myStr[width] = '\0';

   for(i = width-1; i > -1; i--){
       if(num % 2 != 0){
         myStr[i] = '1';
         num--;
         num = num /2;
      }
      else{
         myStr[i] = '0';
         num = num /2;
      }
   }
   return myStr;
}

/** Prints the tree passed in.
 *
 * @param root Tree's root to tranverse and print out
 */
void printTree(TreeNode *root){
   if(root != NULL){
      if(root->leftChild !=NULL)
         printTree(root->leftChild);
      if(root->rightChild != NULL)
         printTree(root->rightChild);

      if((root->rightChild == NULL) && (root->leftChild == NULL))
         printf("%c %d\n",  root->letter, root->count);
   }
}

/** Free's all the elements of Node's of the tree
 * 
 * @param root root of the tree to tranverse and free
 */
void freeTree(TreeNode *root){
   if(root != NULL){
      if(root->leftChild !=NULL)
         freeTree(root->leftChild);
        if(root->rightChild != NULL)
         freeTree(root->rightChild);

      free(root);
   }
}
