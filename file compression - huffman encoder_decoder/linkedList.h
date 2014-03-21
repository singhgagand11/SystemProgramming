/**
 *  @file
 *  Header for functions Link List
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------
 *  Header for linkedlist.h</pre>
 *  @author Gagandeep Singh
 */

#ifndef LINKEDLIST
#define LINKEDLIST

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

/**
 * @struct Node
 * Node element that hold the total children's frequency.
 * Has a parent TreeNode pointer and pointer to the next element in the Linklist.
 */
typedef struct Node{
   int freq;
   struct TreeNode *parent;
   struct Node *next;
}Node;

/**
 * @struct TreeNode
 * Node element that hold the letter and its frequency.
 * Has left and right TreeNode pointer
 */
typedef struct TreeNode{
   unsigned letter;
   int count;
   int end;
   struct TreeNode *leftChild;
   struct TreeNode *rightChild;
}TreeNode;

/**
 * @struct LinkList
 * List structure that hold the total size of the list and the head of the list.
 */
typedef struct LinkList{
   int size;
   Node *head;
}LinkList;

/** Creates a new LinkList with size set to zero and head to point to null.
 *
 *  @param list LinkList structure to initalize
 */ 
void newList(LinkList *list);

/** Adds a TreeNode structure to the list in order by frequency and if the frequency 
 *  are the same then its added by letter ordering.
 *
 *  @param list LinkList structure to add the TreeNode to.
 *  @param root TreeNode structure to add to the tree.
 */ 
void add(LinkList *list, TreeNode *root);

/** prints the link list. Used for testing purposes.
 *
 *  @param list LinkList structure print out.
 */ 
void printList(LinkList *list);

/** Removes the first two TreeNode from link list if tree size is greater than 1 
 *  and and creates a New treeNode structure and sets the removes TreeNode to left
 *  and right child.  
 *
 *  @param list LinkList structure remove and add TreeNode structures to.
 *  @return The new TreeNode created with childrens when successfull otherwise NULL.
 */ 
Node *removeFirst(LinkList *list);

/** Free's the list by deallocating the Nodes.
 *
 *  @param list LinkList structure to free.
 */ 
void freeList(LinkList *list);

#endif
