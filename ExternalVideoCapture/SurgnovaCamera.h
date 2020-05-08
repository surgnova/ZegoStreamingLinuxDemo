#ifndef __H_SURGNOVA_CAMERA
#define __H_SURGNOVA_CAMERA

#include "video_capture.h"

#include <string>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

class SurgnovaCamera : public AVE::VideoCaptureDevice {

public:
    static SurgnovaCamera* Create() {
        SurgnovaCamera * pInstance = new SurgnovaCamera();
        return pInstance;
    }

    static void Destroy(SurgnovaCamera* camera) {
        delete camera;
    }

   void SetDeviceId(const char* device_id);
protected:
    SurgnovaCamera();
    ~SurgnovaCamera();
public:
    virtual void AllocateAndStart(Client* client) override;

    virtual void StopAndDeAllocate() override;

    virtual int StartCapture() override;

    virtual int StopCapture() override;

    virtual AVE::VideoPixelBufferType SupportBufferType() override;

    virtual void* GetInterface() override;


    virtual int SetFrameRate(int framerate) override;

    virtual int SetResolution(int width, int height) override;

    virtual int SetFrontCam(int bFront) override;

    virtual int SetView(void *view) override;

    virtual int SetViewMode(int nMode) override;

    virtual int SetViewRotation(int nRotation) override;

    virtual int SetCaptureRotation(int nRotation) override;

    virtual int StartPreview() override;

    virtual int StopPreview() override;

    virtual int EnableTorch(bool bEnable) override;

    virtual int TakeSnapshot() override;
private:
    void CaptureVideoDataThread();
    time_t GetTimeStamp();
private:
    Client * mpClient;
    std::string m_deviceId;
    std::thread mCaptureThread;
    int mCameraFd;
    int mWidth;
    int mHeight;
    int mCaptureFPS;
    bool mStartCapture;
    uint8_t * mBuffer;
};

#endif
