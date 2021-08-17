/*
 * @Author: your name
 * @Date: 2021-08-17 14:18:33
 * @LastEditTime: 2021-08-17 15:43:05
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/src/sock.cc
 */

#include "sock.hpp"

using namespace hi_rdma;

int Socket::Connect(const char* ip, const char* port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sock_fd = -1, ret = 0;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    ret = getaddrinfo(ip, port, &hints, &result);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock_fd == -1) {
            continue;
        }
        ret = connect(sock_fd, rp->ai_addr, rp->ai_addrlen);
        if (ret == 0) {
            break;
        }
        close(sock_fd);
        sock_fd = -1;
    }
    freeaddrinfo(result);
    return sock_fd;
}

int Socket::Accept(const char* port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sock_fd = -1, ret = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(NULL, port, &hints, &result);
    if (ret) {
        return -1;
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock_fd < 0) {
            continue;
        }
        ret = bind(sock_fd, rp->ai_addr, rp->ai_addrlen);
        if (ret == 0) {
            break;
        }
        close(sock_fd);
        sock_fd = -1;
    }
    freeaddrinfo(result);

    ret = listen(sock_fd, 10);
    if (ret) {
        return -1;
    }

    int accept_sockfd;
    struct sockaddr_in accept_addr;
    socklen_t accept_addr_len = sizeof(struct sockaddr_in);
    accept_sockfd = accept(sock_fd, (struct sockaddr*)&accept_addr, &accept_addr_len);
    return accept_sockfd;
}

uint32_t Socket::Read(int sock_fd, char* buf, size_t len)
{
    size_t nr, tot_read;
    tot_read = 0;
    while (len != 0 && (nr = read(sock_fd, buf, len)) != 0) {
        if (nr < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        }
        len -= nr;
        buf += nr;
        tot_read += nr;
    }
    return tot_read;
}

uint32_t Socket::Write(int sock_fd, char* buf, size_t len)
{
    size_t nw, tot_written;
    for (tot_written = 0; tot_written < len;) {
        nw = write(sock_fd, buf, len - tot_written);
        if (nw <= 0) {
            if (nw == -1 && errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        }
        tot_written += nw;
        buf += nw;
    }
    return tot_written;
}