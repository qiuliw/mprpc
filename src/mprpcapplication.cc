#include "mprpcapplication.h"


void MpRpcApplication::Init(int argc, char **argv){

}

MpRpcApplication& MpRpcApplication::GetInstance()
{
    static MpRpcApplication app;
    return app;
}

MpRpcApplication::MpRpcApplication()
{
    
}
