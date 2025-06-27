#include "mprpcapplication.h"
#include "mprpcconfig.h"
#include <iostream>
#include <unistd.h>

MpRpcConfig MpRpcApplication::m_config;

void ShowArgsHelp(){
    std::cout << "format: command -i <config_file>" << std::endl;
}

void MpRpcApplication::Init(int argc, char **argv){
    if(argc < 2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    // 解析命令行参数 "abc:" -a；-b；-c 参数
    while ((c = getopt(argc, argv, "i:"))!=-1 ) {
        switch (c) {
            case 'i':
                config_file = optarg; // optarg 指向参数串
                break;
            case '?': // '?'：处理无效选项（未在"i:"中定义的选项）。
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            case ':': // ':'：处理缺少参数的情况（如-i后未跟配置文件名）。
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    // 开始加载配置文件 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "rpcserver_ip: " << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "rpcserver_port: " << m_config.Load("rpcserverport") << std::endl;
    // std::cout << "zookeeper_ip: " << m_config.Load("zookeeperip") << std::endl;
    // std::cout << "zookeeper_port: " << m_config.Load("zookeeperport") << std::endl;
}

MpRpcApplication& MpRpcApplication::GetInstance()
{
    static MpRpcApplication app;
    return app;
}

MpRpcConfig& MpRpcApplication::GetConfig()
{
    return m_config;
}

MpRpcApplication::MpRpcApplication()
{
    
}
