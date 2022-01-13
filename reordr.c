/*****************************************************************
 * reordr.c -- reorder.c sort some MAXIMA output.
 *
 * Copyright 2018 Jesse I. Deutsch
 *
 * $Id: reordr.c,v 1.6 2018/03/20 09:06:55 jdeutsch Exp $
 *****************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reordr.h"
#include "block.h"
#include "mgir03.h"


#define  FALSE         0
#define  TRUE          1
typedef  int           BOOL;
#define  MAXNAME       20            /* maximum chars in a name */
#define  MAXTERMS      75            /* maximum number of terms */


     /******** globals in this file  ********/
char   *program_name;         /* name of the program (for errors) */

                              /* functions in this file           */
BOOL   trim_string (char *string);
BOOL   sort_list_by_monmon (PDATA_RDR apList[], int nNum);
BOOL   print_array_of_terms (DATA_RDR **ppA, int count);



/*************** main -- entry point **********************/
int main(int argc, char * argv[])
{
   FILE     *in_file;                  /* input file                */
   char     *InFileName = argv [1];    /* name of input file        */
   char      line [512];
   char     *pRemain;                  /* remainder of input string */ 
   size_t    nSpanLen;                 /* number of char's spanned  */
   int       count = 0;
   int       j;
   PDATA_RDR    pDbTemp_RDR;
   PDATA_RDR    apTermList [MAXTERMS];


   void usage(void);            /* tell user how to use the program */

   printf("\n\t REORDR Program Copyright 2018 Jesse I. Deutsch\n");   

                                /* save the program name for future use */
   program_name = argv[0];

   /*
    * loop for each option.
    *   Stop if we run out of arguments
    *   or get an argument without a dash.
    */

   while ( (argc > 1) && ( argv[1][0] == '-' || argv[1][0] == '/') ) {
                 /*
                  * argv[1][1] is the actual option character
                  */
       switch ( argv[1][1] ) {
                 /*
                  * -h,? list help
                  */
           case 'h':
           case '?':
              usage ();
              break;
           default:
              (void) fprintf( stderr,"Unlisted option %s \n", argv[1]);
              usage();
       }  /* end switch */
                 /*
                  * move the argument list up one
                  * move the count down one
                  */
        argv++;
        argc--;
   }   /* end while */

                 /*
                  * All options processed.
                  */
   if (argc != 2) {
      (void) fprintf( stderr,"Need input filename \n");
      usage();     
      exit (1);
   } 

                 /*
                  *  The main section of the reordr program.
                  *   Read in line.
                  *   Break into sign - coefficient - monomial.
                  *   Add each to list.
                  *   Sort list alphabetically by monomial.
                  *   Print out the sorted list.
                  */   

                                  /* open the input file */
   in_file = fopen (InFileName, "r");   
   if (in_file == NULL) {
      printf ("Can not open %s\n", InFileName);
      exit (8);
   }
                                 /* read in file, line at a time */
   while (feof (in_file) == FALSE) {
      fgets (line, sizeof(line), in_file);
                                 /* skip comment lines */
      if (line[0] == '%')
        continue;

                                  /* verify the input */
      printf("\n\t\t\t  NEW POLYNOMIAL \n\n %s", line);
      trim_string(line);
#ifdef DEBUG
      printf(" %s", line);
#endif       
      pRemain = line;
      count = 0;
                                 /* process whole line to monimials */
   while (1) {     
                                 /* break for end of line */
      if (pRemain[0] == '\0' || pRemain[0] == '\n')  {
        break;                          
      }
       
       fNewMemory ((void**)&(pDbTemp_RDR), sizeof (DATA_RDR));

                                  // initialize db_temp_RDR
       pDbTemp_RDR->monmon [0]  = '\0';
       pDbTemp_RDR->sign [0]    = '\0';
       pDbTemp_RDR->sign [1]    = '\0';
       pDbTemp_RDR->coeff [0]   = '\0';

                                  // get sign
       if (pRemain [0] == '-')
          pDbTemp_RDR->sign [0] = '-';
       else
          pDbTemp_RDR->sign [0] = '+'; 
                                  // if string begins with + or -
                                  // move pointer past the sign.
       if (pRemain[0] == '-' || pRemain[0] == '+')
          pRemain++; 

                                  // break out coefficient
       nSpanLen = strcspn (pRemain, "abcd");
       strncpy (pDbTemp_RDR->coeff, pRemain, nSpanLen);
       pDbTemp_RDR->coeff[nSpanLen] = '\0';
       pRemain += nSpanLen;

                                  // break out monic monomial
       nSpanLen = strspn (pRemain, "abcd1234_*");
       strncpy (pDbTemp_RDR->monmon, pRemain, nSpanLen);
       pDbTemp_RDR->monmon[nSpanLen] = '\0';
#ifdef DEBUG
       printf("   ### sign: %s,   coeff: %-12s ",
         pDbTemp_RDR->sign, pDbTemp_RDR->coeff);
       printf("   monic monomial: %s \n",
         pDbTemp_RDR->monmon);
#endif   /* DEBUG */
       pRemain += nSpanLen;

       apTermList [count] = pDbTemp_RDR;
       count++;
    }     // end while, break all terms in a line

                                  /* output section */
       printf ("  Number of terms: %d \n", count);
#ifdef DEBUG
       printf ("  As entered: \n");
       print_array_of_terms (apTermList, count);
#endif    /* DEBUG */
       sort_list_by_monmon (apTermList, count);
       printf ("\n  Sorted: \n");
       print_array_of_terms (apTermList, count);

                                  /* clean up allocations */
      for (j = 0; j < count; j++) 
         FreeMemory (apTermList[j]);
       
    }     /* end while, loop over all lines in file */

                                  /* clean up */
       fflush (in_file);     
       fclose (in_file);
       return (count);

   return (0);
}  /* end main */




