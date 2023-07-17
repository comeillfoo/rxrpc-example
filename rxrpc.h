#ifndef _RXRPC_H_
#define _RXRPC_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/rxrpc.h>

#include <stdlib.h>
#include <string.h>

#define VL_SERVICE_ID 5555

#define LISTEN_BACKLOG 5
#define XSTR(X) STR(X)
#define STR(X) #X

static struct cmsghdr rxrpc_user_call_id()
{
    return (struct cmsghdr) {
        .cmsg_level = IPPROTO_UDP,
        .cmsg_type = RXRPC_USER_CALL_ID,
        .cmsg_len = sizeof(struct cmsghdr)
    };
}

static struct cmsghdr* rxrpc_tx_length(int64_t total_length)
{
    struct cmsghdr* hdr_p = malloc(sizeof(struct cmsghdr) + sizeof(int64_t));
    hdr_p->cmsg_level = IPPROTO_UDP;
    hdr_p->cmsg_type = RXRPC_TX_LENGTH;
    hdr_p->cmsg_len = sizeof(*hdr_p);
    memcpy(hdr_p->__cmsg_data, &total_length, sizeof(int64_t));
    return hdr_p;
}

static struct msghdr rxrpc_msghdr(struct sockaddr_rxrpc srx)
{
    return (struct msghdr) {
        .msg_name = &srx.transport.sin,
        .msg_namelen = sizeof(srx.transport.sin),
        .msg_control = NULL,
        .msg_controllen = 0,
        .msg_flags = 0,
        .msg_iov = NULL,
        .msg_iovlen = 0
    };
}

#endif // _RXRPC_H_