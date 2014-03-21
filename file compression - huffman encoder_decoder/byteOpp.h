/**
 *  @file
 *  Header for byte opperations 
 *  <pre>CPE 357 - 01
 *  Project03 - File Compressor-Huffman
 *  -------------------
 *  Header for byteOpp.h</pre>
 *  @author Gagandeep Singh
 */

#ifndef BYTEOPP
#define BYTEOPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include "stream.h"

/**
 * @def BUFF
 * Masking for reading bits.
 */
#define MASK 0xff

/** converts interger to bytes and adds the bytes to the output stream.
 *
 * @param interger value to convert to byte.
 * @param stream output stream to write the bytes into.
 * @returns 0 that the byte was successfully entered.
 */
int intToByte(int integer, Stream *stream); 

/** Takes a unsigned character and adds it to the output stream.
 *
 * @param add character of byte to add to the stream.
 * @param stream output stream to write the byte into.
 * @returns 0 that the byte was successfully entered.
 */
int charToByte(unsigned char add, Stream *stream);

/** converts a bytes back into a interger. Used for heading the header.
 *
 *  @param readfd file descriptor to read the byte from.
 *  @returns the integer that was calculated. 
 */
int byteToInt(int readfd);
#endif
