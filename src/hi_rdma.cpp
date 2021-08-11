/*
 * @Author: your name
 * @Date: 2021-08-11 15:44:55
 * @LastEditTime: 2021-08-11 16:42:33
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/src/hi_rdma.cpp
 */

#include "hi_rdma.hpp"

using namespace hi_rdma;

Status HiRDMA::CreateRDMAContext(Options& options, HiRDMA** context)
{
    int _res;
    int _num_dev = 0;
    struct ibv_device* _dev = nullptr;
    struct ibv_context* _dev_ctx = nullptr;
    struct ibv_device** _dev_list = nullptr;

    _dev_list = ibv_get_device_list(&_num_dev);
    if (_num_dev == 0) {
    }

    for (int i = 0; i < _num_dev; i++) {
        _dev = _dev_list[i];
        printf("[%s][%s]\n", _dev->name, _dev->dev_name);
    }
}

HiRDMABuffer* HiRDMA::RegisterRDMABuffer(size_t size, int access_mode)
{
}

Status HiRDMA::ExchangeQPInfo()
{
}

Status HiRDMA::ConnectQP()
{
}

Status HiRDMA::PollQP()
{
}

Status HiRDMA::Write(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size)
{
}

Status HiRDMA::Read(HiRDMABuffer* rbuf, uint64_t offset, char* buf, size_t size)
{
}