/*
 * @Author: your name
 * @Date: 2021-08-11 15:16:46
 * @LastEditTime: 2021-08-11 16:42:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/include/rdma.hpp
 */

#ifndef INCLUDE_INCLUDE_RDMA_HPP_
#define INCLUDE_INCLUDE_RDMA_HPP_

#include "header.hpp"
#include "options.hpp"
#include "status.hpp"

namespace hi_rdma {

class HiRDMABuffer {
public:
    char* data();

    size_t size();

private:
    char* buf_;

    struct ibv_mr* mr_;
};

// only one qp support
class HiRDMA {
public: // initlizate
    static Status CreateRDMAContext(Options& options, HiRDMA** context);

    HiRDMABuffer* RegisterRDMABuffer(size_t size, int access_mode);

    Status ExchangeQPInfo();

    Status ConnectQP();

    Status PollQP();

public: // verb
    Status Write(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size);

    Status Read(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size);

private:
    struct ibv_context* dev_ctx_;
};

};

#endif