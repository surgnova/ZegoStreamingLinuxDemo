
//
// ZGScreenSource.h
//
// Copyright 2018�� Zego. All rights reserved.
//

#ifndef ZGScreenSource_h__
#define ZGScreenSource_h__

#include <vector>
#include <atomic>

#include "ZGExternalVideoSource.h"

#include "Topics/ExternalVideoCapture/ScreenCapture/include/zego-screencapture.h"

using std::vector;

class ZGScreenSource : public ZGExternalVideoSource
{
public:
    ZGScreenSource();
    ~ZGScreenSource();

    virtual void OnGetVideoData(std::shared_ptr<ZGExternalVideoData> & video_data) override;

private:

    static void OnScreenCaptureData(const char *data, uint32_t length, const struct ZegoScreenCaptureVideoCaptureFormat *video_frame_format, uint64_t reference_time, uint32_t reference_time_scale, void *user_data);

    static void OnScreenCaptureError(enum ZegoScreenCaptureCaptureError error, void *data);

    // ������Ļ�ɼ�֡��
    int screen_capture_fps_ = 10;

    // ʹ���������
    bool scale_ = true;

    // ������Ļ�ɼ�����ֱ��ʿ�
    int width_ = 1920;

    // ������Ļ�ɼ�����ֱ��ʸ�
    int height_ = 1080;

    // ���ݻ���
    vector<std::shared_ptr<ZGExternalVideoData> > capture_video_data_vec_;

    // ��������С���ã����λ���
    const int MAX_VIDEO_BUF_LEN = 2;

    // д����λ��
    std::atomic<int> write_index_ = 0;

    // ������λ��
    std::atomic<int> read_index_ = 0;

    // �ɼ��Ƿ���
    bool have_start_captured_ = false;
};


#endif // ZGScreenSource_h__

