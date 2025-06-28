#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h" // 使用pb的数据格式
#include "mprpcchannel.h"

int main(int argc, char** argv){
    // 整个程序启动后，想使用mprpc框架来享受rpc服务，必须先调用框架的初始化函数（只初始化一次）。启动muduo，序列化功能等
    MpRpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MpRpcChannel());
    // stub.Login(); // RpcChannel->CallMethod 集中起来做所有rpc方法调用的参数序列化和网络发送

    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123456");
    // rpc方法的响应
    fixbug::LoginResponse response;
    // 调用rpc方法，同步的rpc调用过程，MpRpcChannel->Method
    stub.Login(nullptr, &request, &response, nullptr);

    // rpc调用完成，读调用的结果
    if(response.result().errcode() == 0){
        std::cout << "rpc login response success：" << response.success() << std::endl;
    }else{
        std::cout << "rpc login response error：" << response.result().errmsg() << std::endl;
    }


    // 测试 Register 方法
    fixbug::RegisterRequest rRequest;
    rRequest.set_id(2000); // 类型属性未初始化会报错
    rRequest.set_name("wangwu");
    rRequest.set_pwd("654321");
    fixbug::RegisterResponse rResponse;
    stub.Register(nullptr, &rRequest, &rResponse, nullptr);

    if(rResponse.result().errcode() == 0){
        std::cout << "rpc register response success：" << rResponse.success() << std::endl;
    }else{
        std::cout << "rpc register response error：" << rResponse.result().errmsg() << std::endl;
    }

    return 0;
}