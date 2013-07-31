/*******************************************************************************
 * include(s):
 *******************************************************************************/
#if !defined(_dhsUtil_H_)
#include "dhsUtil.h"
#endif
#if !defined(_FITSIO_H)
#include "fitsio.h"
#endif

#include <errno.h>

#include "pxf.h"
/*******************************************************************************
 * pxfSendPixelData ( ... )
 *******************************************************************************/
void pxfSendPixelData(long *istat,	/* inherited status               */
		      char *resp,	/* response message               */
		      void *pxlAddr,	/* address of data block          */
		      size_t blkSize,	/* size of data block             */
		      fpConfig_t * cfg,	/* configuration of pixel data    */
		      double *expID,	/* exposure identifier            */
		      char *obsetID,	/* observation set identifier     */
		      fitsfile * fitsID)
{
    /* declare some variable and initialize them */
    int fs = 0;
    int bitpix;
    long nelms;
    long naxis;
    long naxes[2];
    char *fitsDir = (char *) NULL;
    char fitsFile[DHS_MAXSTR] = { '\0' };
    struct stat sts;

    (void) memset((void *) &sts, 0, sizeof(struct stat));
    (void) memset((void *) &fitsFile[0], '\0', DHS_MAXSTR);

    /* check the inherited status */
    if (STATUS_BAD(*istat))
	return;
    MNSN_CLEAR_STR(resp, '\0');

    /* check input parameters */
    if (pxlAddr == (void *) NULL || expID == (double *) NULL) {
	*istat = DHS_ERROR;
	sprintf(resp, "dhsSendPixelData: Bad parameter. %s",
		((expID ==
		  (double *) NULL) ? "expID ptr==NULL." :
		 "FP config ptr == NULL."));
	return;
    }

    DPRINTF(10, procDebug, "dhsSendPixelData: fitsFile=>%s<\n", fitsFile);

    *istat = DHS_OK;
    /* set output size */
    naxes[0] = cfg->xSize;
    naxes[1] = cfg->ySize;
    nelms = naxes[0] * naxes[1];
    naxis = 2;
    bitpix = 32;

    /* make it a FITS mage file */
    if (fits_create_img(fitsID, bitpix, naxis, naxes, &fs)) {
	*istat = DHS_ERROR;
	sprintf(resp,
		"dhsSendPixelData:  fits image file create failed, fitsStatus = %d",
		fs);
	(void) fits_close_file(fitsID, &fs);
	return;
    }

    /* write data as long integers */
    if (fits_write_img(fitsID, TLONG, 1L, nelms, pxlAddr, &fs)) {
	*istat = DHS_ERROR;
	sprintf(resp,
		"dhsSendPixelData:  fits write image file failed, fitsStatus = %d",
		fs);
	(void) fits_close_file(fitsID, &fs);
	return;
    }

    fits_write_date(fitsID, &fs);
    fits_update_key(fitsID, TDOUBLE, "EXPID", expID, "dhs exposure ID",
		    &fs);
    fits_update_key(fitsID, TSTRING, "OBSETID", (char *) obsetID,
		    "Observation ID", &fs);

    /* flush and close */
    (void) ffflus(fitsID, &fs);
/*
    if ( fits_close_file(fitsID,&fs) ) 
    {
	*istat = DHS_ERROR;
	 sprintf(resp,"dhsSendPixelData: fits close failed, fitsStatus = %d", fs);
	return;
    }
*/
    *istat = DHS_OK;
    /* return */
    sprintf(resp, "dhsSendPixelData: (%s) Success.", fitsFile);
    return;
}
