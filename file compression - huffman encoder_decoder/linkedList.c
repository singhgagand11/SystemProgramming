/**
 *  @file
 *  Functions for Link List
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------
 *  Functions for linkedlist.h</pre>
 *  @author Gagandeep Singh
 */
 
#include "linkedList.h" 

/** Adds a TreeNode structure to the list in order by frequency and if the frequency 
 *  are the same then its added by letter ordering.
 *
 *  @param list LinkList structure to add the TreeNode to.
 *  @param root TreeNode structure to add to the tree.
 */ 
void add(LinkList *list, TreeNode *root){
 
   Node *temp, *ptr, *prev ;

   if(list->head == NULL){
   temp = (Node*)malloc(sizeof(Node));

      if(temp == NULL){
         fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
         exit(EXIT_FAILURE); 
      }

      temp->parent = root;
      temp->freq = root->count;
      temp->next = NULL;
      list->head = temp;
   }
   else{
      ptr = list->head;
      prev = NULL;
      while(ptr->next != NULL && (ptr->freq < root->count)){
         prev = ptr;
         ptr = ptr->next;
      }
      /* hit the end of the list or count is < */

      if(ptr->freq > root->count){
         if(prev == NULL){/* add to the front - smalles value */
            temp = (Node*) malloc(sizeof(Node));

            if(temp == NULL){
               fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
               exit(EXIT_FAILURE); 
            }
            temp->parent = root;
            temp->freq = root->count;
            temp->next = ptr;
            list->head = temp; /* set the head of the list */
         }
         else{
            temp = (Node*) malloc(sizeof(Node));
            if(temp == NULL){
               fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
               exit(EXIT_FAILURE);
            }
            temp->parent = root;
            temp->freq = root->count;
            temp->next = ptr;   /*set temp next to pointer */
            prev->next = temp;  /* set prev next to temp */
         }
      }
      else{ /* freq is less than or = than root count */
               /* add to the end */
               if (ptr->freq == root->count){
               temp = (Node*) malloc(sizeof(Node));

               if(temp == NULL){
                  fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
                  exit(EXIT_FAILURE); 
               }
               temp->parent = root;
               temp->freq = root->count;
               temp->next = ptr;   /*set temp next to pointer */
               if(prev == NULL) /* head of the list*/
                  list->head = temp;  /* set prev next to temp */
               else
                  prev->next = temp;
            }
            else{ /* count > freq than */
               temp = (Node*) malloc(sizeof(Node));

               if(temp == NULL){
                  fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
                  exit(EXIT_FAILURE);
               }


               temp->parent = root;
               temp->freq = root->count;
               temp->next = ptr->next;   /*set temp next to pointer */
               ptr->next = temp;  /* set prev next to temp */
         }
      }
   }
   list->size++;
}

/** Creates a new LinkList with size set to zero and head to point to null.
 *
 *  @param list LinkList structure to initalize
 */  
void newList(LinkList *list){
   list->head = NULL;
   list->size = 0;
}

/** Removes the first two TreeNode from link list if tree size is greater than 1 
 * and and creates a New treeNode structure and sets the removes TreeNode to left
 * and right child.  
 *
 * @param list LinkList structure remove and add TreeNode structures to.
 * @return The new TreeNode created with childrens when successfull otherwise NULL.
 */ 
Node *removeFirst(LinkList *list){
   Node *toReturn = NULL;
   if(list->size > 0){
      toReturn = list->head;
      list->head = toReturn->next;
      toReturn->next = NULL; /* un link */
      list->size--;
   }
   return toReturn;
}


/** prints the link list. Used for testing purposes.
 *
 * @param list LinkList structure print out.
 */ 
void printList(LinkList *list){

   Node *temp = list->head;
   if(temp == NULL)
      printf("EMPTY");
   while(temp != NULL){
      printf("%d %0X\n", temp->freq, (temp->parent)->letter);
      temp = temp->next;
   }
}

/** Free's the list by deallocating the Nodes.
 *
 *  @param list LinkList structure to free.
 */
void freeList(LinkList *list)
{
         Node *ptr, *temp;
         ptr = temp = list->head;
         while(ptr != NULL)
         {
              temp = ptr->next;
              free(ptr);
              ptr = temp;
         }
}
