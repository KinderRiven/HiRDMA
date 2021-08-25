/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-25 15:50:40
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/test/example/demo_1.cpp
 */
#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <infiniband/verbs.h>
#include <iostream>
#include <malloc.h>
#include <map>
#include <mutex>
#include <netdb.h>
#include <pthread.h>
#include <queue>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "config.hpp"
#include "hi_rdma.hpp"
#include "sock.hpp"

static void run_client_thread(int thread_id)
{
    hi_rdma::Options _options;
    hi_rdma::HiRDMA* _hi_rdma = nullptr;
    hi_rdma::HiRDMABuffer _remote_buf;
    hi_rdma::HiRDMABuffer* _local_buf = nullptr;

    hi_rdma::Status _status = hi_rdma::HiRDMA::CreateRDMAContext(_options, &_hi_rdma);
    if (_status.ok()) {
        printf("[%d][INFO] CreateRDMAContext Success!\n", thread_id);
    } else {
        printf("[%d][INFO] CreateRDMAContext Failed! [%s]\n", , thread_id, _status.ToString().c_str());
    }

    _local_buf = _hi_rdma->RegisterRDMABuffer(1048576UL, hi_rdma::LOCAL_WR | hi_rdma::REMOTE_RD | hi_rdma::REMOTE_WR | hi_rdma::REMOTE_ATOMIC);
    if (_local_buf != nullptr) {
        printf("[%d][INFO] RegisterRDMABuffer Success!\n", thread_id);
        _local_buf->Print();
    } else {
        printf("[%d][INFO] RegisterRDMABuffer Failed!\n", thread_id);
    }

    int sock_fd = hi_rdma::Socket::Connect(server_ip, server_port);
    if (sock_fd != -1) {
        printf("[%d] Connect = %d\n", thread_id, sock_fd);
        hi_rdma::HiRDMAQPInfo _remote_qp;
        hi_rdma::HiRDMAQPInfo* _local_qp = _hi_rdma->AcquireQPInfo();
        hi_rdma::Socket::Write(sock_fd, (char*)_local_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        hi_rdma::Socket::Read(sock_fd, (char*)&_remote_qp, sizeof(hi_rdma::HiRDMAQPInfo));
        _local_qp->Print();
        _remote_qp.Print();
        _status = _hi_rdma->ConnectQP(_local_qp, &_remote_qp);
        if (_status.ok()) {
            printf("[%d][INFO] Connect QP Success!\n");
        } else {
            printf("[%d][INFO] Connect QP Failed!\n");
        }
        hi_rdma::Socket::Read(sock_fd, (char*)&_remote_buf, sizeof(hi_rdma::HiRDMABuffer));
        _remote_buf.Print();
    }
}

int main(int argc, char** argv)
{
    std::thread _client[64];
    for (auto i = 0; i < kNumClient; i++) {
        _client[i] = std::thread(run_client_thread, i);
    }
    for (auto i = 0; i < kNumClient; i++) {
        _client[i].join();
    }
    return 0;
}