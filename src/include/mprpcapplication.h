#pragma once

// mprpc框架的基础类 单例
class MpRpcApplication {
public:
    static void Init(int argc, char** argv);
    static MpRpcApplication& GetInstance();
private:
    MpRpcApplication();
    MpRpcApplication(const MpRpcApplication&) = delete;
    MpRpcApplication(const MpRpcApplication&&) = delete;
};



