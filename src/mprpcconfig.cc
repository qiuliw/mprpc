#include "mprpcconfig.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>

// 去掉字符串末尾的换行符
void MpRpcConfig::RemoveTrailingNewline(std::string& str) {
    size_t pos = str.find_last_not_of('\n');
    if (pos != std::string::npos) {
        str.erase(pos + 1);
    }
}

// 去掉字符串前后的空格
void MpRpcConfig::Trim(std::string& src_buf) {
    int idx = src_buf.find_first_not_of(' ');
    if(idx == -1) {
        src_buf = src_buf.substr(idx, src_buf.size());
    }
    idx = src_buf.find_last_not_of(' ');
    if(idx == -1) {
        src_buf = src_buf.substr(0, idx);
    }
}

void MpRpcConfig::LoadConfigFile(const char* config_file)
{
    FILE *pf = fopen(config_file, "r");
    if(nullptr == pf){
        std::cout << config_file << " is not exist..." << std::endl;
        exit(EXIT_FAILURE);
    }

    char buf[512] = {0};
    // 1.注释 2. 正确的配置项  3. 去掉开头多余的空格
    while (fgets(buf,sizeof buf,pf)!=nullptr) {
        std::string src_buf(buf);
        // 先去除行尾换行符
        RemoveTrailingNewline(src_buf);
        Trim(src_buf);

        if(src_buf[0] == '#' || src_buf.empty()){
            continue;
        }

        // 解析配置项
        int idx = src_buf.find('=');
        if(idx == -1){
            // 配置项不合法
            continue;
        }

        std::string key = src_buf.substr(0,idx);
        Trim(key);

        std::string value = src_buf.substr(idx+1);
        Trim(value);

        m_config_map.insert({key,value});
    }
}

std::string MpRpcConfig::Load(const std::string& key)
{
    // [] 的副作用：如果key不存在，则插入一个pair。应当返回一个空字符串，不向其添加键值对
    auto it = m_config_map.find(key);
    if(it == m_config_map.end()){
        return "";
    }
    return it->second;
}