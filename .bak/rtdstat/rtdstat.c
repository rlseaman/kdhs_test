#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Obm.h>


#define  RTDSTAT_MAIN
#include "rtdstat.h"



/*  RTDSTAT -- A Real-Time Display and SMC Status Monitor for the 
 *  NEWFIRM/Monsoon DHS.
 */


/* The builtin default GUI. */
char *defgui_text[] = {
#   include "rtdstat.gui.h"
    NULL
};


/* Data. */
XtAppContext app_context;
static char server[] = "server";

int 	  sv_argc;
char 	  **sv_argv;

static void output();


/* MAIN -- The main program.
 */
main (argc, argv)
int argc;
char *argv[];
{
	rsDataPtr rtd = &rtdstat_data;
	Widget 	  toplevel;
	XtPointer obm;
	register  int  i;


        /* Initialize the application defaults.  We do this now so the 
	 * command-line and resource flags can override the defaults.
	 */
        rtd_initialize (rtd);

        /* Loop over the command line options and preprocess the ones that
         * are widget/GUI resources we want to make available more easily.
         * To do this we'll tweak the argument list so it appears to be a
         * "-xrm" resource setting, this means the X initialization code
         * below will do all the real work.
         */
        for (i=1; i < argc; i++) {
            if (strcmp (argv[1], "-help") == 0) {
                Usage ();
                exit (1);

            } else if (strcmp (argv[1], "-defgui") == 0) {
                for (i=0;  defgui_text[i];  i++)
                    printf ("%s\n", defgui_text[i]);
                exit (0);
            }
	}


        /* Get local copy of argc and argv. 
	 */
        if ((sv_argc = argc) > 0) {
            sv_argv = (char **) XtMalloc (argc * sizeof(char *));
            memmove (sv_argv, argv, argc * sizeof(char *));
        } else
            sv_argv = argv;

	/* Initialize applications context. 
	 */
	rtd->toplevel = toplevel = XtAppInitialize (&app_context, "rtdConsole",
	    (XrmOptionDescList) NULL, 0, &sv_argc, sv_argv,
	    (String *) NULL, (ArgList) NULL, 0);

	/* Free saved arglist.
	 */
	free ((char *)sv_argv);

        /* Get application resources. */
	if (!toplevel)
	    exit (1);
	else {
            XtVaGetApplicationResources (rtd->toplevel, rtd,
        	resources, XtNumber(resources),
        	/* Add any resource overrides here */
        	NULL);
	}


	/* Initialize the object manager. 
	 */
	if (!(obm = rtd->obm = (XtPointer) ObmOpen (app_context, argc, argv)))
	    exit (2);
	/*
	ObmAddCallback (obm, OBMCB_clientOutput|OBMCB_preserve, output, obm);
	*/


        /* Loop over the command line options. The default rtd structure
         * should be defined at this point so the command options can be
         * used to override them.
         */
        for (i=1; i < argc; i++) {

            if (strcmp (argv[i], "-cache") == 0) {
                strcpy (rtd->cache_file, argv[++i]);

            } else if (strcmp (argv[i], "-gui") == 0) {
                rtd->gui = argv[++i];

            } else if (strcmp (argv[i], "-host") == 0) {
                rtd->host = argv[++i];

            } else if (strcmp (argv[i], "-port") == 0) {
                rtd->port = atoi (argv[++i]);

            /* Skip any standard X toolkit flags, they're handled above. 
             */
            } else if (strcmp (argv[i], "-bg") == 0) { 		i++;
            } else if (strcmp (argv[i], "-fg") == 0) { 		i++;
            } else if (strcmp (argv[i], "-iconic") == 0) { 	   ;
            } else if (strcmp (argv[i], "-display") == 0) { 	i++;
            } else if (strcmp (argv[i], "-geometry") == 0) { 	i++;
            } else if (strcmp (argv[i], "-title") == 0) { 	i++;
            } else if (strcmp (argv[i], "-xrm") == 0) { 	i++;

            } else {
                fprintf (stderr, "Unrecognized flag '%s'\n", argv[i]);
                Usage();
                exit (1);
            }

	}


        /* Load the RTDSTAT GUI.  If the GUI name is "default" the builtin
         * default GUI is used.  This is stored as an array of text lines,
         * which we must append newlines to and concatenate together to
         * form the GUI message.
         */
        if (strcmp (rtd->gui, "default") == 0 ||
            (ObmDeliverMsgFromFile (obm, server, rtd->gui) != 0)) {

            register char *ip, *op;
            char *message;
            int i;

            message = (char *) malloc (100000);
            for (i=0, op=message;  ip = defgui_text[i];  i++) {
                while (*ip)
                    *op++ = *ip++;
                *op++ = '\n';
            }
            *op++ = '\0';

            ObmDeliverMsg (obm, server, message);
            free ((char *)message);
        }


        /* Initialize the client code. 
	 */
        rtd_clientOpen (rtd);

	/* Activate the GUI.  
	 */
	ObmActivate (obm);

	/* EXECUTE 
	 */
	XtAppMainLoop (app_context);
}


