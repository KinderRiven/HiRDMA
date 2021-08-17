/*
 * @Author: your name
 * @Date: 2021-08-11 15:44:55
 * @LastEditTime: 2021-08-17 11:39:52
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

    struct ibv_port_attr _port_attr;
    struct ibv_device_attr _dev_attr;

    /* device list */
    _dev_list = ibv_get_device_list(&_num_dev);
    if (_num_dev == 0) {
        return Status::IOError("get device list failed.");
    }

    /* device & attribute */
    for (int i = 0; i < _num_dev; i++) {
        _dev = _dev_list[i];
        if (!strcmp(_dev->name, options.dev_name.c_str())) {
            _dev_ctx = ibv_open_device(_dev);
            if (_dev_ctx == nullptr) {
                return Status::IOError("open device failed.");
            } else {
                ibv_query_device(_dev_ctx, &_dev_attr);
                ibv_query_port(_dev_ctx, options.dev_port, &_port_attr);
                break;
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
    _qp_init_attr.send_cq = _dev_cq;
    _qp_init_attr.recv_cq = _dev_cq;
    _qp_init_attr.cap.max_send_wr = 1; // _dev_attr.max_qp_wr;
    _qp_init_attr.cap.max_recv_wr = 1; // _dev_attr.max_qp_wr;
    _qp_init_attr.cap.max_send_sge = 1;
    _qp_init_attr.cap.max_recv_sge = 1;
    _dev_qp = ibv_create_qp(_dev_pd, &_qp_init_attr);
    if (_dev_qp == nullptr) {
        return Status::IOError("create qp failed.");
    }

    *context = new HiRDMA(options.dev_name, options.dev_port, options.dev_idx, _dev, _dev_ctx, _dev_pd, _dev_cq, _dev_qp);
    (*context)->PrintInfo();
    return Status::OK();
}

HiRDMA::HiRDMA(std::string& dev_name, int dev_port, int dev_index, struct ibv_device* dev, struct ibv_context* ctx, struct ibv_pd* pd, struct ibv_cq* cq, struct ibv_qp* qp)
    : dev_name_(dev_name)
    , dev_port_(dev_port)
    , dev_idx_(dev_index)
    , dev_(dev)
    , dev_ctx_(ctx)
    , dev_pd_(pd)
    , dev_cq_(cq)
    , dev_qp_(qp)
{
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

void HiRDMA::PrintInfo()
{
    struct ibv_port_attr _port_attr;
    struct ibv_device_attr _dev_attr;

    ibv_query_device(dev_ctx_, &_dev_attr);
    ibv_query_port(dev_ctx_, dev_port_, &_port_attr);

    printf(">>[HiRDMA]\n");
    printf("  [name:%s][port:%d][index:%d]\n", dev_name_.c_str(), dev_port_, dev_idx_);
    printf("  [max_qp:%d][max_qp_wr:%d]\n", _dev_attr.max_qp, _dev_attr.max_qp_wr);
    printf("  [max_seg:%d][max_seg_rd:%d]\n", _dev_attr.max_sge, _dev_attr.max_sge_rd);
    printf("  [max_cq:%d][max_cqe:%d]\n", _dev_attr.max_cq, _dev_attr.max_cqe);
    printf("  [max_mr:%d][max_pd:%d]\n", _dev_attr.max_mr, _dev_attr.max_pd);
}

Status HiRDMA::Write(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size)
{
}

Status HiRDMA::Read(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size)
{
}