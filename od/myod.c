/*
 * CPE 357 - 01
 * Project 01 - myod: clone of od
 * Gagandeep Singh 
 *
 */

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "myprintout.h"


/*
 * Main: reads in the command line and creates print octal dump of file
 * @param argc count of command-line tokens.
 * @param argv array of command-line tokens.
 * @return 0 on success, 1-255 on failure.
 *
*/
int main(int argc, char *argv[])
{
   long jOffset = 0;
   long nLimit = -1;
   int c = 0;
   int byteRead = 0;
   int i=0;
   int readArg;
   char *endptr = NULL;
   int saveChar[16];
   char *fileName; 
   FILE *readFile = stdin;
   errno = 0; 
 
    
   /*check argc for parameters for -N and -j flags */
   while ( (readArg = getopt(argc, argv, "N:j:")) != -1) {
        switch (readArg) {
        case 'j': 
               jOffset = (int)strtol(optarg,  &endptr, 10);
               /* Checks if jOffset is a valid parameter */
               if ((*endptr) != '\0' || jOffset < 0) 
               {
                 fprintf(stderr, "od: invalid suffix in -j argument `%s'\n", optarg);         
                 exit(EXIT_FAILURE);
               }

               if ((errno != 0 && (jOffset == LONG_MAX || jOffset == LONG_MIN)) ||
                           (jOffset > INT_MAX || jOffset < INT_MIN))
               {
                  fprintf(stderr, "od: invalid suffix in -j argument `%s'\n", optarg);  
                  exit(EXIT_FAILURE);
               }
            break;
        case 'N':
            nLimit = (int)strtol(optarg, &endptr, 10);
            /* Checks if nLimit is a valid parameter */
             if ((*endptr) != '\0' || nLimit < 0) 
               {
                 fprintf(stderr, "od: invalid suffix in -N argument `%s'\n", optarg);       
                  exit(EXIT_FAILURE);
               }

               if ((errno != 0 && (nLimit == LONG_MAX || nLimit == LONG_MIN)) ||
                           (nLimit > INT_MAX || nLimit< INT_MIN))
               {
                  fprintf(stderr, "od: invalid suffix in -N argument `%s'\n", optarg);   
                   
                  exit(EXIT_FAILURE);
               }
            break;
        case '?':
            fprintf(stderr, "Invalid Argument\n");
             exit(EXIT_FAILURE);
            break;
        case ':':
            fprintf(stderr, "Invalid Argument\n");
             exit(EXIT_FAILURE);
        default:
            printf ("?? getopt returned character code 0%o ??\n", readArg);
        }
    }
   /*check arugement for file*/

   /*open and read file */
   if(argv[optind] != NULL)
   {
      fileName = argv[argc-1];
      readFile = fopen(fileName, "r");

      /*check if file was found and open*/
      if(readFile == NULL)
      {
         fprintf(stderr, "ERROR OPENING FILE!\n");  
         exit(EXIT_FAILURE);
      }
   }
    
   /* move the starting position of the read in file by jOffet Value */
   while(jOffset  !=0 && (c = fgetc(readFile)) != EOF)
   {
     jOffset --;
     byteRead++;
   }
   /*CHECK FOR ERROR HANDELING */
   if(ferror(readFile))
   {
      fprintf(stderr, "ERROR READING FILE!\n");
      exit(EXIT_FAILURE);
   }
   
   if (fflush(stdout)== EOF) 
   {
    fprintf(stderr, "ERROR\n"); 
    exit(EXIT_FAILURE);
   }
   if( c == EOF)
   {
      fprintf(stderr, "od: cannot skip past end of combined input\n"); 
      exit(EXIT_FAILURE);
   }
   /* Nothing to display */
   if(nLimit == 0)
   {  
      printf("%06x\n", byteRead);
      exit(EXIT_SUCCESS);
   }
   


/*
 *read the char and check for errors or EOF, also checks the nLimit
 *to see how many character to display out to the screen.
 *
 */ 
   while((c = fgetc(readFile)) != EOF)
   {
      byteRead++;
      saveChar[i] = c;
      i++;
      if(i == 16)
      {
 
         printOut(saveChar, byteRead - i , 16);
         i = 0;
      }
      if(nLimit > 0)
      {
         nLimit--;  
      }
      if(nLimit == 0)
      {
         break;
      }      
   }
   if(ferror(readFile))
   {
      fprintf(stderr, "ERROR READING FILE!\n");
      exit(EXIT_FAILURE);
   }
   else if (fflush(stdout) == EOF) 
   {
      fprintf(stderr, "ERROR\n"); 
      exit(EXIT_FAILURE);
   }
   else
   {
      /*print out the character read in and saved out to the screen.*/
      if(i != 0)
      {
         printOut(saveChar, byteRead - i, i);
      }
      printf("%06x\n", byteRead); /* print out total byte read in. */
      /* check if stdin char was successfully written */
      if (fflush(stdout) == EOF) 
      {
         fprintf(stderr, "ERROR\n"); 
         exit(EXIT_FAILURE);
      }
  
   }

   /*close file*/
   fclose(readFile);
   exit(EXIT_SUCCESS);
}

/*
 * This function prints out the octal value and the character read 
 * in using the format similar to od -Ax -bc command. The function
 * returns void and takes in three arguments saveChar[]: read in 
 * characters from stdin or file, byteRead : number of byte read,
 * and numtoPrint of characters to print out.
 * @param saveChar is the character to print out
 * @param byteRead is the total number of byte read
 * @param numtoPrint is the number of characters to print out.
 */
void printOut(int saveChar[], int byteRead, int numtoPrint)
{
      int i, j;
      i = numtoPrint;
      printf("%06x", byteRead);
      /*print out the numbers*/
      for(j = 0; j < numtoPrint; j++)
      {
        printf(" %03o", saveChar[j]);

      }
      printf("\n");
      /*print out the char*/
      printf("%6s", "");
      for(j = 0; j < i; j++)
      {
         if(saveChar[j] == '\0')
         {
            printf("  \\0");
         }
         else if(saveChar[j] == '\a')
         {
            printf("  \\a");
         }
         else if(saveChar[j] == '\b')
         {
            printf("  \\b");
         }
         else if(saveChar[j] == '\t')
         {
            printf("  \\t");
         }
         else if(saveChar[j] == '\n')
         {
            printf("  \\n");
         }
         else if(saveChar[j] == '\v')
         {
            printf("  \\v");
         }
         else if(saveChar[j] == '\f')
         {
            printf("  \\f");
         }
         else if(saveChar[j] == '\r')
         {
            printf("  \\r");
         }
         else if(saveChar[j] >= ' ' && saveChar[j] <= '~')
         {
            printf(" %3c", saveChar[j]);
         }
         else
         {
            printf(" %03o", saveChar[j]);
         }
      }
      printf("\n");
      
}
