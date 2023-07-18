#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/rxrpc.h>

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include "client.h"

static bool should_stop = false;

static void client_stop(int signal)
{
    (void) signal;
    should_stop = true;
}

int main(int argc, char** argv)
{
    (void) argc; (void) argv;
    int ret = 0;
    if (SIG_ERR == signal(SIGINT, client_stop)) {
        ret = errno;
        perror("Failed to redefine SIGINT handler");
        return ret;
    }

    int client = -1;
    if ((client = socket(AF_RXRPC, SOCK_DGRAM, PF_INET)) < 0) {
        ret = errno;
        perror("Failed to create client socket");
        return ret;
    }

    // server credentials
    struct sockaddr_rxrpc srx = {
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
        perror("Failed to connect to server");
        goto out;
    }

    // structs
    ssize_t tx_ret = 0, rx_ret = 0;
    struct rxrpc_structs
        rx = {0},
        tx = {0};

    // send RXRPC_USER_CALL_ID
    tx.msg.msg_control = tx.data.control;
    tx.msg.msg_controllen = rxrpc_add_user_call_id(
        (struct cmsghdr*) tx.data.control, 0, USER_ID);
    tx_ret = sendmsg(client, &tx.msg, 0);
    if (0 > tx_ret) {
        ret = errno;
        perror("Failed to send RXRPC_USER_CALL_ID(" XSTR(USER_ID) ")");
        goto out;
    }
    fprintf(stderr, "RXRPC_USER_CALL_ID(" XSTR(USER_ID) "): sent\n");

    // receive RXRPC_USER_CALL_ID + RXRPC_ACCEPT
    rx.msg.msg_name = rx.data.name;
    rx.msg.msg_namelen = sizeof(rx.data.name);
    rx.msg.msg_control = rx.data.control;
    rx.msg.msg_controllen = sizeof(rx.data.control);
    rx.msg.msg_iov = &rx.iov;
    rx.msg.msg_iovlen = 1;
    rx_ret = recvmsg(client, &rx.msg, 0);
    if (0 > rx_ret) {
        ret = errno;
        perror("Failed to receive RXRPC_USER_CALL_ID + RXRPC_ACCEPT");
        goto out;
    }

    while (!should_stop) {
        sleep(10);
    }

out:
    close(client);
    return ret;
}