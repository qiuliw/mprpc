# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)

add_executable(consumer ${SRC_LIST}) # 将源码添加到可执行文件（还需要链接第三方库）

# 链接第三方库：依赖优先，优先编译mprpc库
target_link_libraries(consumer mprpc protobuf)