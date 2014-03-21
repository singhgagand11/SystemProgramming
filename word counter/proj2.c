/**
 *  @file
 *  counts the words in directory and display's them in order by frequency
 *  <pre>CPE 357- 01  
 *  Project02 - Word counter
 *  -------------------
 *  counts the words in directory and display's them in order by frequency</pre>
 *  @author Gagandeep Singh
 */


#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ftw.h>
#include <ctype.h>
#include "myhash.h"
#include "proj2.h" 

/**
 * @def BUFF
 * buffer size for reading.
 */

#define BUFF 1024
 
/** Runs the word counter program by reading argv for dictorary name 
 *  and prints out the word by frequency.
 *
 * @param argc the number of tokens on the input line.
 * @param argv an array of tokens.
 * @return 0 on success, 1-255 on failure
 */

int main(int argc, char *argv[])
{
   int  fd, i, rlen, nLimit, firstChar, len;
   char *pathname;
   DIR  *dir;
   struct dirent *currFile; 
   struct stat buf;
   char buff[BUFF];
   char lastChar[BUFF] = " ";
   char fullPath[BUFF]; 
   Table table[TABLESIZE];
   char save[BUFF];
   char * pch;
   char *last;
        
      
   firstChar = -1; /* state for first and last char in buffer read to indicate word break */
 
   /* creates the table and sets elements to NULL and size to 0 */
   for (i = 0; i < TABLESIZE+1; i++){
      table[i].head = NULL;
      table[i].size = 0;
   }
   
   /* check for # of arguments passed in */
   if(argc < 3)
   {
      fprintf(stderr, "To few arguments\n");
      exit(EXIT_FAILURE);         
   }
   nLimit = (int)atol(argv[1]); /* check for failure */
   if(nLimit <= 0)
   {
      fprintf(stderr, "Invalid n's argument\n");
      exit(EXIT_FAILURE);   
   }
   
   pathname = argv[2];  /* save the path of the directory passed in */
   
   /* OPEN DIRECTORY AND CHECK FOR FAILURE */
   if( (dir = opendir(pathname)) == NULL)
   {
      perror("ERROR OPENING DIR");
      exit(EXIT_FAILURE);
   }
   /* the main business: read in the files in the directory */
   while((currFile = readdir(dir)) != NULL)
   {
          /* create the full pathname */
            strcpy(fullPath, pathname);
            strcat(fullPath, "/");
            strcat(fullPath, currFile->d_name);
            lstat(fullPath, &buf);
            
         /* check for regular files */
         if((S_ISREG(buf.st_mode)  != 0))
         {
            /*open file and Check for error's */ 
            if((fd = open(fullPath, O_RDONLY)) == -1)
            {
               fprintf(stderr, "Error opening %s!\n", currFile->d_name);
               
            }  
            else
            {
               /* read the file */
               while( (rlen = read(fd, buff, BUFF-1)) > 0)
               { 
                     buff[rlen] = '\0';
                     for(i = 0; i < rlen; i++)
                     {
                        if( !isalpha(buff[i]) ){
                              buff[i] = ' ';
                           }
                           else{
                              buff[i] = tolower(buff[i]);
                           }
                     }
                     if(buff[0] == ' ')
                       {
                        len = strlen(lastChar);
                        if(len > 3){
                        strncpy(save, lastChar, len);
                        save[len] = '\0';
                        if( len > 20){
                           changeLen(table, save);
                        }
                        else
                        add(table, save);
                        }
                        strcpy(lastChar, "");
                     }
                     
                     if(buff[rlen-1] == ' ' )
                     {
                        firstChar = 1;
                     }
                     pch = strtok(buff," ");
                     last = pch;
                     

                    while (pch != NULL)
                    { 
                      last = pch;
                      
                      pch = strtok(NULL, " ");                    
                      if(pch != NULL)
                      {
                         strcat(lastChar, last);
                          len = strlen(lastChar);
                         if(len > 3){
                        strncpy(save, lastChar, len);
                        save[len] = '\0';
                        if(len > 20){
                           changeLen(table, save);
                        }
                        else
                        add(table, save);
                        }
                         strcpy(lastChar, "");
                      }
                    
                    }
                    if(firstChar == 1){
                       strcpy(lastChar, last);
                        len = strlen(lastChar);
                       if(len > 3){
                        strncpy(save, lastChar, len);
                        save[len] = '\0';
                        if(len > 20){
                           changeLen(table, save);
                        }
                        else
                        add(table, save);
                        }
                       strcpy(lastChar, "");
                       firstChar = 0;
                      }else
                    strcpy(lastChar, last);                     
               }
               if(rlen == -1) /* check if there was any reading error */
               {     
                   printf("Error Reading: %s\n", fullPath);
                   exit(EXIT_FAILURE);
               }        
               /* close the file and Check for error */
               if( close(fd) == -1)
               {
                  fprintf(stderr, "Error closing %s!\n", currFile->d_name);  
               }
            }
         }
   }
   nodeArr(table, table[0].size, nLimit);  /* print the sorted table */
 
   freeTable(table); /* free the nodes */
 
   /* close directory */
   if(closedir(dir) == -1)
   {
      perror("ERROR CLOSING");
      exit(EXIT_FAILURE);
   }         

   exit(EXIT_SUCCESS);
}
/** converts longer strings to 20 characters followed by
 *  6 digit string length and adds it to the hash table.
 * @param table hash table to add word to.
 * @param string the string to be added.
 */
void changeLen(Table *table, char *string)
{
   char save[BUFF];
   char digitNum[6];
   char savLen[6];
   
   strncpy(save, string, 20);

   sprintf(digitNum, "%06d", (int)strlen(string)); /* need to change pointer to be writable */
   strcpy(savLen, digitNum);
   strcat(save, savLen);

   add(table, save);
}
