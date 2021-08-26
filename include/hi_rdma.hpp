/*
 * @Author: your name
 * @Date: 2021-08-11 15:16:46
 * @LastEditTime: 2021-08-26 13:16:35
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

class HiRDMA;

enum access_mode_t {
    LOCAL_WR = IBV_ACCESS_LOCAL_WRITE,
    REMOTE_WR = IBV_ACCESS_REMOTE_WRITE,
    REMOTE_RD = IBV_ACCESS_REMOTE_READ,
    REMOTE_ATOMIC = IBV_ACCESS_REMOTE_ATOMIC,
};

class HiRDMABuffer {
public:
    HiRDMABuffer()
        : mr_(nullptr)
    {
    }

    HiRDMABuffer(struct ibv_mr* mr)
        : mr_(mr)
    {
        addr_ = (char*)mr_->addr;
        length_ = mr_->length;
        lkey_ = mr_->lkey;
        rkey_ = mr_->rkey;
    }

    void Print()
    {
        printf(">>[HiRDMABuffer]\n");
        printf("  [addr:0x%llx][length:%.2fMB]\n", (uint64_t)addr_, 1.0 * length_ / (1024 * 1024));
        printf("  [lkey:%d][rkey:%d]\n", lkey_, rkey_);
    }

    char* buf() { return (char*)addr_; }

    int length() { return length_; }

    int lkey() { return lkey_; }

    int rkey() { return rkey_; }

    struct ibv_mr* mr() { return mr_; }

private:
    struct ibv_mr* mr_;

    char* addr_;

    int length_;

    int lkey_;

    int rkey_;
};

class HiRDMAQP {
public:
    HiRDMAQP() { }

    HiRDMAQP(int port_num, int idx, int qp_num, int lid, struct ibv_cq* cq, struct ibv_qp* qp, union ibv_gid* gid);

    void Print();

    Status PollQP(int num);

    Status Write(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size);

    Status Read(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t offset, size_t size);

    Status Send(HiRDMABuffer* lbuf, uint64_t offset, char* buf, size_t size);

    Status Receive(HiRDMABuffer* lbuf, uint64_t offset, size_t size);

    Status AtomicFetchAdd(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t add, uint64_t offset);

    Status AtomicCompareSwap(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t compare, uint64_t swap, uint64_t offset);

    friend class HiRDMA;

private:
    int lid_;

    int idx_;

    int qp_num_;

    int port_num_;

    uint8_t gid_[16];

    struct ibv_cq* cq_;

    struct ibv_qp* qp_;
};

// only one qp support
class HiRDMA {
public: // initlizate
    HiRDMA() = delete;

    static Status CreateRDMAContext(Options& options, HiRDMA** context);

    static Status ConnectQP(HiRDMAQP* local_qp, HiRDMAQP* remote_qp);

    HiRDMABuffer* RegisterRDMABuffer(size_t size, int access_mode);

    HiRDMAQP* RegisterQP();

    void PrintInfo();

private:
    // only support private initlizate
    HiRDMA(std::string& dev_name, int dev_port, int dev_index, struct ibv_device* dev, struct ibv_context* ctx, struct ibv_pd* pd);

private:
    std::string dev_name_;

    int lid_;

    int dev_idx_;

    int dev_port_;

    union ibv_gid gid_;

    struct ibv_device* dev_;

    struct ibv_context* dev_ctx_; // only one dev context

    struct ibv_pd* dev_pd_; // only one protect domain

    std::vector<HiRDMAQP*> vec_qp_;

    std::vector<HiRDMABuffer*> vec_buf_;
};

};

#endif