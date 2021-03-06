/*
 * @Author: your name
 * @Date: 2021-08-11 15:44:55
 * @LastEditTime: 2021-08-26 13:20:29
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/src/hi_rdma.cpp
 */

#include "hi_rdma.hpp"

using namespace hi_rdma;

static int modify_qp_to_init(struct ibv_qp* qp)
{
    struct ibv_qp_attr attr;
    int flags;
    memset(&attr, 0, sizeof(attr));
    attr.qp_state = IBV_QPS_INIT;
    attr.port_num = 1;
    attr.pkey_index = 0;
    attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_ATOMIC;
    flags = IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS;
    return ibv_modify_qp(qp, &attr, flags);
}

static int modify_qp_to_rtr(struct ibv_qp* qp, uint32_t remote_port, uint32_t remote_idx, uint32_t remote_qp_num, uint16_t remote_lid, uint8_t* remote_gid)
{
    struct ibv_qp_attr attr;
    int flags;
    memset(&attr, 0, sizeof(attr));
    attr.qp_state = IBV_QPS_RTR;
    attr.path_mtu = IBV_MTU_256;
    attr.dest_qp_num = remote_qp_num; // qp number
    attr.rq_psn = 0;
    attr.max_dest_rd_atomic = 1;
    attr.min_rnr_timer = 0x12;
    attr.ah_attr.is_global = 0;
    attr.ah_attr.dlid = remote_lid; // lid
    attr.ah_attr.sl = 0;
    attr.ah_attr.src_path_bits = 0;
    attr.ah_attr.port_num = remote_port; // port number
    attr.ah_attr.is_global = 1;
    memcpy(&attr.ah_attr.grh.dgid, remote_gid, 16); // GID
    attr.ah_attr.grh.flow_label = 0;
    attr.ah_attr.grh.hop_limit = 1;
    attr.ah_attr.grh.sgid_index = remote_idx; // index
    attr.ah_attr.grh.traffic_class = 0;
    flags = IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER;
    return ibv_modify_qp(qp, &attr, flags);
}

static int modify_qp_to_rts(struct ibv_qp* qp)
{
    int flags;
    struct ibv_qp_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.qp_state = IBV_QPS_RTS;
    attr.timeout = 12;
    attr.retry_cnt = 10;
    attr.rnr_retry = 0;
    attr.sq_psn = 0;
    attr.max_rd_atomic = 1;
    flags = IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN | IBV_QP_MAX_QP_RD_ATOMIC;
    return ibv_modify_qp(qp, &attr, flags);
}

/////////////////////////////// RDMA QP ///////////////////////////
HiRDMAQP::HiRDMAQP() { }

HiRDMAQP::HiRDMAQP(int port_num, int idx, int qp_num, int lid, struct ibv_cq* cq, struct ibv_qp* qp, union ibv_gid* gid)
    : port_num_(port_num)
    , qp_num_(qp_num)
    , idx_(idx)
    , lid_(lid)
    , cq_(cq)
    , qp_(qp)
{
    memcpy((void*)gid_, (void*)gid, sizeof(union ibv_gid));
}

void HiRDMAQP::Print()
{
    printf(">>[HiRDMAQP]\n");
    printf("  [port:%d][idx:%d][lid:%d][qp_num:%d]\n", port_num_, idx_, lid_, qp_num_);
}

Status HiRDMAQP::PollQP(int num)
{
    struct ibv_cq* cq = cq_;
    struct ibv_wc wc;
    while (true) {
        int n = ibv_poll_cq(cq, 1, &wc);
        if (n < 0) {
            return Status::IOError("PollQP Failed.");
        }
        if ((n) && (wc.status != IBV_WC_SUCCESS)) {
            printf(">>[Completion was found in CQ with error status][%s][%d]\n", ibv_wc_status_str(wc.status), n);
            printf("  [wr_id:%d][status:%d][opcode:%d]\n", wc.wr_id, wc.status, wc.opcode);
            printf("  [byte_len:%d][qp_num:%d][src_qp:%d]\n", wc.byte_len, wc.qp_num, wc.src_qp);
            return Status::IOError("PollQP Failed.");
        } else if ((n) && (wc.status == IBV_WC_SUCCESS)) {
            num -= n;
        }
        if (num <= 0) {
            break;
        }
    }
    return Status::OK();
}

Status HiRDMAQP::Write(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size)
{
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr* bad_wr = nullptr;

    // local buffer
    memset(&sge, 0, sizeof(sge));
    memcpy((void*)lbuf->buf(), buf, size);
    sge.addr = (uintptr_t)lbuf->buf(); // addr
    sge.length = size; // size
    sge.lkey = lbuf->lkey(); // lkey

    // prepare the send work request
    memset(&wr, 0, sizeof(wr));
    wr.next = nullptr;
    wr.wr_id = 0;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.opcode = IBV_WR_RDMA_WRITE;
    wr.send_flags = IBV_SEND_SIGNALED;
    wr.wr.rdma.remote_addr = (uint64_t)(rbuf->buf() + offset); // remote buffer
    wr.wr.rdma.rkey = rbuf->rkey(); // remote key

    // there is a receive request in the responder side, so we won't get any into RNR flow
    int ret = ibv_post_send(qp_, &wr, &bad_wr);
    return (ret == 0) ? Status::OK() : Status::IOError("write failed.");
}

