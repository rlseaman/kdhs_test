#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

/*
#if !defined(_dhsUtil_H_)
#include "dhsUtil.h"
#endif
*/

/* These PRINT definitions are by default at /MNSN/soft_dev/inc/debug.h */

#define DPRINT(Level, Arg, Msg) \
 if(Arg >= Level) {fprintf(stderr, "\t*DBG* %s", Msg); fflush(stderr);}

#define DPRINTF(Level, Arg, Msg, Val) if(Arg >= Level) \
{   char __dbgMsg[256],__dbgMsg2[256];\
    sprintf(__dbgMsg, "\t*DBG* %s", Msg);\
    sprintf(__dbgMsg2, __dbgMsg, Val);\
    fputs (__dbgMsg2, stderr); \
    fflush(stderr);\
}

#if !defined(_FITSIO_H)
#include "fitsio.h"
#endif

#include "pxf.h"
#include "smCache.h"


#define	DEF_FNAME	"fserTest"	/* default filename root	*/
#define DEF_DIR		"/tmp"		/* default directory		*/



smCache_t *smc    = (smCache_t *) NULL;
smcPage_t *page   = (smcPage_t *) NULL;
smcSegment_t *seg = (smcSegment_t *) NULL;


int console   = 0;                      /* display console messages?    */
int use_mbus  = 1;                      /* Use the msgbus for status?   */
int mb_tid    = 0;                      /* message bus tid              */


					/* Function prototypes		*/
extern smCache_t *smcOpen();
extern smcPage_t *smcGetPage();

					/* I/O Handlers			*/
int   mbusMsgHandler (int fd, void *client_data);
int   consMsgHandler (int fd, void *client_data);



int
main (int argc, char **argv)
{
    char   fname[64], savex[32], config[128], resp[80];
    int    i, fs = 0, debug = 0;
    int    index, len, dirflg, fiflg;
    long   istat, lexpID, saveExpID;
    double expID;

    fitsfile *fd = (fitsfile *) NULL;


    if (argc == 2)
	pxfUsage();


    strcpy (pxfFILENAME, DEF_FNAME);
    strcpy (pxfDIR, DEF_DIR);

    procDebug   = 0;			/* Debug flag for DPRINT macro */
    dirflg 	= 0;
    fiflg 	= 0;

    for (i = 1; i < argc; i++) {
	if (strncmp(argv[i], "-help", 2) == 0) {
	    pxfUsage();

	} else if (strncmp(argv[i], "-dir", 3) == 0) {
	    strcpy(pxfDIR, argv[++i]);
	    dirflg = 1;

	} else if (strncmp(argv[i], "-froot", 3) == 0) {
	    strcpy(pxfFILENAME, argv[++i]);
	    fiflg = 1;

	} else if (strncmp(argv[i], "-debug", 3) == 0) {
	    procDebug = atoi(argv[++i]);

        } else if (strncmp(argv[i], "-console", 3) == 0) {
            console++;
            procDebug += 10;

        } else if (strncmp(argv[i], "-mbus", 3) == 0) {
            use_mbus = 1;

        } else if (strncmp(argv[i], "-nombus", 5) == 0) {
            use_mbus = 0;

	} else {
	    pxfUsage ();
	}
    }


    /*  Open/Attach to the Shared Memory Cache.
     */
    sprintf (config, "debug=%d", debug);
    if ((smc = smcOpen (config)) == (smCache_t *) NULL)
	fprintf(stderr, "Error opening cache, invalid file?.\n");

    smcSetSeqNo (smc, 0);
    if (dirflg == 0) {		/* Get Dirname from shared memory */
	len = min(250, strlen((char *) smcGetDir(smc)));
	memmove (pxfDIR, (char *) smcGetDir(smc), len);
    }
    if (fiflg == 0) {		/* Get Filenam from shared memory */
	len = min (250, strlen((char *) smcGetFRoot(smc)));
	memmove (pxfFILENAME, (char *) smcGetFRoot(smc), len);
    }
    pxfFLAG = 3;	/* indicates that DIR and FILENAME are defined. */




    /*  Simulate the readout sequence.
     */
    index = 1;
    i = 0;
    savex[0] = ' ';
    saveExpID = 0;
    strcpy(fname, pxfFILENAME);	/* save root filename */
    while ((page = smcNextPage(smc, 1000))) {
	smcAttach(smc, page);	/*  Attach to the page.  */
	switch (page->type) {	/*  Process the data.  */
	case TY_VOID:
	    processData(page, fd);
	    break;
	case TY_DATA:
	    processData(page, fd);
	    break;
	case TY_META:
	    expID = smcGetExpID(page);
	    lexpID = expID;
	    printf("OPENING Fits file:: %d saveExpID: %d\n",
		   (lexpID - saveExpID) / 100000, saveExpID);
	    if (((lexpID - saveExpID) / 100000 >= 1) || saveExpID == 0) {
		if (fd != (fitsfile *) NULL) {
		    if (fits_close_file(fd, &fs)) {
			DPRINTF(10, procDebug,
				"pxf: fits close failed, fitsStatus = %d",
				fs);
			exit;
		    }
		}
		len = min(250, strlen((char *) smcGetPageDir(page)));
		if (dirflg == 0 && len > 0) {
		    memmove(pxfDIR, (char *) smcGetPageDir(page), len);
		}
		len = min(250, strlen((char *) smcGetPageFRoot(page)));
		if (fiflg == 0 && len > 0) {
		    memmove(pxfFILENAME, (char *) smcGetPageFRoot(page),
			    len);
		}

		printf("Calling pxfFileOpen\n");
		pxfFileOpen(&istat, resp, &expID, smc, &fd);
		DPRINTF(10, procDebug, "%s\n", resp);
		saveExpID = lexpID;
	    }

	    processMetaData(page, fd);
	    break;
	default:
	    break;
	}

	i++;
	smcDetach(smc, page, TRUE);	/*  Detach from the page.  */
    }


    if (fits_close_file(fd, &fs)) {
	DPRINTF(10, procDebug, "pxf: fits close failed, fitsStatus = %d", fs);
	exit;
    }


    /*
       if (smc && !interactive) 
       smcClose (smc, clear);
     */
}


