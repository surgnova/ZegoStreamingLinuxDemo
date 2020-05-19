#include "SurgnovaCamera.h"
#include "ZGLog.h"
#include "ZGTimer.h"

#include <iostream>

#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#define PRINT_CLASS printf("SurgnovaCamera::")

namespace ZEGO
{
    namespace CAMERA
    {
        SurgnovaCamera* CreateCamera()
        {
            ZGENTER_FUN_LOG;
            return SurgnovaCamera::Create();
        }

        void DestroyCamera(SurgnovaCamera *pCamera)
        {
            ZGENTER_FUN_LOG;
            SurgnovaCamera::Destroy((SurgnovaCamera *)pCamera);
        }
    }
}

static int xioctl(int fd, int request, void *arg)
{
    int r;
    do
        r = ioctl(fd, request, arg);
    while (-1 == r && EINTR == errno);

    return r;
}

SurgnovaCamera::SurgnovaCamera()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    mpClient = nullptr;
    m_deviceId = "";
    mCameraFd = 0;
    mWidth = 0;
    mHeight = 0;
    mStartCapture = false;
    mCaptureFPS = 30;
    mBuffer = NULL;
}

SurgnovaCamera::~SurgnovaCamera()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

void SurgnovaCamera::AllocateAndStart(Client* client)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    mpClient = client;

    //Open the video device here
    if(m_deviceId == "") {
        ZGLog("The device is not available.\n");
        return;
    }

    mCameraFd = open("/dev/video1", O_RDWR);
    if (mCameraFd == -1) {
        // couldn't find capture device
        perror("Opening Video device");
        return;
    }

    struct v4l2_capability caps = {};
    if (-1 == xioctl(mCameraFd, VIDIOC_QUERYCAP, &caps)) {
        perror("Querying Capabilities");
        return;
    }

    printf( "Driver Caps:\n"
            "  Driver: \"%s\"\n"
            "  Card: \"%s\"\n"
            "  Bus: \"%s\"\n"
            "  Version: %d.%d\n"
            "  Capabilities: %08x\n",
            caps.driver,
            caps.card,
            caps.bus_info,
            (caps.version>>16)&&0xff,
            (caps.version>>24)&&0xff,
            caps.capabilities);

    char fourcc[5] = {0};
    int support_grbg10 = 0;
    struct v4l2_fmtdesc fmtdesc = {0};
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    char c, e;
    printf("  FMT : CE Desc\n--------------------\n");
    while (0 == xioctl(mCameraFd, VIDIOC_ENUM_FMT, &fmtdesc)) {
        strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
        if (fmtdesc.pixelformat == V4L2_PIX_FMT_SGRBG10)
            support_grbg10 = 1;
        c = fmtdesc.flags & 1? 'C' : ' ';
        e = fmtdesc.flags & 2? 'E' : ' ';
        printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
        fmtdesc.index++;
    }

    //Query Pixel Format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(mCameraFd, VIDIOC_G_FMT, &fmt)) {
        perror("Getting Pixel Format");
        return;
    }

    strncpy(fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);
    printf( "Selected Camera Mode:\n"
            "  Width: %d\n"
            "  Height: %d\n"
            "  PixFmt: %s\n"
            "  Field: %d\n",
            fmt.fmt.pix.width,
            fmt.fmt.pix.height,
            fourcc,
            fmt.fmt.pix.field);

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 1920;
    fmt.fmt.pix.height      = 1080;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (-1 == xioctl(mCameraFd, VIDIOC_S_FMT, &fmt)) {
        perror("VIDIOC_S_FMT");
        return;
    }
    //Save the height and width
    mWidth = fmt.fmt.pix.width;
    mHeight = fmt.fmt.pix.height;
}

void SurgnovaCamera::StopAndDeAllocate()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    if (mCameraFd != 0) {
        close(mCameraFd);
        mCameraFd = 0;
    }
}

int SurgnovaCamera::StartCapture()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    mStartCapture = true;
    mCaptureThread = std::thread(std::bind(&SurgnovaCamera::CaptureVideoDataThread, this));
}

int SurgnovaCamera::StopCapture()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    if (mStartCapture) {
        mStartCapture = false;
        mCaptureThread.join();
    }
}

AVE::VideoPixelBufferType SurgnovaCamera::SupportBufferType()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    return AVE::VideoPixelBufferType::PIXEL_BUFFER_TYPE_MEM;
}

void* SurgnovaCamera::GetInterface()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

void SurgnovaCamera::SetDeviceId(const char* device_id)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    m_deviceId = device_id;
    std::cout << "m_deviceId=" << m_deviceId << std::endl;
}

int SurgnovaCamera::SetFrameRate(int framerate)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
    mCaptureFPS = framerate;
}

