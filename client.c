#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/rxrpc.h>

#include <errno.h>
#include <stdio.h>

#include "rxrpc.h"

int main(int argc, char** argv)
{
    (void) argc; (void) argv;
    int ret = 0;
    int client = -1;
    if ((client = socket(AF_RXRPC, SOCK_DGRAM, PF_INET)) < 0) {
        ret = errno;
        perror("Unable to create client socket");
        return ret;
    }

    // server credentials
    const struct sockaddr_rxrpc srx = {
		.srx_family	= AF_RXRPC,
		.srx_service = VL_SERVICE_ID, /* RxRPC service ID */
        .transport_len = sizeof(struct sockaddr_in),
		.transport_type	= SOCK_DGRAM, /* type of transport socket */
		.transport.sin.sin_family = AF_INET,
        .transport.sin.sin_port	= htons(7000), /* AFS callback */
        .transport.sin.sin_addr	= { 0 } /* all local interfaces */
	};
	if (0 > connect(client, (const struct sockaddr*) &srx, sizeof(struct sockaddr_rxrpc))) {
        ret = errno;
        perror("Unable to connect to server");
        goto out;
    }

    struct cmsghdr ucid = rxrpc_user_call_id();
    struct msghdr hello = rxrpc_msghdr(srx);
    hello.msg_control = &ucid;
    hello.msg_controllen = sizeof(ucid);
    if (0 > sendmsg(client, &hello, 0)) {
        ret = errno;
        perror("Failed to send hello message");
        goto out;
    }

out:
    close(client);
    return ret;
}