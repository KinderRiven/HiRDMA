/*
 * @Author: your name
 * @Date: 2021-08-11 15:44:55
 * @LastEditTime: 2021-08-12 17:49:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/src/hi_rdma.cpp
 */

#include "hi_rdma.hpp"

using namespace hi_rdma;

Status HiRDMA::CreateRDMAContext(Options& options, HiRDMA** context)
{
    int _res;
    int _num_dev = 0;
    struct ibv_device* _dev = nullptr;
    struct ibv_context* _dev_ctx = nullptr;
    struct ibv_device** _dev_list = nullptr;

    /* device list */
    _dev_list = ibv_get_device_list(&_num_dev);
    if (_num_dev == 0) {
        return Status::IOError();
    }

    /* device */
    for (int i = 0; i < _num_dev; i++) {
        _dev = _dev_list[i];
        if (_dev->name == options.dev_name) {
            dev_ = _dev;
            dev_ctx_ = ibv_open_device(dev_);
            if (dev_ctx_ != nullptr) {
                return Status::IOError();
            }
        }
    }

    /* pd */
    dev_pd_ = ibv_alloc_pd(dev_ctx_);
    if (dev_pd_ == nullptr) {
        return Status::IOError();
    }

    /* create cq */
    dev_cq_ = ibv_create_cq(dev_ctx_, 1, nullptr, nullptr, 0);
    if (dev_cq_ == nullptr) {
        return Status::IOError();
    }

    /* create qp */
    struct ibv_qp_init_attr _qp_init_attr;
    memset(&_qp_init_attr, 0, sizeof(_qp_init_attr));
    _qp_init_attr.qp_type = IBV_QPT_RC;
    _qp_init_attr.sq_sig_all = 1;
    _qp_init_attr.send_cq = dev_cq_;
    _qp_init_attr.recv_cq = dev_cq_;
    _qp_init_attr.cap.max_send_wr = 128;
    _qp_init_attr.cap.max_recv_wr = 128;
    _qp_init_attr.cap.max_send_sge = 128;
    _qp_init_attr.cap.max_recv_sge = 128;

    dev_qp_ = ibv_create_qp(dev_pd_, &_qp_init_attr);
    if (dev_qp_ == nullptr) {
        return Status::IOError();
    }
    return Status::OK();
}

HiRDMABuffer* HiRDMA::RegisterRDMABuffer(size_t size, int access_mode)
{
}

Status HiRDMA::ExchangeQPInfo()
{
}

Status HiRDMA::ConnectQP()
{
}

Status HiRDMA::PollQP()
{
}

Status HiRDMA::Write(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size)
{
}

Status HiRDMA::Read(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size)
{
}