int SurgnovaCamera::SetResolution(int width, int height)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::SetFrontCam(int bFront)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::SetView(void *view)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::SetViewMode(int nMode)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::SetViewRotation(int nRotation)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::SetCaptureRotation(int nRotation)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::StartPreview()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::StopPreview()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::EnableTorch(bool bEnable)
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

int SurgnovaCamera::TakeSnapshot()
{
    PRINT_CLASS;
    ZGENTER_FUN_LOG;
}

void SurgnovaCamera::CaptureVideoDataThread()
{
    ZGENTER_FUN_LOG;

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(mCameraFd, VIDIOC_REQBUFS, &req)) {
        perror("Requesting Buffer");
        return;
    }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (-1 == xioctl(mCameraFd, VIDIOC_QUERYBUF, &buf)) {
        perror("Querying Buffer");
        return;
    }

    mBuffer = (uint8_t*)mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, mCameraFd, buf.m.offset);
    printf("Length: %d\nAddress: %p\n", buf.length, mBuffer);
    printf("Image Length: %d\n", buf.bytesused);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (-1 == xioctl(mCameraFd, VIDIOC_QBUF, &buf))
    {
        perror("Query Buffer");
        return;
    }

    if (-1 == xioctl(mCameraFd, VIDIOC_STREAMON, &buf.type)) {
        perror("Start Capture");
        return;
    }

    while (true)
    {
        ZGTimer timer;
        if (!mStartCapture) {
            break;
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(mCameraFd, &fds);
        struct timeval tv = {0};
        tv.tv_sec = 2;
        int r = select(mCameraFd + 1, &fds, NULL, NULL, &tv);
        if(-1 == r) {
            perror("Waiting for Frame");
            break;
        }

        if(-1 == xioctl(mCameraFd, VIDIOC_DQBUF, &buf)) {
            perror("Retrieving Frame");
            break;
        }

        /* // for v4l2 virtual camera
        cv::Mat picYV12 = cv::Mat(mHeight * 3/2, mWidth, CV_8UC1, (void*)mBuffer);
        cv::Mat picBGRA;
        cv::cvtColor(picYV12, picBGR, CV_YUV2RGBA_YV12);
        */

        /*
        cv::Mat picYV12 = cv::Mat(mHeight, mWidth, CV_8UC2, (void*)mBuffer);
        cv::Mat picBGRA;
        cv::cvtColor(picYV12, picBGR, CV_YUV2BGRA_YUY2);
        */
        cv::Mat picBGRA = cv::Mat(mWidth, mHeight, CV_8UC4);

        if (mBuffer[5] == 0) {
            printf("The buffer is zero.\n");
        } else {
            /*
            int jpgfile;
            char fileName[100];
            static int index = 0;
            snprintf(fileName, 100, "test_%d.jpeg", index++);
            if((jpgfile = open(fileName, O_WRONLY | O_CREAT, 0660)) < 0){
                perror("open");
            }
            write(jpgfile, mBuffer, buf.length);
            close(jpgfile);
            */

            cv::Mat jpgData = cv::Mat(1, buf.length, CV_8UC1, (void*)mBuffer);
            int length_jpg = jpgData.total() * jpgData.channels();
            cv::Mat picBGR = cv::imdecode(jpgData, cv::IMREAD_COLOR);
            int length_dbg = picBGR.total() * picBGR.channels();
            cv::cvtColor(picBGR, picBGRA, CV_BGR2BGRA);
        }

        if (mpClient != NULL) {
            int length = mWidth * mHeight * 4;
            AVE::VideoCaptureFormat frame_format(mWidth, mHeight, AVE::PIXEL_FORMAT_BGRA32);
            frame_format.strides[0] = mWidth * 4;
            unsigned int reference_time_scale = 1000;
            unsigned long long reference_time = GetTimeStamp();
            mpClient->OnIncomingCapturedData((const char *)picBGRA.data, length, frame_format, reference_time, reference_time_scale);
        }

        if(-1 == xioctl(mCameraFd, VIDIOC_QBUF, &buf)) {
            perror("Retrieving Frame");
            break;
        }

        int need_sleep_ms = 1000.0f / mCaptureFPS - timer.ElapsedMs();
        std::this_thread::sleep_for(std::chrono::milliseconds(need_sleep_ms));
    }

    // Deactivate streaming
    if(xioctl(mCameraFd, VIDIOC_STREAMOFF, &buf.type) < 0){
        perror("VIDIOC_STREAMOFF");
        return;
    }

    munmap(mBuffer, buf.length);
    mBuffer = NULL;
    ZGLEAVE_FUN_LOG;
}

time_t SurgnovaCamera::GetTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    time_t timestamp = tmp.count();
    return timestamp;
}
