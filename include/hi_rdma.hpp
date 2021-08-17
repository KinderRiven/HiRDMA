/*
 * @Author: your name
 * @Date: 2021-08-11 15:16:46
 * @LastEditTime: 2021-08-17 13:55:10
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

enum access_mode_t {
    LOCAL_WR = IBV_ACCESS_LOCAL_WRITE,
    REMOTE_WR = IBV_ACCESS_REMOTE_WRITE,
    REMOTE_RD = IBV_ACCESS_REMOTE_READ,
};

struct HiRDMAQPInfo {
public:
    HiRDMAQPInfo(int qp_num, int lid, struct ibv_qp* qp, union ibv_gid* gid)
        : qp_num_(qp_num)
        , lid_(lid)
        , qp_(qp)
    {
        memcpy((void*)gid_, (void*)gid, 16);
    }

private:
    int lid_;

    int qp_num_;

    struct ibv_qp* qp_;

    uint8_t gid_[16];
};

class HiRDMABuffer {
public:
    HiRDMABuffer(struct ibv_mr* mr)
        : mr_(mr)
    {
    }

    char* buf() { return (char*)mr_->addr; }

    int length() { return mr_->length; }

    int lkey() { return mr_->lkey; }

    int rkey() { return mr_->rkey; }

    struct ibv_mr* mr() { return mr_; }

private:
    struct ibv_mr* mr_;
};

// only one qp support
class HiRDMA {
public: // initlizate
    static Status CreateRDMAContext(Options& options, HiRDMA** context);

    HiRDMABuffer* RegisterRDMABuffer(size_t size, int access_mode);

    Status ConnectQP(HiRDMAQPInfo* qp_info);

    Status PollQP();

    void PrintInfo();

public: // verb
    HiRDMA() = delete;

    Status Write(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size);

    Status Read(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size);

private:
    HiRDMA(std::string& dev_name, int dev_port, int dev_index, struct ibv_device* dev, struct ibv_context* ctx, struct ibv_pd* pd, struct ibv_cq* cq, struct ibv_qp* qp);

private:
    std::string dev_name_;

    int dev_port_;

    int dev_idx_;

    struct ibv_device* dev_;

    struct ibv_context* dev_ctx_;

    struct ibv_pd* dev_pd_;

    struct ibv_cq* dev_cq_;

    struct ibv_qp* dev_qp_;
};

};

#endif