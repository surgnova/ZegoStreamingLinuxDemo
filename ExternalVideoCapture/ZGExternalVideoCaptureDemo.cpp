#include "ZGExternalVideoCaptureDemo.h"

#include "LiveRoom-Publisher.h"
#include "zego-api-external-video-capture.h"
#include "zego-api-external-audio-device.h"
#include "zego-api-audio-frame.h"

#include "ZGManager.h"
#include "ZGLog.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace ZEGO;

ZGExternalVideoCaptureDemo::ZGExternalVideoCaptureDemo()
{
    ZGENTER_FUN_LOG;
}

ZGExternalVideoCaptureDemo::~ZGExternalVideoCaptureDemo()
{
    ZGENTER_FUN_LOG;
}

void ZGExternalVideoCaptureDemo::EnableExternalVideoCapture()
{
    ZGENTER_FUN_LOG;

    // 在InitSDK 之前调用
    VCAP::SetVideoCaptureFactory(this);
    LIVEROOM::SetUseTestEnv(true);
    bool success = ZGManagerInstance()->InitSdk();
    ZGLog("ZGExternalVideoCaptureDemo::EnableExternalVideoCapture, Init SDK,success=%d\n", success);
    // 开麦克风
    //LIVEROOM::EnableMic(true);

    ZGLEAVE_FUN_LOG;
}

void ZGExternalVideoCaptureDemo::PushExternalVideoData(const char* data, int len, VideoCaptureFormat format, unsigned long long reference_time)
{
    /*
    cv::Mat picBmp = cv::Mat(format.width, format.height, CV_8UC4, (void*)data);
    char fileName[100];
    static int index = 0;
    snprintf(fileName, 100, "test_%d.bmp", index++);
    cv::imwrite(fileName, picBmp);  //only for test
    */
    //ZGENTER_FUN_LOG;
    {
        std::lock_guard<std::mutex> lock(capture_mutex_);
        if (capture_started_)
        {
            if (width_ != format.width || height_ != format.height)
            {
                width_ = format.width;
                height_ = format.height;

                ZGLog("SetVideoCaptureResolution and SetVideoEncodeResolution,w=%d h=%d",width_,height_);
                // 设置采集分辨率
                LIVEROOM::SetVideoCaptureResolution(width_, height_);
                // 设置编码分辨率
                LIVEROOM::SetVideoEncodeResolution(width_, height_);
            }

            //ZGLog("push external video data to sdk , data len = %d", len);
            if (client_ != nullptr)
            {
                /*
                unsigned int ptest[width_ * height_];
                for(int i = 0; i < width_ * height_; i++) {
                    unsigned int value = 0xff << 24;
                    ptest[i] = value;
                }*/
                //client_->OnIncomingCapturedData((const char *)ptest, len, format, reference_time, 1000);
                client_->OnIncomingCapturedData(data, len, format, reference_time, 1000);
            }
        }
    }
    //ZGLEAVE_FUN_LOG;
}

void ZGExternalVideoCaptureDemo::AllocateAndStart(Client* client)
{
    ZGENTER_FUN_LOG;
    client_ = client;
    ZGLEAVE_FUN_LOG;
}

void ZGExternalVideoCaptureDemo::StopAndDeAllocate()
{
    ZGENTER_FUN_LOG;
    client_->Destroy();
    client_ = nullptr;
    ZGLEAVE_FUN_LOG;
}

int ZGExternalVideoCaptureDemo::StartCapture()
{
    ZGENTER_FUN_LOG;

    std::lock_guard<std::mutex> lock(capture_mutex_);
    capture_started_ = true;
    width_ = 0;
    height_ = 0;

    ZGLEAVE_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::StopCapture()
{
    ZGENTER_FUN_LOG;

    std::lock_guard<std::mutex> lock(capture_mutex_);
    capture_started_ = false;
    ZGLEAVE_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::SetFrameRate(int framerate)
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::SetResolution(int width, int height)
{
    ZGENTER_FUN_LOG;


    return 0;
}

int ZGExternalVideoCaptureDemo::SetFrontCam(int bFront)
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::SetView(void *view)
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::SetViewMode(int nMode)
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::SetViewRotation(int nRotation)
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::SetCaptureRotation(int nRotation)
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::StartPreview()
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::StopPreview()
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::EnableTorch(bool bEnable)
{
    ZGENTER_FUN_LOG;
    return 0;
}

int ZGExternalVideoCaptureDemo::TakeSnapshot()
{
    ZGENTER_FUN_LOG;
    return 0;
}

AVE::VideoCaptureDeviceBase* ZGExternalVideoCaptureDemo::Create(const char* device_id)
{
    ZGENTER_FUN_LOG;

    return this;
}

void ZGExternalVideoCaptureDemo::Destroy(VideoCaptureDeviceBase *vc)
{
    ZGENTER_FUN_LOG;

}

