set(SRC_LIST userservice.cc ../user.pb.cc)

add_executable(provider ${SRC_LIST}) # 将源码添加到可执行文件（还需要链接第三方库）

# 链接第三方库
target_link_libraries(provider mprpc protobuf)