#ifndef _RXRPC_H_
#define _RXRPC_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/rxrpc.h>

#include <stdlib.h>
#include <string.h>

#define USER_ID 42
#define VL_SERVICE_ID 5555

#define XSTR(X) STR(X)
#define STR(X) #X

#define MAX_SIZE 4096

struct rxrpc_data {
    uint8_t name[MAX_SIZE];
    uint8_t control[MAX_SIZE];
    uint8_t iovec[MAX_SIZE];
};

struct rxrpc_structs {
    struct msghdr msg;
    struct iovec iov;
    struct rxrpc_data data;
};

static size_t rxrpc_add_user_call_id(struct cmsghdr* control,
    size_t ctrllen, unsigned long id)
{
    struct cmsghdr* hdr_p = (struct cmsghdr*)((uint8_t*)((void*)control) + ctrllen);
    hdr_p->cmsg_level = SOL_RXRPC;
    hdr_p->cmsg_len = CMSG_LEN(sizeof(unsigned long));
    hdr_p->cmsg_type = RXRPC_USER_CALL_ID;
    *(unsigned long*)CMSG_DATA(hdr_p) = id;
    return ctrllen + hdr_p->cmsg_len;
}

#endif // _RXRPC_H_