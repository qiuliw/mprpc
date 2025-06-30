#pragma once

#include "semaphore.h"
#include <string>
#include <zookeeper/zookeeper.h>

// zk客户端封装类
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    void Start(); // 启动zkclient连接zkserver
    /*
        根据path创建znode节点,
        status: 永久性节点还是临时节点
    */
    void Create(const char* path, const char* data, int datalen, int status = 0);
    // 根据path获取节点数据 max=1MB
    std::string GetData(const char* path);
private:
    // zk的server句柄
    zhandle_t* m_zhandle;
};