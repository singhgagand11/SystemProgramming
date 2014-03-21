/**
 *  @file
 *  Header for functions Huffman Table, Tree, and List
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------
 *  Header for freqTable.h</pre>
 *  @author Gagandeep Singh
 */

#ifndef FREQTABLE
#define FREQTABLE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include "linkedList.h"

/**
 * @def BUFF
 * buffer size for reading.
 */
#define BUFF 1024

/**
 * @def TABLESIZE
 * TABLESIZE for adding words and counting frequency.
 */
#define TABLESIZE 257

/**
 * @def LEFT
 * tranverse left of the tree
 */
#define LEFT 0

/**
 * @def RIGHT
 * tranverse right of the tree
 */
#define RIGHT 1

/**
 * @struct FreqTable
 * Table element for holding letter count, end to indicator for EOF, letter.
 * and the huff tree code.
 */
typedef struct FreqTable{
   int count;
   unsigned char letter;
   unsigned char *binStr;
   int end;
}FreqTable; 

/** Sets the table passed elements in to be initalized to NULL or zero
 *
 * @param table table structure to initalize.
 * @param size the size of the table.
 * @returns 1 if the table was successfully initalizes, -1 otherwise. 
 */
int newTable(FreqTable *table, int size);

/** Prints the table passed in.
 *
 * @param table table structure to print.
 * @param size the size of the table.
 */
void printTable(FreqTable table[], int size);

/** Reads the file and adds the character frequency to the table.
 *
 * @param table table structure to add character frequenct to.
 * @param size the size of the table.
 * @param fd open file discriptor to read words from
 * @param size the size of the table.
 */
void addFreq(FreqTable *table, int fd, int size);
 
/**
 * Acceding compare function for qsort.
 * @param a first element to compare.
 * @param b second element to compare.
 * @returns 1 if a is greater than b, -1 if b is greater, or 0 if both element are equal.
 */
int InverCmpr(const void *a, const void *b);

/** Converts the ordered linked list and converts it to a tree following the huffman algorithm.
 *
 * @param list Link list to conver to huffman tree.
 */
void convToTree(LinkList *list);

/** Prints the tree passed in.
 *
 * @param root Tree's root to tranverse and print out
 */
void printTree(TreeNode *root);


/** Tranverse's the tree in postfix and if a leaf is found, a huffman code string
 * for that character is written to the table, used for encoding the file.
 *
 * @param table table structure to add huff tree code to.
 * @param root root of the tree to tranverse
 * @param code the tree code (contains left/right move made)
 * @param size the depth of tree tranvered to
 */
void binCode(FreqTable *table, TreeNode *root, int code, int size);

/** Creates a huffman tree code given the depth of the tree and its 
 *
 * @param table table structure to add character frequenct to.
 * @param size the size of the table.
 * @param number the tree code (contains left/right move made)
 * @param width the depth of tree tranvered to used to creating the width of the string.
 *
 * @return address of the string created for the leaf's huffcode
 */
unsigned char *binString(int number, int width);

/** Free's all the elements of Node's of the tree
 * 
 * @param root root of the tree to tranverse and free
 */
void freeTree(TreeNode *root);

#endif
