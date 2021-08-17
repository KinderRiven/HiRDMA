/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-17 13:23:24
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/test/example/demo_1.cpp
 */

#include "hi_rdma.hpp"

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
    return 0;
}