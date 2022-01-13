#ifdef  DEBUG

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "assert2.h"

#define   FALSE      0
#define   TRUE       1
#define   bGarbage   0xCC 


/*------------------------------------------------------------
 * The functions in this file must compare arbitrary pointers,
 * an operation that the ANSI standard does not guarantee to
 * be portable
 *
 * The macros below isolate the pointer comparisons needed in
 * this file.  The implementations assume "flat" pointers, for
 * which straightforward comparisions will always work.  The
 * definitions below will *not* work for some of the common
 * 80x86 memory models.
 */

/* 
#define	   fPtrLess (pLeft, pRight)      ((pLeft) <  (pRight))
#define    fPtrGrtr (pLeft, pRight)      ((pLeft) >  (pRight))
#define    fPtrEqual (pLeft, pRight)     ((pLeft) == (pRight))
#define    fPtrLessEq (pLeft, pRight)    ((pLeft) <= (pRight))
#define    fPtrGrtrEq (pLeft, pRight)    ((pLeft) >= (pRight))
*/

flag fPtrGrtrEq (byte *pLeft, byte *pRight)
{
   if (pLeft >= pRight)
      return TRUE;
   else
      return FALSE;
}         

flag fPtrLessEq (byte *pLeft, byte *pRight)
{
   if (pLeft <= pRight)
      return TRUE;
   else
      return FALSE;
}         

flag fPtrLess (byte *pLeft, byte *pRight)
{
   if (pLeft < pRight)
      return TRUE;
   else
      return FALSE;
}         

flag fPtrGrtr (byte *pLeft, byte *pRight)
{
   if (pLeft > pRight)
      return TRUE;
   else
      return FALSE;
}         

flag fPtrEqual (byte *pLeft, byte *pRight)
{
   if (pLeft == pRight)
      return TRUE;
   else
      return FALSE;
}         




/*-------------------------------------------------------*/
/*      * * * * * Private data/functions * * * * *		 */
/*-------------------------------------------------------*/


/*-------------------------------------------------------
 * pbiHead points to a singly linked list if
 * debugging information for the memory manager.
 */
 
static  blockinfo *pbiHead = NULL;


/*---------------------------------------------------------
 * pbiGetBlockInfo (pb)
 *
 * pbiGetBlockInfo searches the memory log to find the
 * block that pb points into and returns a point to the
 * corresponding blockinfo structure of the memory log.
 * Note: pb *must* point into an allocated block or you
 * will get an assertion failure: the funtion either
 * asserts or succeeds -- it never returns an error.
 *
 *   blockinfo *pbi;
 *	 ...
 *   pbi = pbiGetBlockInfo (pb);
 *   // pbi -> pb points to the start of pb's block
 *   // pbi -> size is the size of the block that pb points into
 */
 
static blockinfo *pbiGetBlockInfo (byte *pb)
{
   blockinfo   *pbi;
   
   for (pbi = pbiHead; pbi != NULL; pbi = pbi -> pbiNext)
   {
      byte *pbStart = pbi -> pb;        /* for readability */
      byte *pbEnd   = pbi -> pb  +  pbi -> size  -  1;
      
	  if (fPtrGrtrEq (pb, pbStart) && fPtrLessEq (pb, pbEnd))
	     break;
   }
   
   /* Couldn't find pointer?  Is it (a) garbage? (b) pointing
    * to a block that was freed? or (c) pointed to a block
    * that was moved when it was resized by fResizeMemory?
    */
   ASSERT (pbi != NULL);
   
   return (pbi);
}
          


/*-------------------------------------------------------*/
/*      * * * * * Public functions * * * * *		 */
/*-------------------------------------------------------*/


/*------------------------------------------------------*
 * fCreateBlockInfo (pbNew, sizeNew)
 *
 * This function creates a log entry for the memory block
 * defined by pbNew:sizeNew.  The function returns TRUE if it
 * successfully creates the log information; FALSE otherwise.
 *
 *   if (fCreateBlockInfo (pbNew, sizeNew))
 *      // success -- the memory log has an entry.
 *   else
 *      // failure -- no entry, so release pbNew
 */
 
flag fCreateBlockInfo (byte *pbNew, size_t sizeNew)
{
   blockinfo  *pbi;
   
   ASSERT (pbNew != NULL && sizeNew != 0);
   
   pbi = (blockinfo *) malloc (sizeof (blockinfo));
   if (pbi != NULL)
   {
      pbi -> pb      = pbNew;
      pbi -> size    = sizeNew;
      pbi -> pbiNext = pbiHead;
      pbiHead        = pbi;
   }
      
   return (flag) (pbi != NULL);
}
   


/*----------------------------------------------------
 * FreeBlockInfo (pbToFree)
 *
 * This function destroys the log entry for the memory block
 * that pToFree points to.  pToFree *must* point to the
 * start of an allocated block; otherwise, you will get an
 * assertion failure.
 */
 
void FreeBlockInfo (byte *pbToFree)
{
   blockinfo  *pbi, *pbiPrev;
   
   pbiPrev = NULL;
   for (pbi = pbiHead; pbi != NULL; pbi = pbi -> pbiNext)
   {
      if (fPtrEqual (pbi -> pb, pbToFree))
      {
         if (pbiPrev == NULL)
            pbiHead = pbi -> pbiNext;
         else
            pbiPrev -> pbiNext = pbi -> pbiNext;
         break;
      }
   pbiPrev = pbi;
   }   
   
   /* If pbi is NULL, then pbToFree is invalid */
   ASSERT (pbi != NULL);

   /* Verify that nothing wrote off the end of the block. */
   ASSERT (*(pbi -> pb + pbi -> size) == bDebugByte);
   
   /* Destroy the contents of *pbi before freeing them. */
   memset (pbi, bGarbage, sizeof (blockinfo));
   
   free (pbi);
}

   

