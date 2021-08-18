/*
 * @Author: your name
 * @Date: 2021-08-11 15:16:46
 * @LastEditTime: 2021-08-18 14:05:53
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
};

struct HiRDMAQPInfo {
public:
    HiRDMAQPInfo() { }

    HiRDMAQPInfo(int port_num, int idx, int qp_num, int lid, struct ibv_qp* qp, union ibv_gid* gid)
        : port_num_(port_num)
        , qp_num_(qp_num)
        , idx_(idx)
        , lid_(lid)
        , qp_(qp)
    {
        memcpy((void*)gid_, (void*)gid, 16);
    }

    void Print()
    {
        printf(">>[HiRDMAQP]\n");
        printf("  [port:%d][idx:%d][lid:%d][qp_num:%d]\n", port_num_, idx_, lid_, qp_num_);
    }

    friend class HiRDMA;

private:
    int lid_;

    int qp_num_;

    int port_num_;

    int idx_;

    struct ibv_qp* qp_;

    uint8_t gid_[16];
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

// only one qp support
class HiRDMA {
public: // initlizate
    static Status CreateRDMAContext(Options& options, HiRDMA** context);

    HiRDMABuffer* RegisterRDMABuffer(size_t size, int access_mode);

    HiRDMAQPInfo* AcquireQPInfo();

    Status ConnectQP(HiRDMAQPInfo* local_qp, HiRDMAQPInfo* remote_qp);

    Status PollQP();

    void PrintInfo();

public: // verb
    HiRDMA() = delete;

    Status Write(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size);

    Status Read(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size);

private:
    // only support private initlizate
    HiRDMA(std::string& dev_name, int dev_port, int dev_index, struct ibv_device* dev, struct ibv_context* ctx, struct ibv_pd* pd, struct ibv_cq* cq, struct ibv_qp* qp);

    // Transition a QP from the RESET to INIT state
    int modify_qp_to_init(struct ibv_qp* qp);

    // Transition a QP from the INIT to RTR state, using the specified QP number
    int modify_qp_to_rtr(struct ibv_qp* qp, uint32_t remote_port, uint32_t remote_idx, uint32_t remote_qp_num, uint16_t remote_lid, uint8_t* remote_gid);

    // Transition a QP from the RTR to RTS state
    int modify_qp_to_rts(struct ibv_qp* qp);

private:
    std::string dev_name_;

    int dev_port_;

    int dev_idx_;

    int lid_;

    struct ibv_device* dev_;

    union ibv_gid gid_;

    struct ibv_context* dev_ctx_;

    struct ibv_pd* dev_pd_;

    struct ibv_cq* dev_cq_;

    struct ibv_qp* dev_qp_;
};

};

#endif