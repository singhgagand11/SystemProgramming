/**
 *  @file
 *  Header for stream functions
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------
 *  Header for stream.h</pre>
 *  @author Gagandeep Singh
 */

#ifndef STREAM
#define STREAM

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#define BUFFSIZE 1024

/**
 * @struct Stream
 * Contains a buffer for adding bytes, the index of 
 * how many bytes were added, and the output file discriptor
 * to write to.
 */
typedef struct Stream{
   unsigned char buff[BUFFSIZE];
   int index;
   int outfd;
}Stream; 

/**
 * @struct HuffCode
 * Contains a HuffCode buffer for adding bits and the index of 
 * how many bits were added.
 */
typedef struct HuffCode{
   unsigned char buff;
   int index;
}HuffCode;

/** Sets the HuffCode passes in to be initalized to NULL
 *  @param code HuffCode structure to initalize to NULL or zero.
 */
void newHuff(HuffCode *code);

/** Packs the huff code bits given by the char string and adds it to
 * HuffCode character buffer, if the char is filled the bits gets added to
 * the output Stream structure for output. If the code index is full (8bit) 
 * the code gets transfer to the output buffer and the buffer index is increased.
 *
 * @param code HuffCode structure to add bits to and increase the index.
 * @param stream the output stream to add bits in and increases its index.
 * full stream gets written and empty for processing the next data.
 * @param string string containing the character code of the huffman tree.
 */
void addHuff(HuffCode *code, Stream *stream, unsigned char *string);

/** Packs the huff code bits by adding zero and flushed the code buffer to
 * write the bits to the output stream. Called after all the files bits are
 * processed.       
 * @param code HuffCode structure to add bits to and increase the index.
 * @param stream the output stream to add bits in and increases its index.
 * full stream gets written and empty for processing the next data.
 */
void flushHuff(HuffCode *code, Stream *stream);

/** Sets the Stream structure passed in to be initalized. The buffer
 * gets set, index is set to zero, and output file discriptor is written.
 * @param stream Stream structure for writing output data
 */
void newStream(Stream *stream, int output);

/** Takes the unsigned char byte passed it and writes it to the output stream,
 * if the stream is full it gets written and index is reset.
 * @param stream Stream structure for writing output data
 * @param add the packed byte to add to the stream
 */
void addByte(Stream *stream, unsigned char add);

/** Flushes the output stream. This is called when all the data is processed and
 *  unwriiten data remains in the buffer.
 *
 * @param stream the output stream flush out the unprocessed data of.
 * The stream gets written and emptyed.
 */
void flushStream(Stream *stream);

#endif
