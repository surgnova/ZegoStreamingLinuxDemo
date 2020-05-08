cmake_minimum_required(VERSION 3.5.2)
project (zegoliveroomdemo)

find_package( OpenCV REQUIRED )

include_directories("./include")
include_directories("./AppSupport")

link_directories("./libs")

link_libraries(zegoliveroom rt pthread)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")

add_compile_options(
    -std=c++11
    )

#option(USE_EXTERNAL_RENDER "option for whether use external render" ON)

set (SRC_LIST main.cpp CallbackImpl.cpp
    ./ExternalVideoCapture/ZegoCamera.cpp
    ./ExternalVideoCapture/ZGCameraSource.cpp
    ./ExternalVideoCapture/ZGImageSource.cpp
    ./ExternalVideoCapture/ZGExternalVideoCaptureDemo.cpp
    ./ExternalVideoCapture/ZGExternalVideoCaptureManager.cpp
    ./ExternalVideoCapture/ZGExternalVideoCatpureDemoHelper.cpp
    ./ExternalVideoCapture/ZGExternalVideoSource.cpp
    ./ExternalVideoCapture/SurgnovaCamera.cpp
    ./AppSupport/ZGLog.cpp
    ./AppSupport/ZGManager.cpp
    ./AppSupport/ZGTimer.cpp
    ./AppSupport/ZGTaskQueue.cpp )


message (STATUS "ENABLE EXTERNAL CAPTURE FEATURE")
list (APPEND SRC_LIST media_publisher_event_callback.cpp)


if (USE_EXTERNAL_RENDER)
    message (STATUS "ENABLE EXTERNAL RENDER FEATURE")
    add_definitions(-DENABLE_EXTERNAL_RENDER_FEATURE)
    list (APPEND SRC_LIST ExternalRender.cpp)
endif()

if (USE_OFFICE_APP_KEY)
    add_definitions(-DUSE_OFFICE_APP_KEY)
endif()

if (ENABLE_MEDIA_SIDE_INFO_FEATURE)
    add_definitions(-DENABLE_MEDIA_SIDE_INFO_FEATURE)
endif()

add_executable(aidemo ${SRC_LIST})

target_link_libraries( aidemo ${OpenCV_LIBS} )
