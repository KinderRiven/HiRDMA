/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-12 17:51:41
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/test/example/demo_1.cpp
 */

#include "hi_rdma.hpp"

int main(int argc, char** argv)
{
    hi_rdma::Options _options;
    hi_rdma::HiRDMA* _hi_rdma = nullptr;
    Status _status = hi_rdma::HiRDMA::CreateRDMAContext(_options, &_hi_rdma);
    if (_status.ok()) {
        printf("CreateRDMAContext Success!\n");
    } else {
        printf("CreateRDMAContext Failed!\n");
    }
    return 0;
}