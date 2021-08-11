/*
 * @Author: your name
 * @Date: 2021-08-11 16:43:34
 * @LastEditTime: 2021-08-11 16:44:24
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/test/example/demo_1.cpp
 */

#include "hi_rdma.hpp"

int main(int argc, char** argv)
{
    hi_rdma::HiRDMA* _hi_rdma = nullptr;
    hi_rdma::HiRDMA::CreateRDMAContext(_hi_rdma);
    return 0;
}