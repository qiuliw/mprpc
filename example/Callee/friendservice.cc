#include "friend.pb.h"
#include "logger.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include <iostream>
#include <vector>

class FriendService : public fixbug::FriendServiceRpc {
public:
    
    std::vector<std::string> GetFriendList(uint32_t userid)
    {
        std::cout << "do GetFriendList service! userid: " << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("张三");
        vec.push_back("李四");
        vec.push_back("王五");
        return vec;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                      const ::fixbug::GetFriendListRequest* request,
                      ::fixbug::GetFriendListResponse* response,
                      ::google::protobuf::Closure* done) override
    {
        uint32_t userid = request->userid();
        std::vector<std::string> friendList = GetFriendList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for(std::string& name : friendList){
            std::string* p = response->add_friends(); // protobuf repeat底层使用指针数组记录每个对象
            *p = name;
        }
        done->Run();
    }
};

int main(int argc, char** argv)
{   
    LOG_INFO("first log message");
    Log_ERROR("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);

    MpRpcApplication::Init(argc, argv);
    RpcProvider provider; 
    provider.NotifyService(new FriendService());
    provider.Run();
    return 0;
}
