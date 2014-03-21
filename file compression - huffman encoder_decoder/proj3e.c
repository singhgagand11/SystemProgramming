/**
 *  @file
 *  Header for stream functions
 *  <pre>CPE 357 - 01
 *  Project03 - File Encorder-Huffman
 *  -------------------
 *  Main Program for Encoding a File</pre>
 *  @author Gagandeep Singh
 */

 
#include "proj3e.h"

/** Runs the file encoder program. Takes in a file and encodes it
 *  it using huffman algrithm.
 *
 * @param argc the number of tokens on the input line.
 * @param argv an array of tokens.
 * @return 0 on success, 1-255 on failure
 */
int main(int argc, char *argv[]){
   int input, output;
   
   /* check for argc */
   if(argc < 2)
   {
      fprintf(stderr, "To few arguments\n");
      exit(EXIT_FAILURE);
   }
   /* open the file and create a fd */
   /*open file*/ 
   if((input = open(argv[1], O_RDONLY)) == -1)
   {
      fprintf(stderr, "Error opening File!\n");
   } 
   if(argc == 2){
      output =  STDOUT_FILENO;
   }else if(argc == 3){

      /*create and open file*/ 
      if((output = open(argv[2], O_CREAT | O_RDWR, S_IRWXU )) == -1)
      {
         fprintf(stderr, "Error creating and opening %s!\n", argv[2]);
         exit(EXIT_FAILURE);
      }
   }else{
      fprintf(stderr, "To many arguments\n");
      exit(EXIT_FAILURE);
   }
   /* compress the file */
   compress(input, output);

   /* close the input file */
   if( close(input) == -1)
   {
      fprintf(stderr, "Error closing input file!\n");
      exit(EXIT_FAILURE);
   }

   /* close the output/pipe file */
   if( close(output) == -1)
   {
      fprintf(stderr, "Error closing output file!\n");
      exit(EXIT_FAILURE);
   }
   exit(EXIT_SUCCESS);
}

/** compresses the open file and writes to the output file or
 * pipe directed by the parameters.
 *
 * @param readfd contains the open file discriptor of the file to read.
 * @param writefd contains the open file discriptor of the file to write to
 *   or the stdout.
 */
void compress(int readfd, int writefd)
{
   int fd, i, charCount, rlen, charInt;
   unsigned char ch;
   FreqTable myTable[TABLESIZE], mySort[TABLESIZE];
   Stream myStream; 
   LinkList myList;
   TreeNode *temp;
   HuffCode myCode;
   fd = readfd;

   /* intialize to new table */
   if(newTable(myTable, TABLESIZE) == -1) {
      fprintf(stderr, "Error creating table!\n");
      exit(EXIT_FAILURE); /*should never happen */
   } 

   /* intialize the stream */
   newStream(&myStream, writefd);

   /* intialize the TreeCode */
   newHuff(&myCode);

   /* create the freq table */
   addFreq(myTable,fd, TABLESIZE);

   /*set last character to EOF */
   myTable[TABLESIZE-1].letter = -1;
   myTable[TABLESIZE-1].count = 1;
   myTable[TABLESIZE-1].end = 1;


   for(i = 0; i < TABLESIZE; i++){
      mySort[i] = myTable[i];
   }
   qsort(mySort, 256, sizeof(FreqTable), InverCmpr);
   /* turn Freq into LinkList */
 
     /* track total number of char w/ the NULL*/
      charCount = -1;

   newList(&myList);
   for(i = 0; i < TABLESIZE; i++){
      if (mySort[i].count != 0){
         temp = (TreeNode*)malloc(sizeof(TreeNode));
         /* check malloc */
         if(temp == NULL){
            fprintf(stderr, "Error malloc couldn't allocate memory'!\n");
            exit(EXIT_FAILURE); /*should never happen */
         }
         temp->letter = mySort[i].letter;
         temp->count = mySort[i].count;
         temp->end = mySort[i].end;
         temp->leftChild = NULL;
         temp->rightChild = NULL;
         add(&myList, temp);
         charCount++;
      }
   }
 
   /* create a tree */
   convToTree(&myList); /* head point to the parent node */
   /* add the string code to the table */
    
   binCode(myTable, (myList.head)->parent, 0, 0);

   /* free the binary Tree */
   freeTree((myList.head)->parent);

   /* free the linked List */
   freeList(&myList);

   /*reset the lseek to beginning */
   if(lseek(readfd, 0, SEEK_SET) == -1)
   {
      fprintf(stderr, "Error seeking file!\n");
      exit(EXIT_FAILURE); /*should never happen */
   }
   /* add the header */

      /* write the total number of char */
      intToByte(charCount, &myStream);
 
      /* write the char in alpha order from table*/
      for(i = 0; i < TABLESIZE-1; i++){
         if (myTable[i].count > 0){
            charToByte(myTable[i].letter, &myStream);
            intToByte(myTable[i].count, &myStream);
      }
   }

   /* seek the file and decode */
   while(((rlen = read(fd, &ch, 1)) == 1)) {
      charInt = ch;
      addHuff(&myCode, &myStream, myTable[ch].binStr);
  }
  if (rlen == -1) {
    perror("Error reading File\n");
    exit(EXIT_FAILURE);
  }

   /* add eof */
   addHuff(&myCode, &myStream, myTable[TABLESIZE-1].binStr);
  
   /* empty out buffers */
   /* empty out buffers */
   flushHuff(&myCode, &myStream);
   flushStream(&myStream); 

   /* free string in the table */
   for(i = 0; i < TABLESIZE; i++){
      if (myTable[i].count > 0){
         free(myTable[i].binStr);
      }
   } 
}
