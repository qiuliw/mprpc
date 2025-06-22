#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main(){
    LoginResponse rsp;
    // 对于嵌套类型，使用mutable_<field_name>() 获取可变对象指针，创建在堆区，由栈上父对象管理
    ResultCode *rc = rsp.mutable_result(); 
    rc->set_errcode(1);
    
    // 重复类型 add_<list_name>()
    GetFriendListsResponse flr;
    User *user = flr.add_friend_list();
}

int main1() { 

    // 封装了 login 请求对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");
    // 对象数据序列化
    std:: string send_str;
    if(req.SerializeToString(&send_str))
    {
        std::cout << send_str << std::endl;
    }
    // 从send_str字符串反序列化对象数据
    LoginRequest req2;
    if(req2.ParseFromString(send_str)){
        std::cout << req2.name() << std::endl;
        std::cout << req2.pwd() << std::endl;
    }

}