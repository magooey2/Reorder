/*****************************************************************
 * block.h -- header file for block functions from Maguire.
 *
 * $Id:$
 *****************************************************************/


        /***** make #including the file idempotent *****/
#ifndef      _BLOCK_H
#define      _BLOCK_H

#include <malloc.h>

typedef  unsigned char    byte;
typedef  int              flag;


#ifdef  DEBUG

/*--------------------------------------------------------
 * blockinfo is the structure that contains the memory log
 * information for one allocated memory block.  Every
 * allocated memory block has a corresponding blockinfo
 * structure in the memory log.
 */
 
typedef struct BLOCKINFO
{
   struct   BLOCKINFO *pbiNext;
   byte     *pb;                       /* Start of block */
   size_t   size;					   /* Length of block */
   flag     fReferenced;               /* Ever referenced? */
} blockinfo;

flag    fCreateBlockInfo (byte *pbNew, size_t sizeNew);
void    FreeBlockInfo (byte *pbToFree);
void    UpdateBlockInfo (byte *pbOld, byte *pbNew, size_t sizeNew);
size_t  sizeofBlock (byte *pb);
   

void  ClearMemoryRefs (void);
void  NoteMemoryRef (void *pv);
void  CheckMemoryRefs (void);
flag  fValidPointer (void *pv, size_t size);

#endif



/* bDebugByte is a magic value that is stored at the
 * tail of every allocated memory block in DEBUG
 * versions of the program.  sizeofDebugByte is added
 * to the sizes passed to malloc and realloc so that
 * the correct amount of space is allocated.
 */
 
#define bDebugByte  0xE1

#ifdef DEBUG
   #define sizeofDebugByte  1
#else
   #define sizeofDebugByte  0
#endif

 
#endif    /* of _BLOCK_H */

