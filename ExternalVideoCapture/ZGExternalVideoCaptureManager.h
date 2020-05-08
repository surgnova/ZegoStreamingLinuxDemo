//
// ZGExternalVideoCaptureManager.h
//
// Copyright 2018�� Zego. All rights reserved.
//

#ifndef ZGExternalVideoCaptureManager_h__
#define ZGExternalVideoCaptureManager_h__

#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>


#include "ZGExternalVideoSource.h"
#include "ZGExternalVideoCaptureDemo.h"
#include "ZGExternalVideoCatpureDemoHelper.h"

using std::string;

enum ZGExternalVideoSourceType
{
    ZGExternalVideoSource_Image,
    ZGExternalVideoSource_Camera,
    ZGExternalVideoSource_Screen
};

typedef struct ZGExternalVideoSourceParams
{
    string image_path; // for ZGExternalVideoSource_Image type
    string camera_id;  // fro ZGExternalVideoSource_Camera

}ZGExternalVideoSourceParams;

typedef std::function< void(std::shared_ptr<ZGExternalVideoData>)> VideoCallBackType;

class ZGExternalVideoCaptureManager
{
public:
    ZGExternalVideoCaptureManager();
    ~ZGExternalVideoCaptureManager();

    // ������Ƶ���ݻص����ɼ��������ݵĻص����ڻص��п���������Ⱦ�������ⲿ�ɼ���Ҫ�Լ���Ⱦ
    void SetVideoDataCallBack(VideoCallBackType cb);

    // ��ʼ���ⲿ�ɼ�������
    void InitExternalCapture();

    // ������ɼ�Դ����
    void SetExternalSourceParams(ZGExternalVideoSourceParams params);

    // �����ⲿ�ɼ�Դ
    void CreateExternalSource(ZGExternalVideoSourceType source_type);

    // ���òɼ�fps
    void SetCaptureFps(int fps);

    // ��ʼ�ⲿ�ɼ��߳�
    void StartExternalCaptureThread();

    // ֹͣ�ⲿ�ɼ��߳�
    void StopExternalCaptureThread();

private:

    // �ⲿ�ɼ��̺߳���
    void CaptureVideoDataThread();

    time_t GetTimeStamp();

    std::function< void(std::shared_ptr<ZGExternalVideoData>) > video_data_cb_;

    // ��ȡ�ⲿ�ɼ�Դ���ݵĽӿ�
    std::shared_ptr<ZGExternalVideoSource> external_source_;

    std::mutex external_source_mutex_;

    // �ⲿ�ɼ�Դ����
    ZGExternalVideoSourceParams source_params_;

    ZGExternalVideoCaptureDemo demo_;

    // �ⲿ�ɼ�֡�����úͿ���
    int capture_fps_ = 25;

    // �ⲿ�ɼ��߳�
    std::thread capture_thread_;

    // �ⲿ�Ƿ�ֹͣ
    std::atomic<bool> stoped_ = {true};
};


#endif // ZGExternalVideoCaptureManager_h__

