# set(SRC_LIST userservice.cc ../user.pb.cc)
set(SRC_LIST friendservice.cc ../friend.pb.cc)

add_executable(provider ${SRC_LIST}) # 将源码添加到可执行文件（还需要链接第三方库）

# 链接第三方库：依赖优先，优先编译mprpc库
target_link_libraries(provider mprpc protobuf pthread)