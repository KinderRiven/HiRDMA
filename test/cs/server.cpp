/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-19 12:14:20
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
        char* __data = _rbuf->buf(); // prepare data for client
        strcpy(__data, "Hi, This is Server!");
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

    // ----------------------------------
    int _tmp;
    int _num_msg = 5;
    size_t _size = 32;
    uint64_t _offset = 1024;
    // ----------------------------------
#if 0
    scanf("%d", &_tmp);
    for (int j = 0; j < _num_msg; j++) {
        char* _data = _rbuf->buf() + _offset;
        _offset += _size;
        for (int i = 0; i < _size; i++) {
            printf("%c", _data[i]);
        }
        printf("\n");
    }
#endif
    // --------- TEST RECEIVE -------------
#if 1
    _status = _hi_rdma->Receive(_rbuf, 0, _size);
    if (_status.ok()) {
        printf("[INFO] Receive Success!\n");
    } else {
        printf("[INFO] Receive Failed!\n");
    }
    scanf("%d", &_tmp);
    char* _data = _rbuf->buf();
    for (int i = 0; i < _size; i++) {
        printf("%c", _data[i]);
    }
    printf("\n");
#endif
    return 0;
}