//
// ZGCameraSource.h
//
// Copyright 2018�� Zego. All rights reserved.
//

#ifndef ZGCameraSource_h__
#define ZGCameraSource_h__

#include <string>
#include <mutex>
#include <vector>
#include <atomic>

#include "ZGExternalVideoSource.h"
#include "ZegoCamera.h"

using std::string;
using std::vector;

// ����ͷ������Ϊ�ⲿ�ɼ�Դ��ʾ��
class ZGCameraSource : public ZGExternalVideoSource
{
public:
    ZGCameraSource(string camera_id);
    ~ZGCameraSource();

    virtual void OnGetVideoData(std::shared_ptr<ZGExternalVideoData> & video_data) override;

private:

    // ����ͷ�ɼ������ݵĻص�����������뵽����
    void OnVideoData(const char* data,
        int length,
        const AVE::VideoCaptureFormat& frame_format,
        unsigned long long reference_time,
        unsigned int reference_time_scale);

    // ����ͷ����ص�
    void OnCameraError(const char *deviceId, const char *reason);

    // ����ͷ�ɼ�ģ��
    ZegoVideoCapExternal zego_camera_capture_;

    // ����ͷid
    string camera_id_;

    // ����ͷ���ݻ���
    vector<std::shared_ptr<ZGExternalVideoData> > capture_video_data_vec_;

    // ��������С���ã����λ���
    const int MAX_VIDEO_BUF_LEN = 2;

    // д����λ��
    std::atomic<int> write_index_ = {0};

    // ������λ��
    std::atomic<int> read_index_ = {0};

    // ����ͷ�ɼ��Ƿ���
    bool have_start_captured_ = false;
    // ��������ͷģ��Ĳɼ�֡��
    int capture_fps_ = 25;

    int mResWidth;
    int mResHeight;
};


#endif // ZGCameraSource_h__


