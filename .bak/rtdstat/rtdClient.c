#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Tcl/tcl.h>
#include <Obm.h>
#include <ObmW/Gterm.h>


#include "rtdstat.h"


/*
 * RTDCLIENT.C -- The RTD/Status "client" object.  This code implements an OBM
 * client and responds to messages sent to the client object by the GUI code
 * executing under the object manager.
 *
 *	     rtd_clientOpen (rtd)
 *	    rtd_clientClose (rtd)
 *	  rtd_clientExecute (rtd, objname, key, cmd)
 *
 * The clientExecute callback is called by the GUI code in the object manager
 * to execute client commands.
 *
 * Client commands:
 *
 *		  InitCache 
 *		     Update
 *
 *		  setOption  option value [args]
 *
 *		      Reset
 *		       Quit
 */


/* Client callback struct. */
typedef struct {
        rsDataPtr rtd;
        Tcl_Interp *tcl;
        Tcl_Interp *server;
} rtdClient, *rtdClientPtr;


static 	int InitCache(), Update(), setOption();
static 	int Reset(), Quit();
static 	char *upFmtStr(), *upFmtInt(), *upFmtReal(), *upFmtHex();
static  void rtdUpdateSMC(), rtdUpdateRTD();

char 	strbuf[SZ_LINE];


extern double atof();


/* rtd_clientOpen -- Initialize the client code.
 */
void
rtd_clientOpen (rtd)
rsDataPtr rtd;
{
    char config[SZ_LINE];
    register rtdClientPtr xc;
    register Tcl_Interp *tcl;

    xc = (rtdClientPtr) XtCalloc (1, sizeof(rtdClient));
    rtd->clientPrivate = (int *)xc;

    xc->rtd = rtd;
    xc->tcl = tcl = Tcl_CreateInterp();
    ObmAddCallback (rtd->obm, OBMCB_clientOutput|OBMCB_preserve,
        rtd_clientExecute, (XtPointer)xc);

    Tcl_CreateCommand (tcl,
        "Quit", Quit, (ClientData)xc, NULL);

    Tcl_CreateCommand (tcl,
        "InitCache", InitCache, (ClientData)xc, NULL);
    Tcl_CreateCommand (tcl,
        "Update", Update, (ClientData)xc, NULL);

    Tcl_CreateCommand (tcl,
        "setOption", setOption, (ClientData)xc, NULL);



    /*  Open the SMC interface.  Note we currently only allow the cache
     *  file root name as an option but in later versions we can add other
     *  options to be used in the config string.
     */
    rtd_status ("Opening cache....");
    sprintf (config, "cache_file=%s", rtd->cache_file);

    if ((rtd->smc = smcOpen (config)) == (smCache_t *)NULL) {
        rtd_error ("Error opening cache, invalid file?");
        fprintf (stderr, "Error opening cache, invalid file?\n");
    }

    /*  Open the CDL interface.
     */
    if (!(rtd->cdl = cdl_open ((char *)getenv("IMTDEV"))) ) {
        rtd_error ("ERROR: cannot connect to display server!");
        fprintf (stderr, "ERROR: cannot open CDL\n");
    }

    /*  Update the GUI with the default state.
     */
    rtd_message (rtd, "disp_enable", (rtd->disp_enable ? "True" : "False"));
    rtd_message (rtd, "stat_enable", (rtd->stat_enable ? "True" : "False"));
    rtd_message (rtd, "hdrs_enable", (rtd->hdrs_enable ? "True" : "False"));

    rtd_msgi (rtd, "frame",          rtd->disp_frame);
    rtd_msgi (rtd, "stdimage",       rtd->stdimage);
    rtd_msgi (rtd, "smc_interval",   rtd->smc_interval);
    rtd_msgi (rtd, "disp_interval",  rtd->disp_interval);
}


/* rtd_clientClose -- Shutdown the client code.
 */
void
rtd_clientClose (rtd)
rsDataPtr rtd;
{
	register rtdClientPtr xc = (rtdClientPtr) rtd->clientPrivate;
	Tcl_DeleteInterp (xc->tcl);
}


