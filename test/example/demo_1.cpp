/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-17 11:52:18
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
        printf("CreateRDMAContext Success!\n");
        printf("%s\n", _status.ToString().c_str());
    } else {
        printf("CreateRDMAContext Failed!\n");
        printf("%s\n", _status.ToString().c_str());
    }

    hi_rdma::HiRDMABuffer* _rbuf = _hi_rdma->RegisterRDMABuffer(1024UL * 1024,
        IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);
    return 0;
}