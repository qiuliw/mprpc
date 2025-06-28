## 使用的三方库

- Protobuf 提供数据序列化(libprotoc 3.12.4)
- Muduo 底层网络库


## 框架结构

![alt text](assets/README/image.png)

![alt text](assets/README/image-1.png)

对于客户端，方法首先暴露给用户调用，之后进行封装发送（MpRpcChannel）
    RpcController控制信息，对rpc框架内请求过程中出现的错误进行封装，暴露给用户进行错误处理
对于服务端，方法签名解析完成后(RpcProvider)，调用本地方法

![alt text](assets/README/image-2.png)

使用条件变量进行线程间通信。多生产单消费模型。日志业务的极低概率的虚假唤醒是可以忍受的
也可以使用`kafka`分布式日志存储

## 使用

1. protobuf 定义 service
2. callee 实现 class UserService : public fixbug::UserServiceRpc(要将proto自动生成的方法对接到本地方法) , Notify提交服务到rpc节点
3. caller fixbug::UserServiceRpc_Stub stub(new MpRpcChannel()); 连接节点，调用远程rpc方法，stub提供service签名




