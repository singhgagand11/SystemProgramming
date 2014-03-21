/**
 *  @file
 *  Functions for byte opperations 
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------
 *  functions for byteOpp.h</pre>
 *  @author Gagandeep Singh
 */

#include "byteOpp.h"
/** converts interger to bytes and adds the bytes to the output stream.
 *
 *  @param interger value to convert to byte.
 *  @param stream output stream to write the bytes into.
 *  @returns 0 that the byte was successfully entered.
 */
   int intToByte(int integer, Stream *stream){
   int i = integer;
   int j;
   unsigned char add;

   for(j = 0; j < 4; j++){
      add = (i & MASK);
      addByte(stream, add);
      i = i >> 8;
   }
   return 0;
}

/** converts a bytes back into a interger. Used for heading the header.
 *
 *  @param readfd file descriptor to read the byte from.
 *  @returns the integer that was calculated. 
 */
int byteToInt(int readfd){

   int rlen, count, i;
   unsigned char buf[4];
   if((rlen = read(readfd, buf, 4)) == 4) { 
         count = 0;
         for(i = 3; i > -1; i--){
            count = count << 8;
            count |= buf[i];
      }
   }
   if(rlen < 4 && rlen > -1){
      fprintf(stdout,"Error Reading File, short read\n");
      exit(EXIT_FAILURE);
   }
   if(rlen == -1){     
    fprintf(stdout,"Error Reading File\n");
    exit(EXIT_FAILURE);
   } 

   return count;
}

/** Takes a unsigned character and adds it to the output stream.
 *
 *  @param add character of byte to add to the stream.
 *  @param stream output stream to write the byte into.
 *  @returns 0 that the byte was successfully entered.
 */
int charToByte(unsigned char add, Stream *stream){

   addByte(stream, add);

   return 0;
}
