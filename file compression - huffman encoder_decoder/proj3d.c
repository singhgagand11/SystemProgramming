/**
 *  @file
 *  Header for decoding file
 *  <pre>CPE 357 - 01
 *  Project03 - File decorder-Huffman
 *  -------------------
 *  Main Program for decoding a File</pre>
 *  @author Gagandeep Singh
 */

#include "proj3d.h"

/** Runs the file decoder program. Takes in a file and decodes it
 *  it using huffman algrithm.
 *
 * @param argc the number of tokens on the input line.
 * @param argv an array of tokens.
 * @return 0 on success, 1-255 on failure
 */
int main(int argc, char *argv[]){

   int input, output;
   /* No file input / output case */
   /* if input is file input gets file string,
      stdin = -1, else input is null and stdin = STDIN_FILENO */
   if(argc == 1){
      output = STDOUT_FILENO;
      input = STDIN_FILENO;
   }else if(argc == 2){
      if((strcmp(argv[1], "-")) == 0){ /* input from console */
         input = STDIN_FILENO;
         output = STDOUT_FILENO;
      } else{
         if((input = open(argv[1], O_RDONLY)) == -1){  /* file input .. open it */
            fprintf(stderr, "Error opening File %s!\n", argv[1]);
            exit(EXIT_FAILURE);
         }
         output = STDOUT_FILENO;
      }
   }else if(argc == 3){
      if((strcmp(argv[1], "-")) == 0){ /* input from console */
         input = STDIN_FILENO;
         /*create and open file*/
         if((output = open(argv[2], O_CREAT | O_RDWR, S_IRWXU )) == -1){
            fprintf(stderr, "Error creating and opening %s!\n", argv[2]);
            exit(EXIT_FAILURE);
         }
         } else{
         /* file input ... open both*/
         if((input = open(argv[1], O_RDONLY)) == -1){
            fprintf(stderr, "Error opening File %s!\n", argv[1]);
            exit(EXIT_FAILURE);
         }
         /*create and open file*/
         if((output = open(argv[2], O_CREAT | O_RDWR, S_IRWXU )) == -1){
            fprintf(stderr, "Error creating and opening %s!\n", argv[2]);
            exit(EXIT_FAILURE);
         }
      }
   }
   else{
      fprintf(stderr, "To many arguments\n");
      exit(EXIT_FAILURE); 
   }

   decompress(input, output);
   /* close files */

   /* close the input file */
   if( close(input) == -1)
   {
      fprintf(stderr, "Error closing input file!\n");
      exit(EXIT_FAILURE); 
  }
   /* if it write to terminal then add a new line after writing */
 
   /* close the output/pipe file */
   if( close(output) == -1)
   {
      fprintf(stderr, "Error closing output file!\n");
      exit(EXIT_FAILURE);
   }
   exit(EXIT_SUCCESS);
}

/** de-compresses the open file and writes to the output file or
 * pipe directed by the parameters.
 *
 * @param readfd contains the open file discriptor of the file to read.
 * @param writefd contains the open file discriptor of the file to write to
 *  or the stdout.
 */
void decompress(int readfd, int writefd){

   FreqTable myTable[TABLESIZE], mySort[TABLESIZE];
   Stream myStream;
   unsigned char  ch;
   int i, charCount, freq, rlen, end;
   LinkList myList;
   TreeNode *temp, *travT;

   /* intialize the stream */
   newStream(&myStream, writefd);

   /* intialize to new table */
   if(newTable(myTable, TABLESIZE) == -1) {
      fprintf(stderr, "Error creating table!\n");
      exit(EXIT_FAILURE); /*should never happen */
   } 

   /*create the table */
   /* get total char count */
   charCount = byteToInt(readfd);

   i = 0;
   /* create the freq table */
   while(i != charCount){
      if(read(readfd, &ch, 1) != 1) {
         perror("Error reading File\n");
         exit(EXIT_FAILURE);
      }
      freq = byteToInt(readfd);
      myTable[ch].letter = ch;
      myTable[ch].count = freq;
      i++;
   }

   /*set last character to EOF */
   myTable[TABLESIZE-1].letter = -1;
   myTable[TABLESIZE-1].count = 1;
   myTable[TABLESIZE-1].end = 1;

   for(i = 0; i < TABLESIZE; i++){
      mySort[i] = myTable[i];
   }
   qsort(mySort, 256, sizeof(FreqTable), InverCmpr);
   /* turn Freq into LinkList */

   newList(&myList);
   for(i = 0; i < TABLESIZE; i++){
      if (mySort[i].count != 0){
         temp = (TreeNode*)malloc(sizeof(TreeNode));
         /* check malloc */
         if(temp == NULL){
            fprintf(stdout, "Error malloc couldn't allocate memory'!\n");
            exit(EXIT_FAILURE); /*should never happen */
         }
         temp->letter = mySort[i].letter;
         temp->count = mySort[i].count;
         temp->end = mySort[i].end;
         temp->leftChild = NULL;
         temp->rightChild = NULL;
         add(&myList, temp);
      }
   }

   /* create a tree */
   convToTree(&myList); /* head point to the parent node */
 
   travT = (myList.head)->parent;

   /* travese the tree only if the tree exist..for empty file it is */
   end = 1; /* use to break while loop */
   /* read the file, checks for EOF and the list is greater than 1 */
   /* size of 1 means EOF exist and file is empty */
   while( ((rlen = read(readfd, &ch, 1)) == 1) && end && (myList.head)->freq != 1) {
      for(i = 0; i < 8; i++){
         if((ch & BITMASK) == 0){
            travT = travT->leftChild;
         }
         else{
            travT = travT->rightChild;
         }
         /* check if we reached a leaf */
         if( (travT->rightChild == NULL) && (travT->leftChild == NULL)){
            if(travT->end == 1){ /* eof */
               end = 0; /* break the while loop */
               i = 9; /* break the for loop */
            }
            else{
               addByte(&myStream, travT->letter);
               travT = (myList.head)->parent;
               }
         }
         ch = (ch << 1);
      }
   }
   if (rlen == -1) {
      perror("Error reading File\n");
      exit(EXIT_FAILURE);
   }
   flushStream(&myStream);
   /* free the  Tree */
   freeTree((myList.head)->parent);
   /* free the linked List */
   freeList(&myList);
}
