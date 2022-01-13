/*****************************************************************
 * mgir03.h -- header for functions from Chapter 3 of Maguire.
 *
 * $Id:$
 *****************************************************************/


        /***** make #including the file idempotent *****/
#ifndef      _MGIR03_H
#define      _MGIR03_H

      /**** Functions from Maguire ****/
extern flag fNewMemory (void **ppv, size_t size);
extern void FreeMemory (void *pv);
extern flag fResizeMemory (void **ppv, size_t sizeNew);

#endif