/* rtd_clientExecute -- Called by the GUI code to send a message to the
 * "client", which from the object manager's point of view is RTDSTAT itself.
 */
rtd_clientExecute (xc, tcl, objname, key, command)
register rtdClientPtr xc;
Tcl_Interp *tcl;		/* caller's Tcl */
char *objname;			/* object name */
int key;			/* notused */
char *command;
{
	register rsDataPtr rtd = xc->rtd;

	xc->server = tcl;
	if (strcmp (objname, "client") == 0)
            Tcl_Eval (xc->tcl, command);

	return (0);
}




/*
 * RTDSTAT CLIENT commands.
 * ----------------------------
 */

/* Quit -- Exit rtdstat.
 *
 * Usage:	Quit
 */
static int 
Quit (xc, tcl, argc, argv)
register rtdClientPtr xc;
Tcl_Interp *tcl;
int argc;
char **argv;
{
	register rsDataPtr rtd = xc->rtd;
	rtd_shutdown (rtd);
}


/* InitCache -- Initialize the SMC Cache
 *
 * Usage:	InitCache
 *
 * Reset does a full power-on reset of SMC.
 */
static int 
InitCache (xc, tcl, argc, argv)
register rtdClientPtr xc;
Tcl_Interp *tcl;
int argc;
char **argv;
{
	register rsDataPtr rtd = xc->rtd;
}



/* Update -- Update the SMC Cache status display.
 *
 * Usage:	Update	smc|rtd
 *
 * Update the RTD or Shared Memory Cache Display
 */
static int 
Update (xc, tcl, argc, argv)
register rtdClientPtr xc;
Tcl_Interp *tcl;
int argc;
char **argv;
{
    char     *option;

    if (argc < 2)
	return (TCL_ERROR);

    option = argv[1];
    if (option[0] == 'r' || option[0] == 'R')
	rtdUpdateRTD (xc->rtd);

    else if (option[0] == 's' || option[0] == 'S')
	rtdUpdateSMC (xc->rtd);

    else {
	/* Need some kind of Tcl error message ..... */
	return (TCL_ERROR);
    }
}


/* setOption -- Set an rtdstat client option.
 *
 * Usage:       setOption option value [args]
 *
 * Options:
 *      disp_enable     true|false
 *      stat_enable     true|false
 *      hdrs_enable     true|false

 *      frame       	<frameno>
 *      stdimg       	<fbnum>
 */

static int 
setOption (xc, tcl, argc, argv)
register rtdClientPtr xc;
Tcl_Interp *tcl;
int argc;
char **argv;
{
        register rsDataPtr rtd = xc->rtd;
        char 	*option, *strval, **items;
        char 	buf[SZ_LINE];
        int 	ch, value, nx, ny, nitems, i, frame_list=0;


        if (argc < 3) {
            return (TCL_ERROR);

        } else {
            option = argv[1];
            strval = argv[2];

            ch = strval[0];
            if (isdigit (ch))
                value = atoi (strval);
            else if (ch == 'T' || ch == 't')
                value = 1;
            else if (ch == 'F' || ch == 'f')
                value = 0;
        }

	if (DEBUG)
	    printf ("setOption:  %s = %s\n", option, strval);


        if (strcmp (option, "disp_enable") == 0) {
            if (rtd->disp_enable != value) {
                rtd->disp_enable = value;
                sprintf (buf, "%s", value ? "True" : "False");
                rtd_message (rtd, "disp_enable", buf);
            }
	} else if (strcmp (option, "stat_enable") == 0) {
            if (rtd->stat_enable != value) {
                rtd->stat_enable = value;
                sprintf (buf, "%s", value ? "True" : "False");
                rtd_message (rtd, "stat_enable", buf);
            }

	} else if (strcmp (option, "hdrs_enable") == 0) {
            if (rtd->hdrs_enable != value) {
                rtd->hdrs_enable = value;
                sprintf (buf, "%s", value ? "True" : "False");
                rtd_message (rtd, "hdrs_enable", buf);
            }

	} else if (strcmp (option, "frame") == 0) {
            if (rtd->disp_frame != value) {
                rtd->disp_frame = value;
                rtd_msgi (rtd, "frame", value);
            }

	} else if (strcmp (option, "stdimg") == 0) {
            if (rtd->stdimage != value) {
                rtd->stdimage = value;
                rtd_msgi (rtd, "stdimage", value);
            }
	}
}