/*---------------------------------------------------------
 * UpdateBlockInfo (pbOld, pbNew, sizeNew)
 *
 * UpdateBlockInfo looks up the log information for the memory
 * block that pbOld points to.  The function then updates the               
 * log information to reflect the fact the the block now 
 * lives at pbNew and is "sizeNew bytes" long.  pbOld *must*
 * point to the start of the allocated block; otherwise,
 * you will get an assertion failure.
 */
 
void UpdateBlockInfo (byte *pbOld, byte *pbNew, size_t sizeNew)
{
   blockinfo  *pbi;
   
   ASSERT (pbNew != NULL && sizeNew != 0);
   
   pbi = pbiGetBlockInfo (pbOld);
   ASSERT (pbOld == pbi -> pb);
   
   pbi -> pb   = pbNew;
   pbi -> size = sizeNew;

}




/*---------------------------------------------------------
 * sizeofBlock (pb)
 *
 * sizeofBlock returns the size of the block that pb points to.
 * pb *must* point to the start of the allocated block;
 * otherwise, you will get an assertion failure.
 */
 
size_t sizeofBlock (byte *pb)
{ 
   blockinfo  *pbi;
   
   pbi = pbiGetBlockInfo (pb);
   ASSERT (pb == pbi -> pb);

   /* Verify that nothing wrote off the end of the block. */
   ASSERT (*(pbi -> pb + pbi -> size) == bDebugByte);
   
   return (pbi -> size);
}




/*-----------------------------------------------------------*/
/*     The following routines are used to find dangling 	 */
/*     pointers and lost memory blocks.  See Chapter 3		 */
/*     for a discussion of these routines.					 */
/*-----------------------------------------------------------*/


/*---------------------------------------------------------
 * ClearMemoryRefs (void)
 *
 * ClearMemoryRefs marks all blocks in the memory log as 
 * being unreferenced.
 */
 
void ClearMemoryRefs (void)
{
   blockinfo  *pbi;
   
   for (pbi = pbiHead; pbi != NULL; pbi = pbi -> pbiNext)
      pbi -> fReferenced = FALSE;
}


/*---------------------------------------------------------
 * NoteMemoryRef (pv)
 *
 * NoteMemoryRef marks the block that pv points into as being
 * referenced.  Note: pv does *not* have to point to the
 * start of the block; it may point anywhere within an
 * allocated block.
 */
 
void NoteMemoryRef (void *pv)
{
   blockinfo  *pbi;
   
   pbi = pbiGetBlockInfo ((byte *) pv);
   pbi -> fReferenced = TRUE;      
   
   /* Verify that nothing wrote off the end of the block. */
   ASSERT (*(pbi -> pb + pbi -> size) == bDebugByte);

}



/*---------------------------------------------------------   
 * CheckMemoryRefs (void)
 *
 * CheckMemoryRefs scans the memory log looking for blocks that
 * have not been marked with a call to NoteMemoryRef.  If this
 * function finds an unmarked block, it asserts.
 */
 
void CheckMemoryRefs (void)
{
   blockinfo  *pbi;
   
   for (pbi = pbiHead; pbi != NULL; pbi = pbi -> pbiNext)
   {
      /* A simple check for block integrity.  If this
       * assert fires, it means that something is wrong
       * with the debug code that manages blockinfo or,
       * possibly, that a wild memory store has trashed 
       * the data structure.  Either way, there's a bug.
       */
      ASSERT (pbi -> pb != NULL && pbi -> size != 0);
      
 	  /* A check for lost or leaky memory.  If this assert
 	   * fires, it means that the app has either lost track
 	   * of this block or that not all global pointers have
 	   * been accounted for with NoteMemoryRef.
 	   */
 	  ASSERT (pbi -> fReferenced);
 	  
      /* Verify that nothing wrote off the end of the block. */
      ASSERT (*(pbi -> pb + pbi -> size) == bDebugByte);

   }	   
}

/*---------------------------------------------------------
 * fValidPointer (pv, size)
 *
 * fValidPointer verifies that pv points into an allocated
 * memory block and that there are at least "size" allocated
 * bytes from pv to the end of the block.  If either condition
 * is not met, fValidPointer will assert; the function will
 * never return FALSE.
 *
 * The reason fValidPointer returns a flag at all (always TRUE)
 * is to allow you to call the function within an ASSERT macro.
 * While this isn't the most efficient method to use, using the
 * macro neatly handles the debug-vs-ship version control
 * issue without your having to resort to #ifdef DEBUG's or
 * to introducting other ASSERT-like macros.
 *
 *     ASSERT (fValidPointer (pb, size));
 */
 
flag fValidPointer (void *pv, size_t size)
{
   blockinfo  *pbi;
   byte       *pb = (byte *) pv;
   
							/* could have '\0' and zero length pointer */
   ASSERT (pv != NULL);
//   ASSERT (pv != NULL && size != 0);
   
   pbi = pbiGetBlockInfo (pb);           /* This validates pv */
   
   /* size isn't valid if pb + size overflows the block. */
   ASSERT (fPtrLessEq (pb + size, pbi -> pb  +  pbi -> size));

   /* Verify that nothing wrote off the end of the block. */
   ASSERT (*(pbi -> pb + pbi -> size) == bDebugByte);
    
   return (TRUE);
}

#endif

   
   
