## 使用的三方库

- Protobuf 提供数据交换格式(libprotoc 3.12.4)
- Muduo 底层网络库


## 框架结构

![alt text](assets/README/image.png)

![alt text](assets/README/image-1.png)


## 使用

1. protobuf 定义 service
2. callee 实现 class UserService : public fixbug::UserServiceRpc(要将proto自动生成的方法对接到本地方法) , Notify提交服务到rpc节点
3. caller fixbug::UserServiceRpc_Stub stub(new MpRpcChannel()); 连接节点，调用远程rpc方法，stub提供service签名