Status HiRDMAQP::Read(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t offset, size_t size)
{
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr* bad_wr = nullptr;

    // local buffer
    memset(&sge, 0, sizeof(sge));
    sge.addr = (uintptr_t)lbuf->buf(); // addr
    sge.length = size; // size
    sge.lkey = lbuf->lkey(); // lkey

    // prepare the send work request
    memset(&wr, 0, sizeof(wr));
    wr.next = nullptr;
    wr.wr_id = 0;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.opcode = IBV_WR_RDMA_READ;
    wr.send_flags = IBV_SEND_SIGNALED;
    wr.wr.rdma.remote_addr = (uint64_t)(rbuf->buf() + offset); // remote buffer
    wr.wr.rdma.rkey = rbuf->rkey(); // remote key

    // there is a receive request in the responder side, so we won't get any into RNR flow
    int ret = ibv_post_send(qp_, &wr, &bad_wr);
    return (ret == 0) ? Status::OK() : Status::IOError("read failed.");
}

Status HiRDMAQP::Send(HiRDMABuffer* lbuf, uint64_t offset, char* buf, size_t size)
{
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr* bad_wr = nullptr;

    // local buffer
    memset(&sge, 0, sizeof(sge));
    memcpy((void*)lbuf->buf(), buf, size);
    sge.addr = (uintptr_t)lbuf->buf(); // addr
    sge.length = size; // size
    sge.lkey = lbuf->lkey(); // lkey

    // prepare the send work request
    memset(&wr, 0, sizeof(wr));
    wr.next = nullptr;
    wr.wr_id = 0;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.opcode = IBV_WR_SEND;
    wr.send_flags = IBV_SEND_SIGNALED;

    // there is a receive request in the responder side, so we won't get any into RNR flow
    int ret = ibv_post_send(qp_, &wr, &bad_wr);
    return (ret == 0) ? Status::OK() : Status::IOError("send failed.");
}

Status HiRDMAQP::Receive(HiRDMABuffer* lbuf, uint64_t offset, size_t size)
{
    struct ibv_sge sge;
    struct ibv_recv_wr wr;
    struct ibv_recv_wr* bad_wr;

    // local buffer
    memset(&sge, 0, sizeof(sge));
    sge.addr = (uintptr_t)lbuf->buf() + offset; // addr
    sge.length = size; // size
    sge.lkey = lbuf->lkey(); // lkey

    // prepare the send work request
    memset(&wr, 0, sizeof(wr));
    wr.next = nullptr;
    wr.wr_id = 0;
    wr.sg_list = &sge;
    wr.num_sge = 1;

    // there is a receive request in the responder side, so we won't get any into RNR flow
    int ret = ibv_post_recv(qp_, &wr, &bad_wr);
    return (ret == 0) ? Status::OK() : Status::IOError("read failed.");
}

Status HiRDMAQP::AtomicFetchAdd(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t add, uint64_t offset)
{
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr* bad_wr = nullptr;

    memset(&sge, 0, sizeof(sge)); // local buffer
    sge.addr = (uintptr_t)lbuf->buf(); // addr
    sge.length = 64; // uint64_t
    sge.lkey = lbuf->lkey(); // lkey

    // prepare the send work request
    memset(&wr, 0, sizeof(wr));
    wr.next = nullptr;
    wr.wr_id = 0;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.opcode = IBV_WR_ATOMIC_FETCH_AND_ADD;
    wr.send_flags = IBV_SEND_SIGNALED;
    wr.wr.atomic.remote_addr = (uint64_t)(rbuf->buf() + offset); // remote buffer
    wr.wr.atomic.rkey = rbuf->rkey();
    wr.wr.atomic.compare_add = add;

    // there is a receive request in the responder side, so we won't get any into RNR flow
    int ret = ibv_post_send(qp_, &wr, &bad_wr);
    return (ret == 0) ? Status::OK() : Status::IOError("atmic fetch add failed.");
}

