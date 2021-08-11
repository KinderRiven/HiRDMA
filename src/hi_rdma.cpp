/*
 * @Author: your name
 * @Date: 2021-08-11 15:44:55
 * @LastEditTime: 2021-08-11 16:38:00
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/src/hi_rdma.cpp
 */

#include "hi_rdma.hpp"

using namespace hi_rdma;

Status HiRDMA::CreateRDMAContext(Options& options, HiRDMA** context)
{
    find_then_open_device();
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

bool HiRDMA::find_then_open_device()
{
    int _res;
    int _num_dev = 0;
    struct ibv_device* _dev = nullptr;
    struct ibv_context* _dev_ctx = nullptr;
    struct ibv_device** _dev_list = nullptr;

    _dev_list = ibv_get_device_list(&_num_dev);
    if (_num_dev == 0) {
        return false;
    }

    for (int i = 0; i < _num_dev; i++) {
        _dev = _dev_list[i];
        _dev_ctx = ibv_open_device(_dev);
        if (_dev_ctx != NULL) {
            printf("[%s][%s]\n", _dev_ctx->name, _dev_ctx->dev_name);
        }
    }
    return true;
}

bool HiRDMA::create_qp()
{
}