/*****************************************************************************
*
*  Private Utility Procedures.
*
*****************************************************************************/


/*  rtdUpdateSMC -- Update the Shared Memory Cache status display.
 */

static void
rtdUpdateSMC (rtd) 
rsDataPtr  rtd;
{
    smCache_t *smc = rtd->smc;
    double   size = 0.0;
    char     *fmt;

    extern char *smUtilTimeStr();


    /* Update the SMC status displays.
     */
    rtd_message (rtd, "smcVal", 
        upFmtStr("ctime", NULL, smUtilTimeStr(smc->ctime)) );
    rtd_message (rtd, "smcVal", 
        upFmtStr("utime", NULL, smUtilTimeStr(time(0))) );

    rtd_message (rtd, "smcVal", upFmtHex("memKey", NULL, smc->memKey) );
    rtd_message (rtd, "smcVal", upFmtInt("shmid", NULL, smc->shmid) );
    rtd_message (rtd, "smcVal", 
        upFmtInt("size", "%d bytes", smc->cache_size));

    size = smc->mem_allocated / (1024.0*1024.0);
    if (size < 1.0) {
        size *= 1024., fmt = "%d Kb";
    } else
        fmt = "%d Mb";
    rtd_message (rtd, "smcVal", upFmtInt("memUsed", fmt, (int)size));

    size = smc->mem_avail / 4096.0 ;
    fmt = "%d pages";
    rtd_message (rtd, "smcVal", upFmtInt("memAvail", fmt, (int) size));

    rtd_message (rtd, "smcVal", 
	upFmtInt("numProcs", NULL, smc->nattached));
    rtd_message (rtd, "smcVal", 
	upFmtInt("nsegs", "%d of 2048", smc->npages));


    /* Now update the segment listings. */




}


/*  rtdUpdateRTD -- Update the Real-Time Display.
 */

static void
rtdUpdateRTD (rtd) 
rsDataPtr  rtd;
{
    smCache_t *smc = rtd->smc;
    smcPage_t  *page = (smcPage_t *) NULL;
    smcPage_t  *last_page = (smcPage_t *) NULL;


    /*  Find the most recent page in the cache.
     */
    while (page = smcNextPage (smc, NULL)) {
	if (page->type == TY_DATA)
	    last_page = page;
        page = (smcPage_t *) NULL;
    }

    /*  No page found, just leave.
     */
    if (page == (smcPage_t *)NULL && last_page == (smcPage_t *)NULL)
	return;

    else if (rtd->disp_enable || rtd->stat_enable)
        rtd_message (rtd, "rtdVal", upFmtInt("seqno", NULL, 1));


    /*  Finally, update the GUI and display the image.
     */
    smcAttach (smc, last_page);

    if (rtd->stat_enable)
	rtdPixelStats (rtd, last_page);
    if (rtd->disp_enable)
	rtdDisplayPixels (rtd, last_page);

    smcDetach (smc, last_page, FALSE);

}


/*  rtdDisplayPixels -- Display the pixels.
 */

