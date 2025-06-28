#include <iostream>
#include "logger.h"
#include "mprpcapplication.h"
#include "friend.pb.h" // 使用pb的数据格式
#include "mprpccontroller.h"

int main(int argc, char** argv)
{

    // 整个程序启动后，想使用mprpc框架来享受rpc服务，必须先调用框架的初始化函数（只初始化一次）。启动muduo，序列化功能等
    MpRpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::FriendServiceRpc_Stub stub(new MpRpcChannel());
    // stub.Login(); // RpcChannel->CallMethod 集中起来做所有rpc方法调用的参数序列化和网络发送

    // rpc方法的请求参数
    fixbug::GetFriendListRequest request;
    request.set_userid(1000);
    // rpc方法的响应
    fixbug::GetFriendListResponse response;

    // 调用rpc方法，同步的rpc调用过程，MpRpcChannel->Method
    MpRpcController controller; // 控制信息，客户端本地框架内发生的错误进行描述。
    stub.GetFriendList(&controller, &request, &response, nullptr);

    // rpc调用完成，读调用的结果
    if(controller.Failed()){
        std::cout << controller.ErrorText() << std::endl;
    }else{
        if (response.result().errcode() == 0)
        {
            // 遍历response.friends()
            for (int i = 0; i < response.friends_size(); i++)
            {
                std::cout << "friendname:" << response.friends(i) << std::endl;
            }
        }else{
            std::cout << response.result().errmsg() << std::endl;
        }
    }

}