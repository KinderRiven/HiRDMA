/*
 * @Author: your name
 * @Date: 2021-08-11 15:28:44
 * @LastEditTime: 2021-08-11 15:37:50
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /HiRDMA/include/options.hpp
 */

#ifndef INCLUDE_INCLUDE_OPTIONS_HPP_
#define INCLUDE_INCLUDE_OPTIONS_HPP_

#include "header.hpp"

namespace hi_rdma {

class Options {
public:
    Options()
        : dev_name("mlx5_0")
        , dev_port(1)
        , dev_idx(2)
    {
    }

public:
    std::string dev_name; // mlx5_0

    int dev_port;

    int dev_idx;
};
};

#endif