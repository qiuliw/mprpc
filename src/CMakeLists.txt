# 局限性: CMakeList.txt没改变时，不会重新搜索源文件更新makefile，造成源文件未添加到动态库
aux_source_directory(. SRC_LIST) # 获取当前目录下所有源文件，生成makefile的SRC_LIST变量

# 改进，手动添加源文件
# set(SRC_LIST mprpcapplication.cc mprpcconfig.cc)
# 或者，直接重新生成makefile
# make clean

add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc muduo_net muduo_base pthread)