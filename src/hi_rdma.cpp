/*
 * @Author: your name
 * @Date: 2021-08-11 15:44:55
 * @LastEditTime: 2021-08-12 17:59:33
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
    struct ibv_pd* _dev_pd = nullptr;
    struct ibv_cq* _dev_cq = nullptr;
    struct ibv_qp* _dev_qp = nullptr;

    /* device list */
    _dev_list = ibv_get_device_list(&_num_dev);
    if (_num_dev == 0) {
        return Status::IOError("get device list failed.");
    }

    /* device */
    for (int i = 0; i < _num_dev; i++) {
        _dev = _dev_list[i];
        if (_dev->name == options.dev_name) {
            _dev_ctx = ibv_open_device(_dev);
            if (_dev_ctx != nullptr) {
                return Status::IOError("open device failed.");
            }
        }
    }

    /* pd */
    _dev_pd = ibv_alloc_pd(_dev_ctx);
    if (_dev_pd == nullptr) {
        return Status::IOError("allocate pd failed.");
    }

    /* create cq */
    _dev_cq = ibv_create_cq(_dev_ctx, 1, nullptr, nullptr, 0);
    if (_dev_cq == nullptr) {
        return Status::IOError("create cq failed.");
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
    _dev_qp = ibv_create_qp(_dev_pd, &_qp_init_attr);
    if (_dev_qp == nullptr) {
        return Status::IOError("create qp failed.");
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