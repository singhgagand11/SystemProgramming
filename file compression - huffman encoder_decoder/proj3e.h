/**
 *  @file
 *  Header for encoding file
 *  <pre>CPE 357 - 01
 *  Project03 - File encorder-Huffman
 *  -------------------
 *  Header for main Program for encoding a File</pre>
 *  @author Gagandeep Singh
 */

#ifndef PROJ3E
#define PROJ3E

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include "linkedList.h" 
#include "freqTable.h"
#include "stream.h"
#include "byteOpp.h" 

/** compresses the open file and writes to the output file or
 * pipe directed by the parameters.
 *
 * @param readfd contains the open file discriptor of the file to read.
 * @param writefd contains the open file discriptor of the file to write to
 *  or the stdout.
 */
void compress(int readfd, int writefd);


#endif