rtdDisplayPixels (rtd, page)
rsDataPtr  rtd;
smcPage_t *page;
{
    CDLPtr  cdl;
    int     nx=0, ny=0, bitpix=32;
    int     fb_w, fb_h, fbconfig, nf;
    int     *pix, *dpix;
    fpConfig_t *fp;


    if (!rtd->disp_enable)
	return;

    /* Open the package and a connection to the server. */
    if (!(cdl = cdl_open ((char *)getenv("IMTDEV"))) ) {
        fprintf (stderr, "ERROR: cannot connect to display server!\n");
        return;
    }

    fp = smcGetFPConfig (page);
    nx = fp->xSize;
    ny = fp->ySize;

    /* Get the pixel pointer and raster dimensions.
     */
    pix = (int *) smcGetPageData (page);
    dpix = (int *) calloc ((nx*ny), sizeof (int));
    bcopy (pix, dpix, (nx*ny*(bitpix/8)));

    /* Update the display.
     */
    rtd_message (rtd, "rtdVal", upFmtReal("expid", NULL, smcGetExpID(page)));
    rtd_message (rtd, "rtdVal", upFmtStr("obsid", NULL, smcGetObsetID(page)));


    /* Select a frame buffer large enough for the image.  
     *
    cdl_selectFB (cdl, nx, ny, &fbconfig, &fb_w, &fb_h, &nf, 1);
     */

    cdl_displayPix (cdl, dpix, nx, ny, bitpix, rtd->disp_frame, 
	rtd->stdimage, TRUE);

    cdl_close (cdl);
    free (dpix);
}


/*  rtdPixelStats -- Compute the pixel raster statistics.
 */
rtdPixelStats (rtd, page)
rsDataPtr  rtd;
smcPage_t *page;
{
    int   i, npix, data_size, *pix;
    smcSegment_t *seg = (smcSegment_t *) NULL;
    fpConfig_t *fp = (fpConfig_t *) NULL;
    double sum=0.0, sum2=0.0, mean=0.0, sigma=0.0, min=0.0, max=0.0;


    if (!rtd->stat_enable)
	return;

    seg = SEG_ADDR(page);
    data_size = seg->dsize;
    pix = (int *) smcGetPageData (page);

    fp = smcGetFPConfig (page);
    npix = (fp->xSize * fp->ySize);


    for (i=0; i < npix; i++) {
        sum += pix[i];
        min = (pix[i] < min ? pix[i] : min);
        max = (pix[i] > max ? pix[i] : max);
    }
    mean = (sum / (double)npix);

    for (i=0; i < npix; i++)
        sum2 += ( (pix[i] - mean) * (pix[i] - mean) );
    sigma = sqrt ( (sum2 / (double)npix) );


    /*  Send the pixel stat messages for display.
     */
    rtd_message (rtd, "rtdVal", upFmtReal("mean",  "%.2f", (float) mean));
    rtd_message (rtd, "rtdVal", upFmtReal("sigma", "%.3f", (float) sigma));
    rtd_message (rtd, "rtdVal", upFmtReal("min",   "%.1f", (float) min));
    rtd_message (rtd, "rtdVal", upFmtReal("max",   "%.1f", (float) max));
    rtd_message (rtd, "rtdVal", upFmtReal("expid", NULL, smcGetExpID(page)));
    rtd_message (rtd, "rtdVal", upFmtStr("obsid", NULL, smcGetObsetID(page)));
}


/*  Message Formatting Utility procedures.
 */

static char *
upFmtStr (char *key, char *fmt, char *val)
{
    char ch, lfmt[SZ_LINE];
    int i;

    for (i=0; val[i]; i++)
	if (val[i] == '\n') val[i] = '\0';

    sprintf (lfmt, "{ %s {%s} }", key, (fmt ? fmt : "%s") );
    sprintf (strbuf, lfmt, val);
    return (strbuf);
}

static char *
upFmtInt (char *key, char *fmt, int val)
{
    char lfmt[SZ_LINE];
    sprintf (lfmt, "{ %s {%s} }", key, (fmt ? fmt : "%d") );
    sprintf (strbuf, lfmt, val);
    return (strbuf);
}

static char *
upFmtReal (char *key, char *fmt, double val)
{
    char lfmt[SZ_LINE];
    sprintf (lfmt, "{ %s {%s} }", key, (fmt ? fmt : "%f") );
    sprintf (strbuf, lfmt, val);
    return (strbuf);
}

static char *
upFmtHex (char *key, char *fmt, long val)
{
    char lfmt[SZ_LINE];
    sprintf (lfmt, "{ %s {%s} }", key, (fmt ? fmt : "0x%x") );
    sprintf (strbuf, lfmt, val);
    return (strbuf);
}

