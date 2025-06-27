#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "mprpcconfig.h"
#include "rpcheader.pb.h"
#include <cstdint>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <iostream>
#include <muduo/net/Callbacks.h>
#include <string>
#include <sys/types.h>

/*
service_name => service描述 
                    => service* 记录服务对象
                    => method_name => method方法对象

json protobuf 
protobuf字段是预先确定的，而json字段是动态的，需要内存存储字段信息，信息密度低
*/

// 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(::google::protobuf::Service* service)
{

    ServiceInfo serviceInfo; // 服务对象描述信息

    // 获取service服务对象描述
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    std::string serviceName = pserviceDesc->name();
    // 获取服务对象service的方法数量
    int methodCnt = pserviceDesc->method_count();
    for (int i = 0; i < methodCnt; ++i)
    {
        // 获取了服务对象指定下标的服务方法的描述（抽象描述）
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string methodName = pmethodDesc->name();

        serviceInfo.m_methodMap.insert({methodName, pmethodDesc});
    }
    serviceInfo.m_service = service;
    m_serviceMap.insert({serviceName, serviceInfo});
}

// 启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    std::string ip = MpRpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = std::atoi(MpRpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);

    // 创建TCPServer
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    
    // 绑定新连接回调  分离了网络代码和业务代码
    server.setConnectionCallback([this](const muduo::net::TcpConnectionPtr& conn) {
        this->OnConnection(conn);
    });
    // 绑定读写回调方法
    server.setMessageCallback([this](const muduo::net::TcpConnectionPtr& conn,
                                     muduo::net::Buffer* buffer,
                                     muduo::Timestamp time) {
        this->OnMessage(conn, buffer, time);
    });


    // 设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider start service at ip:" << ip << " port:" << port << std::endl;

    // 启动网络服务
    server.start();
    m_eventLoop.loop();

}


// 新连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected()){
        // rpc client has disconnected
        conn->shutdown();
    }

}

/*
在框架内部，RpcComSumer和RpcProvider之间协商好数据的格式，就是protobuf数据类型
service_name method_name args args_size 定义proto的message类型格式，进行数据头的序列化和反序列化
header_size(4字节) + header_str + args_str   粘包可能存在，所以需要解决粘包问题
std::string insert和copy方法

数据长度不够 | 粘包
*/

// 读写事件回调。rpc远程服务的调用请求从此处开始处理
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buffer,
                   muduo::Timestamp)
{

    /*
        protobuf反序列化前，确定包完整性，预读取首字段4字节规定包长度
        protobuf 预设字段确定各数据部分长度，可变长度字段使用特殊结束符
        得到完整protobuf包后，可调用 messge的ParseFromString解析相应消息类型的包
    */

    // 网络上接受的远程rpc调用请求的字符流
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, sizeof(header_size), 0);

    // 根据header_size，从字符流中读取数据包
    std::string rpc_header_str = recv_buf.substr(sizeof(header_size), header_size); 

    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else{
        std::cout << "parse rpc header error!" << std::endl;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4+header_size, args_size);

    // 打印调试信息
    std::cout << "===========================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;

    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===========================" << std::endl;

    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end()){
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }else{
        std::cout << "service_name: " << service_name << std::endl;
    }
    ServiceInfo serviceInfo = it->second;

    auto mit = serviceInfo.m_methodMap.find(method_name);
    if(mit == serviceInfo.m_methodMap.end()){ 
        std::cout <<  service_name<< ":" << method_name << " is not exist!" << std::endl;
        return;
    }
    
    google::protobuf::Service* service = serviceInfo.m_service; // 获取service对象
    const google::protobuf::MethodDescriptor* method = mit->second; // 获取method对象

    // 生成rpc方法调用请求request和响应response参数
    // 根据service和method生成request和response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New(); // 根据服务的方法获取参数request的类型，方法的参数类型被封装在reqest中
    if(!request->ParseFromString(args_str)){ // 解析实参到request中
        std::cout << "request parse args error: " << args_str << std::endl;
        return;
    }
    // 获取protobuf中定义的service方法的响应类型
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    
    // 给method方法的调用，绑定一个Closure的回调函数
    google::protobuf::Closure *done = 
        google::protobuf::NewCallback<RpcProvider, // 调用类的类型
                                        // 方法类型，略
                                        const muduo::net::TcpConnectionPtr&, // 参数类型
                                        google::protobuf::Message*
                                        >
                                        (this,
                                        &RpcProvider::SendRpcResponse,
                                        conn,
                                        response);

    // 在框架上根据远端rpc请求，调用rpc节点上发布的方法。参数封装到request中，传给业务逻辑层。
    // done封装rpc框架回调逻辑，负责返回response给rpc调用方
    service->CallMethod(method, nullptr, request, response, done);


}

// Closure 的回调操作,用于序列化rpc的响应与网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str)){ // response序列化
        // 序列化成功后，通过网络把rpc方法执行的结果发送回rpc的调用方
        conn->send(response_str);
    }else{
        std::cout << "serialize response_str error!" << std::endl;
    }

    conn->shutdown(); // 模拟http的短链接服务，由服务端主动断开连接
}