/*  rtd_initialize -- Initialize the RTDSTAT application.
 */
void
rtd_initialize (rtd)
rsDataPtr rtd;
{
    /*  Set the application defaults.
     */
    rtd->disp_enable	= TRUE;			/* initial display options */
    rtd->stat_enable	= TRUE;
    rtd->hdrs_enable	= FALSE;

    rtd->disp_interval	= DEF_INTERVAL;		/* update interval	   */
    rtd->disp_frame	= DEF_FRAME;
    rtd->stdimage	= DEF_FBCONFIG;		/* imt4400		   */
    rtd->width		= DEF_FBWIDTH;
    rtd->height		= DEF_FBHEIGHT;
   
    rtd->smc_interval	= DEF_INTERVAL;		/* update interval	   */

    sprintf (rtd->cache_file, DEF_CACHE, getpid());/* default cache file   */
}


/*  RTD_STATUS -- Send a status message to the GUI.
 */
void
rtd_status (msg)
char *msg;
{
}

/*  RTD_ERROR -- Send an error status message to the GUI.
 */
void
rtd_error (msg)
char *msg;
{
}



/* RTD_MESSAGE -- Send a message to the user interface.
 */
void
rtd_message (rtd, object, message)
register rsDataPtr rtd;
char *object;
char *message;
{
        char msgbuf[SZ_MSGBUF];

        sprintf (msgbuf, "setValue {%s}", message);
        ObmDeliverMsg (rtd->obm, object, msgbuf);
}


/* RTD_MSGI -- Like rtd_message, but the message is an integer value.
 */
void
rtd_msgi (rtd, object, value)
register rsDataPtr rtd;
char *object;
int value;
{
        char msgbuf[SZ_LINE];
        sprintf (msgbuf, "setValue {%d}", value);
        ObmDeliverMsg (rtd->obm, object, msgbuf);
}


/* RTD_ALERT -- Issue an alert to the server.  The message text input will
 * be displayed and either the ok (proceed) or cancel action will be taken,
 * causing the action text input to be sent back to the client to be
 * executed as a command.  This is used to alert the server (i.e. user) of
 * unusual circumstances and determine whether or not the server wants to
 * proceed.  An alert with no actions is a warning.
 */
void
rtd_alert (rtd, text, ok_action, cancel_action)
register rsDataPtr rtd;
char *text;                     /* message text */
char *ok_action;                /* command sent back to client for "ok" */
char *cancel_action;            /* command sent back to client for "cancel" */
{
        char msgbuf[SZ_LINE];
        sprintf (msgbuf, "setValue {{%s} {%s} {%s}}", text,
            ok_action ? ok_action : "", cancel_action ? cancel_action : "");
        ObmDeliverMsg (rtd->obm, "alert", msgbuf);
}


/******************************************************************************/


/* RTD_SHUTDOWN -- Shutdown the RTDSTAT.
 */
rtd_shutdown (rtd)
rsDataPtr rtd;
{
    
    rtd->smc = smcClose (rtd->smc, FALSE); 	/* close the SMC interface  */
    cdl_close (rtd->cdl); 			/* close the CDL interface */


    rtd_status ("Have a nice day.\n");
    exit (0);
}





/*  USAGE -- Print a list of command-line options.
 */
Usage ()
{
    fprintf (stderr, "Usage:\n\n");
    printoption ("    rtdstat");
    printoption ("[-defgui]");                  /* Print default GUI      */
    printoption ("[-gui <file>]");              /* GUI file               */
    printoption ("[-help]");                    /* Print help             */
    printoption ("[-host <name>]");             /* display host machine   */
    printoption ("[-port <num>]");              /* inet port              */
    fprintf (stderr,"\n");
}


/* PRINTOPTION -- Pretty-print an option string.
 */
static int cpos = 0;
printoption(st)
char    *st;
{
        if (strlen(st) + cpos > 78) {
            fprintf (stderr,"\n\t");
            cpos = 8;
        }
        fprintf (stderr,"%s ",st);
        cpos = cpos + strlen(st) + 1;
}
