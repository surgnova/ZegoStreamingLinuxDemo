//
// ZGExternalVideoSource.h
//
// Copyright 2018�� Zego. All rights reserved.
// 

#ifndef ZGExternalVideoSource_h__
#define ZGExternalVideoSource_h__

#include <stdint.h>
#include <memory>

#include "video_capture.h"

using namespace AVE;

// �ⲿ�ɼ�����Ƶ���ݽṹ
typedef struct ZGExternalVideoData
{
    // std::unique_ptr<uint8_t[]> data(new uint8_t[10]);
    // this will correctly call delete[]
    std::unique_ptr<uint8_t[] > data;      // �ⲿ�ɼ���video data���飬����new 10�����ȵ����飬std::unique_ptr<uint8_t[]> data(new uint8_t[10])��ָ���׵�ַdata.get()
    uint32_t len = 0;                      // ���ݳ���
    VideoCaptureFormat fomat;              // ��ʽ RGBA ����BGRA��
    unsigned long long reference_time = 0; // ʱ���

}ZGExternalVideoData;

// �ⲿ��ƵԴ�ӿڣ������ⲿ�ɼ�Դ�Ļ���
class ZGExternalVideoSource
{
public:
    ZGExternalVideoSource();
    ~ZGExternalVideoSource();
    // �ⲿ�ɼ���ȡ���ݵĽӿ�
    virtual void OnGetVideoData(std::shared_ptr<ZGExternalVideoData> & video_data) = 0;
};

#endif // ZGExternalVideoSource_h__