/*************************************************
 * usage -- tell the user how to use this program and
 *          exit
 **************************************************/
void usage(void)
{
     (void) fprintf(stderr, "Usage is %s [options] file_in \n",
                    program_name);
     (void) fprintf(stderr, "Options\n");
     (void) fprintf(stderr, "  -h             help\n");
     (void) fprintf(stderr, "  -?             help\n");
     exit (8);
}



/***********************************************
 * trim_string -- clean blanks off from front 
 *         and end.
 **********************************************/
BOOL trim_string (char *string)
{
   int  j, len;
   BOOL reverse_string_in_place (char *string);
   

   len = strlen (string);
   j = len - 1;
                                  // clean spaces off the back
   for (j = len - 1; j >=0 && string [j] == ' '; --j) 
      string [j] = '\0';
      
   reverse_string_in_place (string);

   len = strlen (string);
   j = len - 1;
                                  // clean spaces off the back (now front)
   for (j = len - 1; j >=0 && string [j] == ' '; --j) 
      string [j] = '\0';
   
                                  // return to original order
   reverse_string_in_place (string);
   return TRUE;
}


/***********************************************
 * reverse_string_in_place -- reverse a string
 **********************************************/
BOOL reverse_string_in_place (char *string)
{
   char   *pBegin, *pEnd;
   char   temp;
   int    len;
   
   len = strlen (string);
   
   pBegin = string;
   pEnd = string + len - 1; 
   for (; pBegin < pEnd; pBegin++, pEnd--) {
      temp  =  *pBegin;
      *pBegin = *pEnd;
      *pEnd = temp;
   }   
   
   return (TRUE);
}            



/******************************************************
 * sort_list_by_monmon -- cheap and quick bubble sort
 *****************************************************/
BOOL   sort_list_by_monmon (PDATA_RDR apList[], int nNum)
{
   int         i, j;
   PDATA_RDR   pDbTemp_RDR;
   
   for (i = 0; i < nNum - 1; ++i)
      for (j = nNum - 1; j > i; --j) 
         if (strcmp((apList [j-1])->monmon, (apList [j])->monmon) > 0) {
            pDbTemp_RDR      = apList [j];
            apList [j]       = apList [j-1];
            apList [j-1]     = pDbTemp_RDR;
         }   

   return TRUE;
}


/***********************************************
 * print_array_of_terms -- what it says
 **********************************************/
BOOL   print_array_of_terms (DATA_RDR **ppA, int count)
{
   int     j;

   for (j = 0; j < count; ++j) {
      printf ("%s %s %s ", 
              (*ppA [j]).sign, 
              (*ppA [j]).coeff,
              (*ppA [j]).monmon);
   }
   printf ("\n");

   return (TRUE);
}




