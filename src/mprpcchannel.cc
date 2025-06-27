#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "mprpcchannel.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"



/*
header_size + service_name + method_name + args_size + args_data
*/

// 抽象接口类提供rpc服务的接口数据类型
// rpcchannel框架传入，框架实现序列化编码发送（CallMethod中）。
void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller,
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name(); // service_name
    std::string method_name = method->name(); // method_name

    // 获取参数的序列化字符串长度
    uint32_t args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str)){
        args_size = args_str.size();
    }else{
        std::cout << "serialize request error" << std::endl;
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name); // 序列化给框架使用,由框架使用的protobuf包头数据字段
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);
    // args_str 根据用户类型序列化

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpcHeader.SerializeToString(&rpc_header_str)){
        header_size = rpc_header_str.size();
        std::cout << "header_str:" << rpc_header_str << std::endl;
    }else{
        std::cout << "serialize header error" << std::endl;
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char*)&header_size,4)); // header_size
    send_rpc_str.insert(4,rpc_header_str); // header_str
    send_rpc_str.insert(4+header_size,args_str); // 框架模拟http短链接，所以不用考虑粘包问题

    // 打印调试信息
    std::cout << "==========================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str" << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "==========================" << std::endl;
    
    // 使用rpc编程，完成rpc方法的远程调用。
    // 客户端，不需要并发高性能，直接使用socket编程即可
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cout << "create socket error, errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // 读取配置文件rpcserver的信息
    std::string ip = MpRpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MpRpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());


    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点的muduo服务器
    if( -1 == connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ){
        std::cout << "connect error, errno: " << errno <<std::endl;
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    // 发送rpc请求
    if(-1 == send(client_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0)){
        std::cout << "send error, errno: " << errno <<std::endl;
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    // 接收rpc请求的响应
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if(-1 == (recv_size = recv(client_fd, recv_buf, sizeof(recv_buf), 0))){
        std::cout << "recv error, errno: " << errno <<std::endl;
        close(client_fd);
        exit(EXIT_FAILURE);
    }


    // 反序列化rpc调用的响应数据
    std::string response_str(recv_buf,recv_size);
    if(!response->ParseFromString(response_str)){
        std::cout << "parse response error, response_str" << response_str <<std::endl;
        close(client_fd);
        return;
    }

    close(client_fd);

}
