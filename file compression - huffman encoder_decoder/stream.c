/**
 *  @file
 *  Functions for stream functions
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------
 *  Functions for stream.h</pre>
 *  @author Gagandeep Singh
 */
 
#include "stream.h"

/** Sets the Stream structure passed in to be initalized. The buffer
    gets set, index is set to zero, and output file discriptor is written.
    @param stream Stream structure for writing output data
 */
void newStream(Stream *stream, int output){

   int i;
   /* clear the stream */
   for(i = 0; i < BUFFSIZE-1; i++){
      stream->buff[i] = ' ';
   }
   stream->buff[BUFFSIZE] = '\0';
   stream->index = 0;
   stream->outfd = output;
}

/** Takes the unsigned char byte passed it and writes it to the output stream,
 *   if the stream is full it gets written and index is reset.
 * 
 * @pram stream Stream structure for writing output data
 * @param add the packed byte to add to the stream
 */
void addByte(Stream *stream, unsigned char add){

   if(stream->index == BUFFSIZE-1){
      flushStream(stream);
   }
   stream->buff[stream->index] = add;
   stream->index++;
}

/** Sets the HuffCode passes in to be initalized to NULL
 *   @param code HuffCode structure to initalize to NULL or zero.
 */
void newHuff(HuffCode *code){
   code->buff = 0;
   code->index = 0;
}

/** Packs the huff code bits given by the char string and adds it to
 *  HuffCode character buffer, if the char is filled the bits gets added to
 *  the output Stream structure for output. If the code index is full (8bit) 
 *  the code gets transfer to the output buffer and the buffer index is increased.
 *
 * @param code HuffCode structure to add bits to and increase the index.
 * @param stream the output stream to add bits in and increases its index.
 *  full stream gets written and empty for processing the next data.
 * @param string string containing the character code of the huffman tree.
 */
void addHuff(HuffCode *code, Stream *stream, unsigned char *string){
   int i;
   for(i = 0; string[i] != '\0'; i++){
      code->buff = (code->buff << 1);
      if(string[i] == '1')
         code->buff |= 1;
      else
         code->buff |=0;
      code->index++;

      if(code->index == 8){
         addByte(stream, code->buff);
         code->buff = 0;
         code->index = 0;
      }
   }
}

/** Packs the huff code bits by adding zero and flushed the code buffer to
 *  write the bits to the output stream. Called after all the files bits are
 *  processed.       
 *
 * @param code HuffCode structure to add bits to and increase the index.
 * @param stream the output stream to add bits in and increases its index.
 * full stream gets written and empty for processing the next data.
 */
void flushHuff(HuffCode *code, Stream *stream){

   if(code->index != 0){
      code->buff = code->buff << (8-code->index);
      addByte(stream, code->buff);
      code->buff = 0;
      code->index = 0;
   }
}

/** Flushes the output stream. This is called when all the data is processed and
 *  unwriiten data remains in the buffer.
 *
 * @param stream the output stream flush out the unprocessed data of.
 * The stream gets written and emptyed.
 */
void flushStream(Stream *stream){
   int wlen;

   wlen = write(stream->outfd, stream->buff, stream->index);

   if(wlen != stream->index){
      fprintf(stderr, "Error writing\n");
      exit(EXIT_FAILURE); 
   }
   stream->index = 0;
}
