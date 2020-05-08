//
// AppIDHelper.h
//
// Copyright 2018 Zego. All rights reserved.
// 
#ifndef AppIDHelper_h__
#define AppIDHelper_h__
#include <string>
#include <vector>
//#include "AppSupport/ZGUtilTools.h"
using std::string;
using std::vector;
// appid
unsigned int GetAppId()
{
    // 向zego申请获取，然后把appid和sign存放于自己的服务器上，运行时通过网络请求动态获取，防止泄露
    return 999299184;
}
// app sign
unsigned char * GetAppSign(int & len)
{
    static unsigned char appSignature[] = {
        0xb4, 0x29, 0x07, 0xb9, 0x1e, 0xc7, 0x8e, 0xf7,
        0x05, 0x02, 0x1e, 0xc2, 0xc1, 0xb3, 0x84, 0xbb,
        0x21, 0xf1, 0xea, 0x34, 0xba, 0xf2, 0x6f, 0xbb,
        0x43, 0xc3, 0x53, 0xeb, 0x0f, 0x0d, 0xae, 0xfc
    };
    len = 32;
    return appSignature;
}
#endif // AppIDHelper_h__
