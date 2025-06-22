aux_source_directory(. SRC_LIST)

add_library(mprpc SHARED ${SRC_LIST}) # 生成动态库