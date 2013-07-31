#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __POSIX__
#include <sys/select.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#endif
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stddef.h>

#include "mbus.h"

extern int console;
extern int procDebug;



/**************************************************************************
**  MBUSMSGHANDLER -- Standard client application message handler.
**
*/
int
mbusMsgHandler (int fd, void *data)
{
    char *from = NULL, *to = NULL, *host = NULL, *msg = NULL;
    int  from_tid, to_tid, subject;
    int  mytid = mbAppGet(APP_TID);


    to_tid = subject = -1;
    if (mbusRecv(&from_tid, &to_tid, &subject, &host, &msg) < 0) {
	if (console && procDebug)
            fprintf (stderr, "Error in mbusRecv()\n");
	return (ERR);
    }

    /* See if message was meant specifically for me.... */
    if (to_tid == mytid && subject != MB_ERR) {
	if (console && procDebug)
            fprintf (stderr, "SENDTO:  from:%d  subj:%d msg='%s'\n",
		from_tid, subject, msg);
        myHandler (from_tid, subject, msg);

    } else if (to_tid < 0) {
	if (console && procDebug)
            fprintf (stderr, "BCAST:  from:%d  subj:%d msg='%s'\n",
		from_tid, subject, msg);
        myHandler (from_tid, subject, msg);

    } else {
	if (console && procDebug) {
            printf("Monitor...\n");
            printf("   from:%d\n   to:%d\n   subj:%d\n",
		from_tid, to_tid, subject);
            printf("   host:'%s'\n   msg='%s'\n", host, msg);
	}
    }

    return (OK);
}



/*  Local Message Handler
 */
myHandler(int from, int subject, char *msg)
{
    int   from_tid, to_tid, subj;
    int   tid = 0, pid = 0, status;
    int   mytid = mbAppGet (APP_TID);
    char  *txt, *me = mbAppGetName();
    char  who[128], host[128];


    switch (subject) {
    case MB_CONNECT:
	if (console && procDebug)
	    fprintf (stderr, "CONNECT on %s: %s\n", me, msg);

	mbParseConnectMsg (msg, &tid, &who, &host, &pid);

	/*  If it's the supervisor connecting, and we don't already have an
	 *  established connection to the Super, set it up now.
	 */
	if (isSupervisor (who) && mbAppGet (APP_STID) < 0) {
	    mytid = mbAppGet(APP_TID);
	    if (mbConnectToSuper (mytid, tid, host, pid) == OK)
		mbAppSet (APP_TID, abs(mytid));
	}

	/*  When we get a CONNECT message, post a notifier so we're
	 *  alerted whent the task exits.
	mbAddTaskExitHandler (tid, myTaskExited);
	 */

	break;

    case MB_EXITING:
	if (console && procDebug)
	    fprintf (stderr, "DISCONNECT on %s: %s\n", me, msg);
	break;

    case MB_STATUS:
	if (console && procDebug)
	    fprintf (stderr, "STATUS on %s: %s\n", me, msg);
	/* Send a Status response.... */
	break;

    case MB_ORPHAN:
	if (console && procDebug)
	    fprintf (stderr, "ORPHAN on %s: ", me);
	sscanf(msg, "Orphan {From: %d  To: %s  Subj: %d -- (%s)}",
	       &from_tid, &to_tid, &subj, txt);
	break;

    case MB_PING:
	if (console && procDebug)
	    fprintf (stderr, "PING on %s: %s\n", me, msg);
	/* Return an ACK/STATUS to sender .... */
	break;

    case MB_ERR:
	if (console && procDebug)
	    fprintf (stderr, "ERR on %s: %s\n", me, msg);
	break;

    default:
	if (console && procDebug) {
	    fprintf (stderr, "DEFAULT recv:%d:  ", subject);
	    fprintf (stderr, "   from:%d  subj:%d\n   msg='%s'\n",
		from, subject, msg);
	}
    }
}
