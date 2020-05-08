#include "ZGCameraSource.h"

#include "ZGLog.h"

#include <string.h>

using namespace std::placeholders;
ZGCameraSource::ZGCameraSource(string camera_id):
    camera_id_(camera_id)
{
    // ���û����С
    capture_video_data_vec_.resize(MAX_VIDEO_BUF_LEN);

    // ��������ͷģ�����ݻص�
    zego_camera_capture_.SetVideoFrameCallBack(std::bind(&ZGCameraSource::OnVideoData, this, _1, _2, _3, _4, _5));
    // ��������ͷģ�����ص�
    zego_camera_capture_.SetCaptureDeviceErrorCallBack(std::bind(&ZGCameraSource::OnCameraError, this, _1, _2));
    mResWidth = 176;
    mResHeight = 144;
}

ZGCameraSource::~ZGCameraSource()
{
    // ֹͣ����ͷģ��ɼ�
    zego_camera_capture_.StopCapture();
}

void ZGCameraSource::OnGetVideoData(std::shared_ptr<ZGExternalVideoData> & video_data)
{
    if (!have_start_captured_)
    {
        have_start_captured_ = true;

        // ��������ͷid
        zego_camera_capture_.SetCamera(camera_id_);

        // ��������ͷģ��ķֱ���
        zego_camera_capture_.SetCameraResolution(mResWidth, mResHeight);

        // ��������ͷģ���֡��
        zego_camera_capture_.SetFrameRate(capture_fps_);

        // ��ʼ�ɼ�
        zego_camera_capture_.StartCapture();

        video_data = nullptr;

        return;
    }

    if (read_index_ != write_index_)
    {
        // �Ӳɼ���ʱ��������ȡ���ݸ��ⲿ�ɼ�
        video_data = capture_video_data_vec_[read_index_];
        read_index_ = (read_index_ + 1) % MAX_VIDEO_BUF_LEN;
    }
}

// ����ͷ�ɼ������ݵĻص�����������뵽���壬���ⲿ�ɼ�������
void ZGCameraSource::OnVideoData(const char* data, int length, const AVE::VideoCaptureFormat& frame_format, unsigned long long reference_time, unsigned int reference_time_scale)
{
    //ZGENTER_FUN_LOG;
    std::shared_ptr<ZGExternalVideoData> external_data = std::make_shared<ZGExternalVideoData>();

    // ��������
    external_data->data = std::unique_ptr<uint8_t[] >(new uint8_t[length]);

    // ��������ͷ�ɼ�������
    memcpy(external_data->data.get(), data, length);

    // ���ø�ʽ
    external_data->fomat = frame_format;

    // ���ݳ���
    external_data->len = length;

    // ʱ���
    external_data->reference_time = reference_time;

    // ���浽��ʱ����
    capture_video_data_vec_[write_index_] = external_data;

    // д��������
    write_index_ = (write_index_ + 1) % MAX_VIDEO_BUF_LEN;
    if (write_index_ == read_index_)
    {
        read_index_ = (read_index_ + 1) % MAX_VIDEO_BUF_LEN;
    }
}

void ZGCameraSource::OnCameraError(const char *deviceId, const char *reason)
{
    ZGLog("OnCameraError %s , %s", deviceId, reason);
}
