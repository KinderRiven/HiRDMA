/*
 * @Author: your name
 * @Date: 2021-08-17 14:18:41
 * @LastEditTime: 2021-08-17 15:06:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/include/sock.hpp
 */

#ifndef INCLUDE_SOCK_HPP_
#define INCLUDE_SOCK_HPP_

#include "header.hpp"

namespace hi_rdma {

class Socket {
public:
    static int Connect(const char* ip, const char* port);

    static int Accept(const char* port);

    static uint32_t Read(int sock_fd, char* buf, size_t size);

    static uint32_t Write(int sock_fd, char* buf, size_t size);
};

};

#endif