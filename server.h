#ifndef _SERVER_H_
#define _SERVER_H_

#include "rxrpc.h"

#define LISTEN_BACKLOG 5

static size_t rxrpc_add_accept(struct cmsghdr* control,
    size_t ctrllen)
{
    struct cmsghdr* hdr_p = (struct cmsghdr*)((uint8_t*)((void*)control) + ctrllen);
    hdr_p->cmsg_level = SOL_RXRPC;
    hdr_p->cmsg_len = CMSG_LEN(0);
    hdr_p->cmsg_type = RXRPC_USER_CALL_ID;
    return ctrllen + hdr_p->cmsg_len;
}

#endif // _SERVER_H_