Status HiRDMAQP::AtomicCompareSwap(HiRDMABuffer* lbuf, HiRDMABuffer* rbuf, uint64_t compare, uint64_t swap, uint64_t offset)
{
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr* bad_wr = nullptr;

    memset(&sge, 0, sizeof(sge)); // local buffer
    sge.addr = (uintptr_t)lbuf->buf(); // addr
    sge.length = 64; // uint64_t
    sge.lkey = lbuf->lkey(); // lkey

    // prepare the send work request
    memset(&wr, 0, sizeof(wr));
    wr.next = nullptr;
    wr.wr_id = 0;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.opcode = IBV_WR_ATOMIC_CMP_AND_SWP;
    wr.send_flags = IBV_SEND_SIGNALED;
    wr.wr.atomic.remote_addr = (uint64_t)(rbuf->buf() + offset);
    wr.wr.atomic.rkey = rbuf->rkey();
    wr.wr.atomic.compare_add = compare;
    wr.wr.atomic.swap = swap;

    // there is a receive request in the responder side, so we won't get any into RNR flow
    int ret = ibv_post_send(qp_, &wr, &bad_wr);
    return (ret == 0) ? Status::OK() : Status::IOError("compare and swap failed.");
}

/////////////////////////////// RDMA CONTEXT ///////////////////////////
Status HiRDMA::CreateRDMAContext(Options& options, HiRDMA** context)
{
    int _res;
    int _num_dev = 0;

    struct ibv_device* _dev = nullptr;
    struct ibv_context* _dev_ctx = nullptr;
    struct ibv_device** _dev_list = nullptr;
    struct ibv_pd* _dev_pd = nullptr;

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

    *context = new HiRDMA(options.dev_name, options.dev_port, options.dev_idx, _dev, _dev_ctx, _dev_pd);
    (*context)->PrintInfo();
    return Status::OK();
}

HiRDMA::HiRDMA(std::string& dev_name, int dev_port, int dev_index, struct ibv_device* dev, struct ibv_context* ctx, struct ibv_pd* pd)
    : dev_name_(dev_name)
    , dev_port_(dev_port)
    , dev_idx_(dev_index)
    , dev_(dev)
    , dev_ctx_(ctx)
    , dev_pd_(pd)
{
    struct ibv_port_attr _port_attr;
    ibv_query_port(dev_ctx_, dev_port_, &_port_attr);
    lid_ = _port_attr.lid;
    ibv_query_gid(dev_ctx_, dev_port_, dev_idx_, &gid_);
}

HiRDMABuffer* HiRDMA::RegisterRDMABuffer(size_t size, int access_mode)
{
    char* _buf = new char[size];
    struct ibv_mr* _mr = ibv_reg_mr(dev_pd_, _buf, size, access_mode);
    if (_mr == nullptr) {
        return nullptr;
    } else {
        HiRDMABuffer* _rbuf = new HiRDMABuffer(_mr);
        return _rbuf;
    }
}

HiRDMAQP* HiRDMA::RegisterQP()
{
    /* create cq */
    struct ibv_cq* _dev_cq = ibv_create_cq(dev_ctx_, 4096, nullptr, nullptr, 0);
    if (_dev_cq == nullptr) {
        return nullptr;
    }

    /* create qp */
    struct ibv_qp_init_attr _qp_init_attr;
    memset(&_qp_init_attr, 0, sizeof(_qp_init_attr));
    /*
    IBV_QPT_RC = 2,
    IBV_QPT_UC,
    IBV_QPT_UD,
    IBV_QPT_RAW_PACKET = 8,
    IBV_QPT_XRC_SEND = 9,
    IBV_QPT_XRC_RECV,
    IBV_QPT_DRIVER = 0xff,
    */
    _qp_init_attr.qp_type = IBV_QPT_RC;
    _qp_init_attr.sq_sig_all = 1;
    _qp_init_attr.send_cq = _dev_cq;
    _qp_init_attr.recv_cq = _dev_cq;
    _qp_init_attr.cap.max_send_wr = 4096; // _dev_attr.max_qp_wr / 2; // [???]
    _qp_init_attr.cap.max_recv_wr = 4096; // _dev_attr.max_qp_wr / 2; // [???]
    _qp_init_attr.cap.max_send_sge = 1;
    _qp_init_attr.cap.max_recv_sge = 1;

    struct ibv_qp* _dev_qp = ibv_create_qp(dev_pd_, &_qp_init_attr);
    if (_dev_qp == nullptr) {
        return nullptr;
    }

    HiRDMAQP* _qp = new HiRDMAQP(dev_port_, dev_idx_, _dev_qp->qp_num, lid_, _dev_cq, _dev_qp, &gid_);
    vec_qp_.push_back(_qp);
    return _qp;
}

Status HiRDMA::ConnectQP(HiRDMAQP* local_qp, HiRDMAQP* remote_qp)
{
    int _res;
    _res = modify_qp_to_init(local_qp->qp_);
    if (_res) {
        return Status::IOError("modify qp to init failed.");
    }
    _res = modify_qp_to_rtr(local_qp->qp_, remote_qp->port_num_, remote_qp->idx_, remote_qp->qp_num_, remote_qp->lid_, remote_qp->gid_);
    if (_res) {
        return Status::IOError("modify qp to rtr failed.");
    }
    _res = modify_qp_to_rts(local_qp->qp_);
    if (_res) {
        return Status::IOError("modify qp to rts failed.");
    }
    return Status::OK();
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