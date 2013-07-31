/**
 *  DETECTOR.H -- Detector definitions for Mosaic 1.1
 */

/*****************************************************************************/
/**  Detector Layout                                                        **/
/*****************************************************************************/


/**
 *  DO NOT DELETE		
 */
typedef struct {
    char  *colID;			/* collector ID			*/
    int    pageNum;			/* shared memory page number	*/
    int    destNum;			/* image destination extn 	*/
} pixArray, *pixArrayP;

/*****************************************************************************/


#define CCD_W		  2148		/**  width		       **/
#define CCD_H		  4096		/**  height		       **/
#define CCD_BPP		  4		/**  bytes per pixel	       **/
#define CCD_ROWS	  32		/**  No rows per chunk         **/
#define NCCDS		  1		/**  No of CCDs		       **/
#define NAMPS		  2		/**  No of amplifiers	       **/

#define BIAS_WIDTH        50
#define REFERENCE_WIDTH   BIAS_WIDTH
#define BITPIX            32


#ifndef _RASINFO_DEFINED

static pixArray rasInfo[] = {
    { "PanA",	1,	1},
    { "PanA",	2,	2},
};

#define _RASINFO_DEFINED
#endif
