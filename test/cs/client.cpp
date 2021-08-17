/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-17 16:03:35
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/test/example/demo_1.cpp
 */

#include "config.hpp"
#include "hi_rdma.hpp"
#include "sock.hpp"

int main(int argc, char** argv)
{
    hi_rdma::Options _options;
    hi_rdma::HiRDMA* _hi_rdma = nullptr;
    hi_rdma::Status _status = hi_rdma::HiRDMA::CreateRDMAContext(_options, &_hi_rdma);
    if (_status.ok()) {
        printf(">> CreateRDMAContext Success!\n");
        printf("%s\n", _status.ToString().c_str());
    } else {
        printf(">> CreateRDMAContext Failed!\n");
        printf("   [status:%s]\n", _status.ToString().c_str());
    }
    for (auto i = 0; i < 3; i++) {
        hi_rdma::HiRDMABuffer* _rbuf = _hi_rdma->RegisterRDMABuffer(1048576UL,
            hi_rdma::LOCAL_WR | hi_rdma::REMOTE_RD | hi_rdma::REMOTE_WR);
        if (_rbuf != nullptr) {
            printf(">> RegisterRDMABuffer Success!\n");
            printf("   [addr:0x%llx][length:%.2fMB]\n", (uint64_t)_rbuf->buf(), 1.0 * _rbuf->length() / (1024 * 1024));
            printf("   [lkey:%d][rkey:%d]\n", _rbuf->lkey(), _rbuf->rkey());
        } else {
            printf(">> RegisterRDMABuffer Failed!\n");
        }
    }

    int sock_fd = hi_rdma::Socket::Connect(server_ip, server_port);
    if (sock_fd != -1) {
        printf("connect = %d\n", sock_fd);
        hi_rdma::HiRDMAQPInfo _remote_qp;
        hi_rdma::HiRDMAQPInfo* _local_qp = _hi_rdma->AcquireQPInfo();
        hi_rdma::Socket::Write(sock_fd, (char*)_local_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        hi_rdma::Socket::Read(sock_fd, (char*)&_remote_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        _local_qp->Print();
        _remote_qp.Print();
    }
    return 0;
}