pxfUsage ()
{
    printf ("Usage: ");
    printf ("pxf -dir <Output FITS dir> -froot <FITS root name>\n");
    printf("           -debug_level 'int >= 10'\n");
    exit (0);
}


/*  NOTE:  Assumes an integer data array.
 */
processData(smcPage_t * p, fitsfile * fd)
{
    int i, *idata;
    ulong data_size;
    long istat;
    double expID;
    smcSegment_t *seg = (smcSegment_t *) NULL;
    fpConfig_t *fpCfg = smcGetFPConfig(p);
    char obsetID[80], resp[80];


    /* stdHdrOutput (p);          print standard header */

    seg = SEG_ADDR(p);
    data_size = seg->dsize;
    istat = 0;
    strcpy(obsetID, smcGetObsetID(p));
    idata = (int *) smcGetPageData(p);

    expID = smcGetExpID(p);
    /*  strcpy(obsetID,smcGetObsetID(p)); */
    pxfSendPixelData(&istat, resp, (void *) idata, (size_t) data_size,
		     (fpConfig_t *) fpCfg, &expID, obsetID, fd);

    DPRINTF(30, procDebug, "%s\n", resp);
}


processMetaData(smcPage_t * p, fitsfile * fd)
{
    int i, data_size;
    char *cdata, *ip, *edata;
    smcSegment_t *seg = (smcSegment_t *) NULL;
    mdConfig_t *mdCfg = smcGetMDConfig(p);
    double expID;
    char obsetID[80], resp[80];
    long istat;


    /* stdHdrOutput (p);                    print standard header        */

    seg = SEG_ADDR(p);
    data_size = seg->dsize;
    cdata = ip = (char *) smcGetPageData(p);
    edata = cdata + data_size;

    expID = smcGetExpID(p);
    strcpy(obsetID, smcGetObsetID(p));

    pxfSendMetaData(&istat, resp, (char *) cdata, (size_t) data_size,
		    (mdConfig_t *) mdCfg, &expID, obsetID, fd);
    ip = cdata;			/* initialize                 */
    edata = (cdata + data_size);	/* find addr of end of data   */

    DPRINTF(30, procDebug, "%s\n", resp);
}


stdHdrOutput(smcPage_t * p)
{
    mdConfig_t *md = smcGetMDConfig(p);
    fpConfig_t *fp = smcGetFPConfig(p);
    smcSegment_t *seg = (smcSegment_t *) NULL;


    seg = SEG_ADDR(p);
    printf("  Lock: %s  size = %d   dsize = %d\n",
	   p->lockFile, p->size, seg->dsize);
    printf("  MD Config:  metaType = %d\tnumFields = %d:\n",
	   md->metaType, md->numFields);
    printf("  FP Config:  %d x %d  at (%d,%d) type = %d\n",
	   fp->xSize, fp->ySize, fp->xStart, fp->yStart, fp->dataType);
}
