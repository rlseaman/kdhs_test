#include <stdio.h>		/* for printf() and fprintf()           */
#include <sys/types.h>		/* for SO_REUSEADDR                     */
#include <sys/socket.h>		/* for socket(), bind(), and connect()  */
#include <arpa/inet.h>		/* for sockaddr_in and inet_ntoa()      */
#include <stdlib.h>		/* for atoi() and exit()                */
#include <string.h>		/* for memset()                         */

#define MAXCONN 	5	/* Max connection requests              */


/*************************************************************************
**  colOpenPort -- Create a socket connection on the specified port we'll
**  use to accept client connections.
*/
int
colOpenPort(unsigned short port)
{
    struct sockaddr_in sockaddr;	/* local address        */
    int sock = 0;			/* socket to create     */
    int reuse = 1;			/* make reusable socket */
    extern int procDebug;		


    /* Create socket for incoming connections.
     */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	goto err_;

    /* Construct local address structure.
     */
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);			/* local port    */
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* any interface */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse,
		   sizeof(reuse)) < 0)
	goto err_;

    /* Bind to the local address.
     */
    if (bind(sock, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0)
	goto err_;

    /* Mark the socket so it will listen for incoming connections.
     */
    if (listen(sock, MAXCONN) < 0)
	goto err_;

    if (procDebug)
	fprintf (stderr, "Opened socket on port %d, fd=%d\n", port, sock);


    return sock;

  err_:
    perror("colOpenPort:  Error opening Collector inet port");
    exit(1);
}
