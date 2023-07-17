#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/rxrpc.h>

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include "rxrpc.h"

static bool should_stop = false;

static void server_stop(int signal)
{
    (void) signal;
    should_stop = true;
}


int main(int argc, char** argv)
{
    (void) argc; (void) argv;
    int ret = 0;
    if (SIG_ERR == signal(SIGINT, server_stop)) {
        ret = errno;
        perror("Unable to redefine SIGINT handler");
        return ret;
    }

    int server = -1;
    if ((server = socket(AF_RXRPC, SOCK_DGRAM, PF_INET)) < 0) {
        ret = errno;
        perror("Unable to create server socket");
        return ret;
    }

    // create RxRCP service
    const struct sockaddr_rxrpc srx = {
		.srx_family	= AF_RXRPC,
		.srx_service = VL_SERVICE_ID, /* RxRPC service ID */
        .transport_len = sizeof(struct sockaddr_in),
		.transport_type	= SOCK_DGRAM, /* type of transport socket */
		.transport.sin.sin_family = AF_INET,
        .transport.sin.sin_port	= htons(7000), /* AFS callback */
        .transport.sin.sin_addr	= { 0 } /* all local interfaces */
	};
	if (0 > bind(server,
        (const struct sockaddr*) &srx, sizeof(struct sockaddr_rxrpc))) {
            ret = errno;
            perror("Unable to bind server socket");
            goto out;
    }
    if (0 > listen(server, LISTEN_BACKLOG)) {
        ret = errno;
        perror("Unable to listen with " XSTR(LISTEN_BACKLOG) " backlog");
        goto out;
    }

    while (!should_stop) {
    }

out:
    close(server);
    return ret;
}