#pragma once
#include "mprpcconfig.h"

// mprpc框架的基础类 单例
class MpRpcApplication {
public:
    static void Init(int argc, char** argv);
    static MpRpcApplication& GetInstance();
    static MpRpcConfig& GetConfig();
private:
    MpRpcApplication();
    MpRpcApplication(const MpRpcApplication&) = delete;
    MpRpcApplication(const MpRpcApplication&&) = delete;

    static MpRpcConfig m_config;
};



