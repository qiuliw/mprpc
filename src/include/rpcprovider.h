#pragma once

#include "user.pb.h"

// 框架提供的专门用于rpc的rpc服务发布的网络对象类
class RpcProvider {
public:
    // 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(::google::protobuf::Service* service);
    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
};