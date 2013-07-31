#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

#include "fitsio.h"

#include "pxf.h"


void pxfSendMeta(double *expID, char *obsetID, fitsfile * fitsID);
void pxfFopen(int *seq, fitsfile ** fd);

main(int argc, char **argv)
{
    char c, fname[64], root[64], buf[64], savex[32], val[12], config[128];
    char resp[80];
    long istat;
    double expID;
    int seq, page;
    fitsfile *fd;


    strcpy(pxfFILENAME, "fserTest");
    strcpy(pxfDIR, "/tmp");


    printf("DIR: %s\n", pxfDIR);
    printf("FILENAME: %s\n", pxfFILENAME);

    seq = 0;
    pxfFopen(&seq, &fd);
    processMetaData(&page, fd);

}

processMetaData(int *p, fitsfile * fd)
{
    int i, data_size;
    char *cdata, *ip, *edata;
    double expID;
    char obsetID[80], resp[80];
    long istat;

    pxfSendMeta(&expID, obsetID, fd);

    printf("%s\n", resp);
}



/*******************************************************************************
 * pxfFileOpen ( ... )
 *******************************************************************************/
void pxfFopen(int *seq, fitsfile ** fd)
{
    /* declare some variable and initialize them */
    int fitsStatus = 0;
    char *fitsDir = (char *) NULL;
    char fitsFile[80] = { '\0' };
    char tFName[80] = { '\0' };
    int stat, seqnum;


    (void) sprintf(tFName, "%s/%s", pxfDIR, pxfFILENAME);

    sprintf(fitsFile, "%s.fits", tFName);

    stat = access(fitsFile, F_OK);
    if (stat != 0) {
	printf("File does not exists: (%s), create it\n", fitsFile);
	if (fits_create_file(fd, fitsFile, &fitsStatus)) {
	    printf("pxfFileOpen: fits file create failed, Status=%d.",
		   fitsStatus);
	    return;
	}
    } else {
	/* the file exists from a previous exposure find a new name 
	   and create or open it.
	 */
	printf("File exists: (%s)\n", fitsFile);
	seqnum = *seq;
	while (access(fitsFile, F_OK) == 0) {
	    sprintf(fitsFile, "%s%d.fits", tFName, seqnum++);
	}
	*seq = seqnum;
	printf("Create (%s) \n", fitsFile);
	if (fits_create_file(fd, fitsFile, &fitsStatus)) {
	    printf("pxfFileOpen: fits file create failed, Status=%d.",
		   fitsStatus);
	    return;
	}
	return;
    }

    return;
}

void pxfSendMeta(double *expID, char *obsetID, fitsfile * fitsID)
{
    /* declare some variable and initialize them */
    int nr = 0;
    int fc = 0;
    int cpos = 0;
    int fitsStatus = 0;

    (void) ffpkyj(fitsID, "EXTTYPE", 123, "meta data type", &fitsStatus);

    /* flush and close */
    (void) ffflus(fitsID, &fitsStatus);
    if (fits_close_file(fitsID, &fitsStatus)) {
	printf("pxfSendMetaData:  fits close failed. Status = %d\n",
	       fitsStatus);
	return;
    }

    return;
}
