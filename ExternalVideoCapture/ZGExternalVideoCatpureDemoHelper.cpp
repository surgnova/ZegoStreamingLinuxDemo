#include "ZGExternalVideoCatpureDemoHelper.h"

#include <assert.h>

#include "ZGManager.h"
//#include "ZGHelper.h"
#include "ZGLog.h"
//#include "ZGUtilTools.h"
//#include "ZGUIOperator.h"



ZGExternalVideoCatpureDemoHelper::ZGExternalVideoCatpureDemoHelper()
{
    //device_uuid_ = ZGHelperInstance()->GetDeviceUUID();
    sprintf(device_uuid_, "embedded_user_%d", 5654);
    sprintf(userName, "embedded_user_%d", rand() % 10000);
    mLoginSuccess = false;
}

ZGExternalVideoCatpureDemoHelper::~ZGExternalVideoCatpureDemoHelper()
{

}

void ZGExternalVideoCatpureDemoHelper::LoginRoomAndPublishing()
{
    ZGLog("Helper:: LoginRoomAndPublishing,uuid=%s, userName=%s\n", device_uuid_, userName);
    // 设置用户id，用户名字，业务自己保证唯一即可
    LIVEROOM::SetUser(device_uuid_, userName);
    // 设置推流回调
    LIVEROOM::SetLivePublisherCallback(this);
    // 设置拉流回调
    LIVEROOM::SetLivePlayerCallback(this);
    // 设置房间回调
    LIVEROOM::SetRoomCallback(this);
    // 登录房间
    bool success = LIVEROOM::LoginRoom(device_uuid_, ZEGO::LIVEROOM::Anchor, device_uuid_);
    ZGLog("LoginRoom return: %d\n", success);

    SetCurStatus(kZGExternalVideoCaptureStatus_Starting_Login_Room);
}

void ZGExternalVideoCatpureDemoHelper::Stop()
{
    // 停止推流
    LIVEROOM::StopPublishing();
    // 停止拉流
    LIVEROOM::StopPlayingStream(device_uuid_);
    // 退出登录
    LIVEROOM::LogoutRoom();
    // 反初始化sdk
    LIVEROOM::UnInitSDK();
}

void ZGExternalVideoCatpureDemoHelper::InitRemoteVideoView(void * remote_video_view)
{
    remote_video_view_ = remote_video_view;
}

string ZGExternalVideoCatpureDemoHelper::DescOfStatus(ZGExternalVideoCaptureStatus status)
{
    string status_str = "";
    switch (status)
    {
    case kZGExternalVideoCaptureStatus_None:
        status_str = "NONE";
    	break;
    case kZGExternalVideoCaptureStatus_Starting_Login_Room:
        status_str = "Logining";
        break;
    case kZGExternalVideoCaptureStatus_Login_OK:
        status_str = "LoginOk";
        break;
    case kZGExternalVideoCaptureStatus_Starting_Publishing:
        status_str = "Starting Publishing";
        break;
    case kZGExternalVideoCaptureStatus_Starting_Playing:
        status_str = "Starting Playing";
        break;
    case kZGExternalVideoCaptureStatus_Playing:
        status_str = "Playing";
        break;
    default:
        break;
    }

    return status_str;
}

vector<string> ZGExternalVideoCatpureDemoHelper::GetCameraDeviceIDList()
{
    int ncount = 0;
    AV::DeviceInfo* device_list = LIVEROOM::GetVideoDeviceList(ncount);
    vector<string> ret;
    for (int i = 0;i < ncount; ++i)
    {
        ret.push_back(device_list[i].szDeviceId);
    }
    return ret;
}

vector<string> ZGExternalVideoCatpureDemoHelper::GetCameraDeviceNameList()
{
    int ncount = 0;
    AV::DeviceInfo* device_list = LIVEROOM::GetVideoDeviceList(ncount);
    vector<string> ret;
    for (int i = 0; i < ncount; ++i)
    {
        ret.push_back(device_list[i].szDeviceName);
    }
    return ret;
}

void ZGExternalVideoCatpureDemoHelper::SetInitSdkCallBack(ZGInitSdkCallBackType cb)
{
    init_sdk_cb_ = cb;
}

void ZGExternalVideoCatpureDemoHelper::SetBitrate(int bitrate)
{
    // 设置码率，单位bps
    LIVEROOM::SetVideoBitrate(bitrate);
}

/*
void ZGExternalVideoCatpureDemoHelper::InitMainHwnd(HWND hwnd)
{
    main_hwnd_ = hwnd;
}
*/

