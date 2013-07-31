/*
 *  RTDSTAT.H -- Global definitions for RTDSTAT.
 */


#define	DEBUG			1

#ifdef __DARWIN__
#define MACOSX
#endif


#include "cdl/cdl.h"            /* NOTE: should use std dirs!! */
#include "smCache.h"   		/* NOTE: should use std dirs!! */


/* Default values, size limiting values.
 */
#define	MAX_CLIENTS		8	/* max display server clients	*/
#define	DEF_INTERVAL		1.0	/* default polling time		*/
#define	DEF_FBCONFIG		42	/* stdimage = imt4400		*/
#define	DEF_FBWIDTH		4400
#define	DEF_FBHEIGHT		4400
#define	DEF_FRAME		1
#define	DEF_CACHE		"/tmp/.smc\0"

#define	SZ_NAME			80	/* object name buffer		*/
#define	SZ_IMTITLE		128	/* image title string		*/
#define	SZ_WCTEXT		80	/* WCS box text 		*/
#define	SZ_WCSBUF		1024	/* WCS text buffer size		*/
#define	SZ_MSGBUF		8192	/* message buffer size		*/

#ifndef	SZ_FNAME
#define	SZ_FNAME		256
#endif
#ifndef	SZ_LINE
#define	SZ_LINE			512
#endif
#ifndef	ERR
#define	ERR			(-1)
#endif
#ifndef	OK
#define	OK			0
#endif
#ifndef	EOS
#define	EOS			'\0'
#endif


/* Magic numbers. */
#define DEF_PORT	   5137			/* default tcp/ip socket      */
#define DEF_HOST	   "localhost"		/* default tcp/ip socket      */
#define	DEF_UNIXADDR	   "/tmp/.IMT%d"	/* default unix socket        */



/*
 * Application resources and runtime descriptor.
 * ----------------------------------------------
 */
typedef struct {
	/* Resources. */
	Boolean invert;			/* use inverted colormap 	*/
	int def_config;			/* default FB config 		*/
	int def_nframes;		/* default number of frames 	*/

	String gui;			/* GUI file name 		*/
	String host;			/* host for INET socket	 	*/
	int port;			/* port for INET socket	 	*/

	/* Internal state. */
	XtPointer obm;			/* object manager 		*/
	Widget toplevel;		/* dummy toplevel app shell 	*/

	CDLPtr	cdl;			/* CDL interface descriptor	*/
	smCache_t *smc;			/* SMC interface descriptor	*/

	int  disp_enable;		/* RTD enable flag	 	*/
	int  stat_enable;		/* pixel stats enable flag 	*/
	int  hdrs_enable;		/* hdr display enable flag 	*/
	float disp_interval;		/* display polling interval 	*/
	int  disp_frame;		/* current display frame 	*/
	int  stdimage;			/* display frame buffer 	*/
	int  width, height;		/* FB width, height 		*/

	char cache_file[SZ_FNAME];	/* SMC cache file		*/
	float smc_interval;		/* SMC stat polling interval 	*/

	int *clientPrivate;		/* used by rtd client code 	*/

} rsData, *rsDataPtr;



#ifdef RTDSTAT_MAIN
rsData rtdstat_data;

#define	XtNgui			"gui"
#define	XtCGui			"Gui"
#define	XtNport			"port"
#define	XtCPort			"Port"
#define	XtNhost			"host"
#define	XtCHost			"Host"

static XtResource resources[] = {
    {
	XtNgui,
	XtCGui,
	XtRString,
	sizeof(String),
	XtOffsetOf(rsData, gui),
	XtRImmediate,
	(caddr_t)"default"
    },
    {
	XtNport,
	XtCPort,
	XtRInt,
	sizeof(int),
	XtOffsetOf(rsData, port),
	XtRImmediate,
	(caddr_t)DEF_PORT
    },
    {
	XtNhost,
	XtCHost,
	XtRString,
	sizeof(String),
	XtOffsetOf(rsData, host),
	XtRImmediate,
	(caddr_t)DEF_HOST
    },
};



#else

extern XtResource resources[];
extern rsData rtdstat_data;

#endif

/* Functions.
 */
#ifndef abs
#define	abs(a)		(((a)<0)?(-(a)):(a))
#endif
#ifndef min
#define min(a,b)	((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b)	((a)<(b)?(b):(a))
#endif
#ifndef nint
#define nint(a)		((a)<(0)?((int)(a-0.5)):((int)(a+0.5)))
#endif

void rtd_initialize(), rtd_reset(), rtd_refresh();
void rtd_close(), rtd_alert();
void rtd_status(), rtd_error();
void rtd_message(), rtd_msgi();

/*
XtPointer xim_addInput();
void xim_removeInput();
*/

void rtd_clientOpen(), rtd_clientClose();
int rtd_clientExecute();


