#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "assert2.h"

#define  bGarbage   0xCC


flag fNewMemory (void **ppv, size_t size)
{
   byte  **ppb = (byte **) ppv;
   
   ASSERT (ppv != NULL && size != 0);
   
   *ppb = (byte *) malloc (size + sizeofDebugByte);
   
   #ifdef  DEBUG
   {
      if ( *ppb != NULL)
      {
         *(*ppb + size) = bDebugByte;
         
         memset (*ppb, bGarbage, size);
         
         /* If unable to create the block information,
          * fake a total memory failure.
          */
         if (!fCreateBlockInfo (*ppb, size))
         {
            free (*ppb);
            *ppb = NULL;
         }
      }       
   }
   #endif
   
   return (*ppb != NULL);
}
         


void FreeMemory (void *pv)
{
   ASSERT (pv != NULL);
   
   #ifdef  DEBUG
   {
      memset (pv, bGarbage, sizeofBlock (pv));
	  FreeBlockInfo (pv);
   }
   #endif
   
   free (pv);
}



flag fResizeMemory (void **ppv, size_t sizeNew)
{
   byte  **ppb = (byte **)ppv;
   byte  *pbNew;
   #ifdef DEBUG
      size_t sizeOld;
   #endif
   
   ASSERT (ppb != NULL && sizeNew != 0);
      
   #ifdef DEBUG
   {
      sizeOld = sizeofBlock (*ppb);
      
      /* If the block is shrinking, pre-fill the soon-to-be-
       * released memory.  If the block is expanding, force
       * it to move (instead of expanding in place) by faking
       * a realloc.  If the block is the same size, don't do
       * anything.
       */
       
      if (sizeNew < sizeOld)
         memset ((*ppb) + sizeNew, bGarbage, sizeOld - sizeNew);
      else if (sizeNew > sizeOld)
      {
         byte *pbForceNew;
            
         if (fNewMemory ((void **)&pbForceNew, sizeNew))
         {
            memcpy (pbForceNew, *ppb, sizeOld);
            FreeMemory (*ppb);
            *ppb = pbForceNew;
         }   
      }
   }
   #endif

   pbNew = (byte *) realloc (*ppb, sizeNew + sizeofDebugByte);
   if (pbNew != NULL)
   {
      #ifdef DEBUG
      {
         *(pbNew + sizeNew) = bDebugByte;
         UpdateBlockInfo (*ppb, pbNew, sizeNew);

         /* If expanding, initialize the new tail. */
         if (sizeNew > sizeOld)
            memset (pbNew + sizeOld, bGarbage, sizeNew - sizeOld);
      }
      #endif
            
      *ppb = pbNew;
   }
   return (pbNew != NULL);
}


     
