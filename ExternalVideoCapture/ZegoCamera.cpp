#include "ZegoCamera.h"
#include "ZGLog.h"

class ZegoVideoCapExternalClient : public AVE::VideoCaptureDevice::Client
{
public:
    ZegoVideoCapExternalClient(ZegoVideoCapExternal *pExternal, std::string& deviceId)
        : m_pExternal(pExternal),
          m_deviceId(deviceId)
    {}

    ~ZegoVideoCapExternalClient()
    {
        m_pExternal = nullptr;
        m_deviceId.clear();
    }

public:
    void Destroy() override {
        delete this;
    }

    void OnError(const char* reason) override
    {
        if (m_pExternal)
            m_pExternal->OnCaptureDeviceError(m_deviceId, reason);
    }

    void SetFillMode(int mode) override {}

public:
    void OnIncomingCapturedData(
        const char* data,
        int length,
        const AVE::VideoCaptureFormat& frame_format,
        unsigned long long reference_time,
        unsigned int reference_time_scale) override
    {
        if (m_pExternal)
            m_pExternal->OnIncommingCaptureData(m_deviceId, data, length, frame_format, reference_time, reference_time_scale);
    }

    void OnTakeSnapshot(void *image) override
	{
		if (m_pExternal)
			m_pExternal->OnTakeSnapshot(image);
    }

    std::string getDeviceId()
    {
        return m_deviceId;
    }
private:
    ZegoVideoCapExternal *m_pExternal;
    std::string m_deviceId;
};

//
// * ZegoVideoCapExternal
//
std::string ZegoVideoCapExternal::GetDeviceId()
{
	return m_deviceId;
}

int ZegoVideoCapExternal::SetCamera(std::string deviceId)
{
    ZGENTER_FUN_LOG;
    ZGLog("deviceId=%s\n", deviceId.c_str());
    ZGLog("m_deviceId=%s\n", m_deviceId.c_str());
	if (deviceId.size() == 0)
	{
		m_deviceId = "";
		return -1;
	}

	if (m_deviceId == deviceId)
	{
		return -1;
	}

    SurgnovaCamera *pCameraDevice =  ZEGO::CAMERA::CreateCamera();

	if (pCameraDevice == nullptr)
	{
		return -2;
	}

	m_deviceId = deviceId;

	AVE::VideoPixelBufferType bufferType = pCameraDevice->SupportBufferType();
	if (bufferType & AVE::PIXEL_BUFFER_TYPE_MEM)
	{
		ZegoVideoCapExternalClient *client = new ZegoVideoCapExternalClient(this, m_deviceId);
		pCameraDevice->SetDeviceId(m_deviceId.c_str());
		pCameraDevice->AllocateAndStart(client);

		m_deviceInfo.cameraDevice = pCameraDevice;
		m_deviceInfo.isCapture = false;
	}
	else
	{

        ZEGO::CAMERA::DestroyCamera(pCameraDevice);
	}

    ZGLEAVE_FUN_LOG;
	return 0;
}

int ZegoVideoCapExternal::RemoveCamera()
{
    ZGENTER_FUN_LOG;
	SurgnovaCamera *pCameraDevice = m_deviceInfo.cameraDevice;
	if (pCameraDevice)
	{
		pCameraDevice->StopAndDeAllocate();
        ZEGO::CAMERA::DestroyCamera(pCameraDevice);
	}

	m_deviceInfo.Reset();

    ZGLEAVE_FUN_LOG;
	return 0;
}


int ZegoVideoCapExternal::StartCapture()
{
    ZGENTER_FUN_LOG;
	if (m_deviceInfo.isCapture)
		return 0;

	m_deviceInfo.isCapture = true;

	if (m_deviceInfo.cameraDevice)
		m_deviceInfo.cameraDevice->StartCapture();

	return 0;
}

int ZegoVideoCapExternal::StopCapture()
{
    ZGENTER_FUN_LOG;
	m_deviceInfo.isCapture = false;

	if (m_deviceInfo.cameraDevice)
		m_deviceInfo.cameraDevice->StopCapture();

	return 0;
}

int ZegoVideoCapExternal::SetFrameRate(int framerate)
{
    ZGENTER_FUN_LOG;
	if (m_deviceInfo.cameraDevice)
		m_deviceInfo.cameraDevice->SetFrameRate(framerate);

	return 0;
}

int ZegoVideoCapExternal::SetCameraResolution(int width, int height)
{
    ZGENTER_FUN_LOG;
	if (m_deviceInfo.cameraDevice)
		m_deviceInfo.cameraDevice->SetResolution(width, height);

	return 0;
}

void ZegoVideoCapExternal::OnCaptureDeviceError(const std::string& deviceId, const char* reason)
{
    ZGENTER_FUN_LOG;

	if (m_deviceId != deviceId)
		return;

	//RemoveCamera();

	if (on_device_error_delegate_)
        on_device_error_delegate_(deviceId.c_str(), reason);
}

void ZegoVideoCapExternal::OnIncommingCaptureData(
	const std::string& deviceId,
	const char* data,
	int length,
	const AVE::VideoCaptureFormat& frame_format,
	unsigned long long reference_time,
	unsigned int reference_time_scale)
{
    //ZGENTER_FUN_LOG;
    if (on_video_frame_delegate_) {
        on_video_frame_delegate_(data, length, frame_format, reference_time, reference_time_scale);
    }
}

void ZegoVideoCapExternal::OnTakeSnapshot(void* image)
{
    ZGENTER_FUN_LOG;
}




