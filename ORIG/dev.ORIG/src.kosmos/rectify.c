/**
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include "../../include/location.h"		/* REQUIRED	*/
#include "../../include/instrument.h"		/* REQUIRED	*/

#include "mbus.h"
#include "smCache.h"


#define	BITPIX			32
#define REFERENCE_WIDTH		32


extern  int trim_ref;                   /* trim reference pix flag      */
extern  int rotate_enable;
extern  int console, verbose, debug;

extern void rotate(void *iadr, XLONG **oadr, int type, int nx, int ny, int dir);



/*  smcRectifyPage -- Rectify the display pixels of an SMC page.
*/
void
smcRectifyPage (page)
smcPage_t *page;
{
    int    i, j, k, nx=0, ny=0, xs=0, ys=0;
    XLONG  *pix, *dpix, *lpix, *sp, *ep, npix, temp;
    static XLONG  sv_nx=0, sv_ny=0, first_time=1;
    static XLONG *dpix_buf=(XLONG *)NULL,
		 *lpix_buf=(XLONG *)NULL;

    fpConfig_t *fp;


    if (!rotate_enable)			/* sanity checks		*/
	return;

    fp = smcGetFPConfig (page);		/* get the focal plane config	*/
    nx = fp->xSize;
    ny = fp->ySize;
    xs = fp->xStart;
    ys = fp->yStart;
    npix = nx * ny;

    /* Get the pixel pointer and raster dimensions.  Allocate space that 
    ** includes the reference in case we use it later.
    */
    pix  = (XLONG *) smcGetPageData (page);

    if ((nx*ny) != (sv_nx*sv_ny)) {
        if (dpix_buf) free ((XLONG *)dpix_buf);
        if (lpix_buf) free ((XLONG *)lpix_buf);
        sv_nx = nx;
        sv_ny = ny;
        first_time = 1;
    }
    if (first_time) {
        dpix = dpix_buf = (XLONG *) calloc ((nx*ny), sizeof (XLONG));
        lpix = lpix_buf = (XLONG *) calloc ( nx    , sizeof (XLONG));
        first_time = 0;
    } else {
        dpix = (XLONG *) dpix_buf;
        lpix = (XLONG *) lpix_buf;
    }


    /* Get the rectification direction.  	[ DEVICE SPECIFIC ]
    */
    if (ys == 0)			/* Only top row gets rotated	*/
	return;			


    /* Transform the raster by flipping the raster in-place.
     */
    sp = pix;
    ep = pix + npix - 1;
    while (sp < ep) {
	temp = *sp;
	*sp  = *ep; 
	*ep  = temp;
	sp++;
	ep--;
    }

    /* Now move the bias regions to the end of the data.
     */
    sp = pix;
    for (k=0; k < ny; k++) {
	for (i=0, j=100; j < nx;  j++, i++)	/* get data pixels	*/
	    lpix[i] = sp[j];
	for (     j=0;   j < 100; j++, i++)	/* get bias regions	*/
	    lpix[i] = sp[j];

	for (i=0; i < nx; i++)			/* copy back the line	*/
	    *sp++ = lpix[i];
    }

    /*free ((XLONG *)dpix);*/ 				/* clean up.	*/
}
