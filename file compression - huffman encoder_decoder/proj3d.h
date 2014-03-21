/**
 *  @file
 *  Header for decoding file
 *  <pre>CPE 357 - 01
 *  Project03 - File decorder-Huffman
 *  -------------------
 *  Header for main Program for decoding a File</pre>
 *  @author Gagandeep Singh
 */

#ifndef PROJ3D
#define PROJ3D

#define LEFT 0
#define RIGHT 1
#define BITMASK 128

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>
#include "stream.h"
#include "linkedList.h" 
#include "freqTable.h"
#include "byteOpp.h"

/** de-compresses the open file and writes to the output file or
 *  pipe directed by the parameters.
 *
 * @param readfd contains the open file discriptor of the file to read.
 * @param writefd contains the open file discriptor of the file to write to
 *  or the stdout.
 */
void decompress(int readfd, int writefd);
#endif
