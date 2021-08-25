/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-25 16:06:58
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/test/example/demo_1.cpp
 */

#include "config.hpp"
#include "hi_rdma.hpp"
#include "sock.hpp"

static void run_server_thread(int thread_id)
{
    hi_rdma::Options _options;
    hi_rdma::HiRDMA* _hi_rdma = nullptr;
    hi_rdma::HiRDMABuffer* _rbuf = nullptr;

    hi_rdma::Status _status = hi_rdma::HiRDMA::CreateRDMAContext(_options, &_hi_rdma);
    if (_status.ok()) {
        printf("[%d][INFO] CreateRDMAContext Success!\n", thread_id);
    } else {
        printf("[%d][INFO] CreateRDMAContext Failed! [%s]\n", thread_id, _status.ToString().c_str());
    }

    _rbuf = _hi_rdma->RegisterRDMABuffer(1048576UL, hi_rdma::LOCAL_WR | hi_rdma::REMOTE_RD | hi_rdma::REMOTE_WR | hi_rdma::REMOTE_ATOMIC);
    if (_rbuf != nullptr) {
        printf("[%d][INFO] RegisterRDMABuffer Success!\n", thread_id);
        _rbuf->Print();
        char* __data = _rbuf->buf();
        strcpy(__data, "Hi, This is Server!");
    } else {
        printf("[%d][INFO] RegisterRDMABuffer Failed!\n", thread_id);
    }

    int sock_fd = hi_rdma::Socket::Accept(server_port);
    if (sock_fd != -1) {
        printf("[%d][INFO] accept = %d\n", sock_fd);
        hi_rdma::HiRDMAQPInfo _remote_qp;
        hi_rdma::HiRDMAQPInfo* _local_qp = _hi_rdma->AcquireQPInfo();
        hi_rdma::Socket::Read(sock_fd, (char*)&_remote_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        hi_rdma::Socket::Write(sock_fd, (char*)_local_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        _local_qp->Print();
        _remote_qp.Print();
        _status = _hi_rdma->ConnectQP(_local_qp, &_remote_qp);
        if (_status.ok()) {
            printf("[%d][INFO] Connect QP Success!\n", thread_id);
        } else {
            printf("[%d][INFO] Connect QP Failed!\n", thread_id);
        }
        hi_rdma::Socket::Write(sock_fd, (char*)_rbuf, sizeof(hi_rdma::HiRDMABuffer));
    }
}

int main(int argc, char** argv)
{
    std::thread _server[64];
    for (auto i = 0; i < kNumClient; i++) {
        _server[i] = std::thread(run_server_thread, i);
    }
    for (auto i = 0; i < kNumClient; i++) {
        _server[i].join();
    }
    return 0;
}