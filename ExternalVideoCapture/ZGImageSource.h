//
// ZGImageSource.h
//
// Copyright 2018�� Zego. All rights reserved.
// 

#ifndef ZGImageSource_h__
#define ZGImageSource_h__

#include <string>
#include <mutex>

#include "ZGExternalVideoSource.h"

using std::string;

// �ⲿ�ɼ�ͼƬԴʾ��
class ZGImageSource : public ZGExternalVideoSource
{
public:
    ZGImageSource(string img_path);
    ~ZGImageSource();

    virtual void OnGetVideoData(std::shared_ptr<ZGExternalVideoData> & video_data) override;

private:
    // bmpͼ��·��
    string image_path_;
    // ͼ������buf
    unsigned char * img_buf_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    std::mutex buf_mutex_;
    int pixel_byte_count_ = 3;//  RGBͼ������Դһ�����ؼ����ֽڣ����磺����Դ RGB ʱΪ3�� ����ԴΪRGBAʱΪ4
};


#endif // ZGImageSource_h__

