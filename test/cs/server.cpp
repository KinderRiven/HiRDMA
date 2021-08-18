/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-18 14:23:09
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
    hi_rdma::HiRDMABuffer* _rbuf = nullptr;
    hi_rdma::Status _status = hi_rdma::HiRDMA::CreateRDMAContext(_options, &_hi_rdma);
    if (_status.ok()) {
        printf(">> CreateRDMAContext Success!\n");
    } else {
        printf(">> CreateRDMAContext Failed! [%s]\n", _status.ToString().c_str());
    }
    _rbuf = _hi_rdma->RegisterRDMABuffer(1048576UL, hi_rdma::LOCAL_WR | hi_rdma::REMOTE_RD | hi_rdma::REMOTE_WR);
    if (_rbuf != nullptr) {
        printf(">> RegisterRDMABuffer Success!\n");
        _rbuf->Print();
    } else {
        printf(">> RegisterRDMABuffer Failed!\n");
    }

    int sock_fd = hi_rdma::Socket::Accept(server_port);
    if (sock_fd != -1) {
        printf("accept = %d\n", sock_fd);
        hi_rdma::HiRDMAQPInfo _remote_qp;
        hi_rdma::HiRDMAQPInfo* _local_qp = _hi_rdma->AcquireQPInfo();
        hi_rdma::Socket::Read(sock_fd, (char*)&_remote_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        hi_rdma::Socket::Write(sock_fd, (char*)_local_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        _local_qp->Print();
        _remote_qp.Print();
        _status = _hi_rdma->ConnectQP(_local_qp, &_remote_qp);
        if (_status.ok()) {
            printf(">> Connect QP Success!\n");
        } else {
            printf(">> Connect QP Failed!\n");
        }
        hi_rdma::Socket::Write(sock_fd, (char*)_rbuf, sizeof(hi_rdma::HiRDMABuffer));
    }

    int _tmp;
    char* _data = _rbuf->buf() + 1024;
    scanf("%d", &_tmp);
    for (int i = 0; i < 16; i++) {
        printf("%c", _data[i]);
    }
    printf("\n");
    return 0;
}