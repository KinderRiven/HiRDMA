/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-19 11:17:34
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
    hi_rdma::HiRDMABuffer _remote_buf;
    hi_rdma::HiRDMABuffer* _local_buf = nullptr;

    hi_rdma::Status _status = hi_rdma::HiRDMA::CreateRDMAContext(_options, &_hi_rdma);
    if (_status.ok()) {
        printf("[INFO] CreateRDMAContext Success!\n");
    } else {
        printf("[INFO] CreateRDMAContext Failed! [%s]\n", _status.ToString().c_str());
    }
    _local_buf = _hi_rdma->RegisterRDMABuffer(1048576UL, hi_rdma::LOCAL_WR | hi_rdma::REMOTE_RD | hi_rdma::REMOTE_WR);
    if (_local_buf != nullptr) {
        printf("[INFO] RegisterRDMABuffer Success!\n");
        _local_buf->Print();
    } else {
        printf("[INFO] RegisterRDMABuffer Failed!\n");
    }

    int sock_fd = hi_rdma::Socket::Connect(server_ip, server_port);
    if (sock_fd != -1) {
        printf("connect = %d\n", sock_fd);
        // qp info read
        hi_rdma::HiRDMAQPInfo _remote_qp;
        hi_rdma::HiRDMAQPInfo* _local_qp = _hi_rdma->AcquireQPInfo();
        hi_rdma::Socket::Write(sock_fd, (char*)_local_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        hi_rdma::Socket::Read(sock_fd, (char*)&_remote_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        _local_qp->Print();
        _remote_qp.Print();
        _status = _hi_rdma->ConnectQP(_local_qp, &_remote_qp);
        if (_status.ok()) {
            printf("[INFO] Connect QP Success!\n");
        } else {
            printf("[INFO] Connect QP Failed!\n");
        }
        // buffer read
        hi_rdma::Socket::Read(sock_fd, (char*)&_remote_buf, sizeof(hi_rdma::HiRDMABuffer));
        _remote_buf.Print();
    }

    // ------------------------------------
    char _data1[32];
    int _num_msg = 5;
    size_t _size = 32;
    uint64_t _offset = 1024;

#if 1
    // ------------ TEST WRITE ------------
    // write to remote [offset]
    for (int i = 0; i < _num_msg; i++) {
        sprintf(_data1, "%s [%d]", "TEST WRITE.", i);
        _status = _hi_rdma->Write(_local_buf, &_remote_buf, _offset, _data1, _size);
        _offset += _size;
        if (_status.ok()) {
            printf("[INFO] Write Success!\n");
        } else {
            printf("[INFO] Write Failed!\n");
        }
    }
    _status = _hi_rdma->PollQP(_num_msg);
    if (_status.ok()) {
        printf("[INFO] PollQP Write Success!\n");
    } else {
        printf("[INFO] PollQP Write Failed!\n");
    }
#endif

#if 1
    // ------------ TEST READ ------------
    _status = _hi_rdma->Read(_local_buf, &_remote_buf, 0, _size);
    if (_status.ok()) {
        printf("[INFO] Read Success!\n");
    } else {
        printf("[INFO] Read Failed!\n");
    }
    _status = _hi_rdma->PollQP(1); // poll
    if (_status.ok()) {
        printf("[INFO] PollQP Read Success!\n");
        char* __data = _local_buf->buf();
        for (int i = 0; i < _size; i++) {
            printf("%c", __data[i]);
        }
        printf("\n");
    } else {
        printf("[INFO] PollQP Read Failed!\n");
    }
#endif

#if 1
    // ------------ TEST SEND ------------
    char _data2[32] = "TEST SEND.";
    _size = 32;
    _status = _hi_rdma->Send(_local_buf, 0, _data2, _size);
    if (_status.ok()) {
        printf("[INFO] Send Success!\n");
    } else {
        printf("[INFO] Send Failed!\n");
    }
    _status = _hi_rdma->PollQP(1);
    if (_status.ok()) {
        printf("[INFO] PollQP Send Success!\n");
    } else {
        printf("[INFO] PollQP Send Failed!\n");
    }
#endif
    return 0;
}