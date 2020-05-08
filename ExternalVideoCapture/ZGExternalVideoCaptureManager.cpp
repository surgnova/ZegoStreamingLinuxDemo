#include "ZGExternalVideoCaptureManager.h"

#include <chrono>

#include "LiveRoom-Publisher.h"

#include "ZGLog.h"
#include "ZGTimer.h"

#include "ZGImageSource.h"
#include "ZGCameraSource.h"
//#include "ZGScreenSource.h"

ZGExternalVideoCaptureManager::ZGExternalVideoCaptureManager()
{
}

ZGExternalVideoCaptureManager::~ZGExternalVideoCaptureManager()
{
}

void ZGExternalVideoCaptureManager::SetVideoDataCallBack(VideoCallBackType cb)
{
    video_data_cb_ = cb;
}

void ZGExternalVideoCaptureManager::SetExternalSourceParams(ZGExternalVideoSourceParams params)
{
    source_params_ = params;
}

void ZGExternalVideoCaptureManager::CreateExternalSource(ZGExternalVideoSourceType source_type)
{
    std::lock_guard<std::mutex> lock(external_source_mutex_);
    switch (source_type)
    {
        // ͼƬԴ
    case ZGExternalVideoSource_Image:
        external_source_ = std::make_shared<ZGImageSource>(source_params_.image_path);
    	break;
    /*
    case ZGExternalVideoSource_Screen:
        external_source_ = std::make_shared<ZGScreenSource>();
        break;
    */
        // ����ͷԴ
    case ZGExternalVideoSource_Camera:
        external_source_ = std::make_shared<ZGCameraSource>(source_params_.camera_id);
        break;
    default:
        break;
    }
}

void ZGExternalVideoCaptureManager::SetCaptureFps(int fps)
{
    capture_fps_ = fps;
}

void ZGExternalVideoCaptureManager::InitExternalCapture()
{
    // �����ⲿ�ɼ�
    demo_.EnableExternalVideoCapture();
}

void ZGExternalVideoCaptureManager::StartExternalCaptureThread()
{
    if (stoped_)
    {
        stoped_ = false;
        // �����������ⲿ�ɼ��߳�
        capture_thread_ = std::thread(std::bind(&ZGExternalVideoCaptureManager::CaptureVideoDataThread, this));
    }
}

void ZGExternalVideoCaptureManager::StopExternalCaptureThread()
{
    // ֹͣ�ɼ��߳�
    if (!stoped_)
    {
        stoped_ = true;
        capture_thread_.join();
    }

}

void ZGExternalVideoCaptureManager::CaptureVideoDataThread()
{
    ZGENTER_FUN_LOG;
    // ����sdk��֡��fps���ⲿ�ɼ���֡��һ��
    bool set_ret = LIVEROOM::SetVideoFPS(capture_fps_);
    if (set_ret)
    {
        ZGLog("set publish fps = %d ", capture_fps_);
    }
    else {
        ZGLog("set publish fps failed");
    }

    // �ⲿ�ɼ���loop���ɼ��������Ƹ�sdk�����ص����һ������Ⱦ
    while (true)
    {
        ZGTimer timer;
        if (stoped_)
        {
            break;
        }

        {
            std::lock_guard<std::mutex> lock(external_source_mutex_);
            if (external_source_)
            {
                std::shared_ptr<ZGExternalVideoData> video_data;
                // ��ȡ�ⲿ�ɼ�������
                external_source_->OnGetVideoData(video_data);
                if (video_data != nullptr)
                {
                    unsigned long long reference_time = video_data->reference_time == 0 ? GetTimeStamp() : video_data->reference_time;
                    // �Ƹ�sdk
                    demo_.PushExternalVideoData((const char *)video_data->data.get(), video_data->len, video_data->fomat, reference_time);
                    // �ص���Ⱦ
                    if (video_data_cb_)
                    {
                        // for render
                        video_data_cb_(video_data);
                    }
                }
            }

        }

        // sleep ����֡��
        int need_sleep_ms = 1000.0f / capture_fps_ - timer.ElapsedMs();
        std::this_thread::sleep_for(std::chrono::milliseconds(need_sleep_ms));
    }
    ZGLEAVE_FUN_LOG;
}

time_t ZGExternalVideoCaptureManager::GetTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    time_t timestamp = tmp.count();
    return timestamp;
}