void ZGExternalVideoCatpureDemoHelper::OnPublishQualityUpdate(const char* pszStreamID, LIVEROOM::ZegoPublishQuality publishQuality)
{
    ZGLog("helper::publish fps = %.4f", publishQuality.fps);
}

void ZGExternalVideoCatpureDemoHelper::OnPlayQualityUpdate(const char* pszStreamID, LIVEROOM::ZegoPlayQuality playQuality)
{
    ZGLog("helper::play fps = %.4f", playQuality.fps);
    ZGLog("helper::play quality = %d\n", playQuality.quality);
    ZGLog("helper::play rtt = %d\n", playQuality.rtt);
    ZGLog("helper::play peerToPeerDely=%d\n", playQuality.rtt);
}

void ZGExternalVideoCatpureDemoHelper::OnInitSDK(int nError)
{
    if (init_sdk_cb_)
    {
        init_sdk_cb_(nError);
    }
}

void ZGExternalVideoCatpureDemoHelper::OnLoginRoom(int errorCode, const char *pszRoomID, const ZegoStreamInfo *pStreamInfo, unsigned int streamCount)
{
    ZGLog("Helper::OnLoginRoom, roomID=%s, errorCode=%d\n", pszRoomID, errorCode);
    if (errorCode != 0)
    {
        SetCurStatus(kZGExternalVideoCaptureStatus_None);
    }
    else {
        SetCurStatus(kZGExternalVideoCaptureStatus_Login_OK);
        mLoginSuccess = true;
    }
}

void ZGExternalVideoCatpureDemoHelper::RealPublishing()
{
    ZGLog("Before Helper::OnLoginRoom, publishing\n");
    bool ret = LIVEROOM::StartPublishing(device_uuid_, device_uuid_, 0, "");
    ZGLog("Helper::OnLoginRoom, publishing ret=%d\n", ret);
    if (ret)
    {
        SetCurStatus(kZGExternalVideoCaptureStatus_Starting_Publishing);
    }
}

void ZGExternalVideoCatpureDemoHelper::OnKickOut(int reason, const char *pszRoomID)
{
    SetCurStatus(kZGExternalVideoCaptureStatus_None);
}

void ZGExternalVideoCatpureDemoHelper::OnDisconnect(int errorCode, const char *pszRoomID)
{
    SetCurStatus(kZGExternalVideoCaptureStatus_None);
}

void ZGExternalVideoCatpureDemoHelper::OnStreamUpdated(ZegoStreamUpdateType type, ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID)
{
}

void ZGExternalVideoCatpureDemoHelper::OnStreamExtraInfoUpdated(ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID)
{
}

void ZGExternalVideoCatpureDemoHelper::OnCustomCommand(int errorCode, int requestSeq, const char *pszRoomID)
{
}

void ZGExternalVideoCatpureDemoHelper::OnRecvCustomCommand(const char *pszUserId, const char *pszUserName, const char *pszContent, const char *pszRoomID)
{
}

void ZGExternalVideoCatpureDemoHelper::OnPublishStateUpdate(int stateCode, const char* pszStreamID, const ZegoPublishingStreamInfo& oStreamInfo)
{
    ZGLog("Helper::OnPublishStateUpdate, pszStreamID=%s, stateCode=%d\n", pszStreamID, stateCode);
    if (stateCode == 0)
    {
        void * remote_view = remote_video_view_;

        // 推流成功，开始拉流
        bool play_result = LIVEROOM::StartPlayingStream(device_uuid_, nullptr, "");
        if (play_result)
        {
            SetCurStatus(kZGExternalVideoCaptureStatus_Starting_Playing);
        }
        else {
            SetCurStatus(kZGExternalVideoCaptureStatus_None);
        }
    }
}

void ZGExternalVideoCatpureDemoHelper::OnPlayStateUpdate(int stateCode, const char* pszStreamID)
{
    if (stateCode == 0)
    {
        SetCurStatus(kZGExternalVideoCaptureStatus_Playing);
    }
    else {
        SetCurStatus(kZGExternalVideoCaptureStatus_None);
    }
}

void ZGExternalVideoCatpureDemoHelper::SetCurStatus(ZGExternalVideoCaptureStatus s)
{
    ZGENTER_FUN_LOG;

    ZGLog("cur status %d ", s);
    cur_status_ = s;
    if (cur_status_ == kZGExternalVideoCaptureStatus_None)
    {
        LIVEROOM::LogoutRoom();
    }

    if (status_cb_)
    {
        status_cb_(DescOfStatus(cur_status_));
    }
    ZGLEAVE_FUN_LOG;
}
