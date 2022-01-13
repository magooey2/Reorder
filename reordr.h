/*****************************************************************
 * reordr.h -- header file for reorder.c
 *
 * Copyright 2018 Jesse I. Deutsch
 *
 * $Id: reordr.h,v 1.2 2018/03/20 08:36:11 jdeutsch Exp $
 *****************************************************************/


        /***** make #including the file idempotent *****/
#ifndef      _REORDR_H
#define      _REORDR_H


     /****** generally useful defines and typedefs ******/
#ifndef  MAX
#define  MAX(a, b)         (((a) < (b)) ? (b) : (a))
#define  MIN(a, b)         (((a) < (b)) ? (a) : (b))
#define  MAX4(a, b, c, d)  MAX(MAX(a,b), MAX(c,d))
#define  MIN4(a, b, c, d)  MIN(MIN(a,b), MIN(c,d))
#endif


       /*************** database structures **************/
struct tagDBENTRY_RDR {
   char        monmon [12];          /* monic monomial           */
   char        sign [2];
   char        coeff [12];           /* abs value of coefficient */
};

typedef  struct tagDBENTRY_RDR    DATA_RDR;
typedef  DATA_RDR                *PDATA_RDR;    /* a pointer to DATA */


enum     tagFIELD_RDR           {monmon, sign, coeff};
typedef  enum tagFIELD_RDR       FIELD_RDR;               


#endif    /* boundary for _REORDR_H */
