#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Obm.h>

/*
 */

/* Compatibility hacks. */
#if defined(sun) && !defined(SYSV)
void *memmove(a,b,n) void *a, *b; int n; { bcopy(b,a,n); }
#endif

/* Data. */
XtAppContext app_context;
static char server[] = "server";
char **sv_argv;
int sv_argc;

static void output();


/* MAIN -- The main program.
 */
main (argc, argv)
int argc;
char *argv[];
{
	Widget toplevel;
	XtPointer obm;
	char *fname;

        /* Get local copy of argc and argv. */
        if ((sv_argc = argc) > 0) {
            sv_argv = (char **) XtMalloc (argc * sizeof(char *));
            memmove (sv_argv, argv, argc * sizeof(char *));
        } else
            sv_argv = argv;

	/* Initialize applications context. */
	toplevel = XtAppInitialize (&app_context, "irafConsole",
	    (XrmOptionDescList) NULL, 0, &sv_argc, sv_argv,
	    (String *) NULL, (ArgList) NULL, 0);

	/* Free saved arglist.
	 */
	free ((char *)sv_argv);
	if (!toplevel)
	    exit (1);

	/* Initialize the object manager. */
	if (!(obm = (XtPointer) ObmOpen (app_context, argc, argv)))
	    exit (2);
	ObmAddCallback (obm, OBMCB_clientOutput|OBMCB_preserve, output, obm);

	/* Open and execute the GUI file if there was one, otherwise read
	 * the GUI from the standard input.
	 */
	if (ObmDeliverMsgFromFile (obm, server, "hello.gui") != 0) {
	    fprintf (stderr, "error executing GUI \n");
	    exit (4);
	}

	/* Activate the GUI.  */
	ObmActivate (obm);

	/* EXECUTE */
	XtAppMainLoop (app_context);
}


/* OUTPUT -- The output callback is called when the GUI sends data or requests
 * to the "client" object, which is this routine in the case of obmsh.
 */
static void
output (obm, tcl, key, string)
XtPointer obm;
XtPointer tcl;			/* not used */
int key;
char *string;
{
	static int i = 0;

	ObmDeactivate (obm);
	ObmClose (obm);

	if (!(obm = (XtPointer) ObmOpen (app_context, sv_argc, sv_argv)))
	    exit (2);
	ObmAddCallback (obm, OBMCB_clientOutput|OBMCB_preserve, output, obm);

	ObmInitialize (obm);

	printf ("%04d\n", i++); 
	fflush (stdout);

	if (ObmDeliverMsgFromFile (obm, server, "hello.gui") != 0) {
	    fprintf (stderr, "error executing GUI \n");
	    exit (4);
	}
	ObmActivate (obm);
}
