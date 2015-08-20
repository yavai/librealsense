#pragma once
#ifndef LIBREALSENSE_UVC_CAMERA_H
#define LIBREALSENSE_UVC_CAMERA_H

#include "rs-internal.h"

#ifdef USE_UVC_DEVICES
#include <libuvc/libuvc.h>

namespace rs
{
    inline void CheckUVC(const char * call, uvc_error_t status)
    {
        if (status < 0)
        {
            throw std::runtime_error(ToString() << call << "(...) returned " << uvc_strerror(status));
        }
    }

    inline void GetUSBInfo(uvc_device_t * dev, rs::USBDeviceInfo & info)
    {
        uvc_device_descriptor_t * desc;
        if (uvc_get_device_descriptor(dev, &desc) == UVC_SUCCESS)
        {
            if (desc->serialNumber) info.serial = desc->serialNumber;
            if (desc->idVendor) info.vid = desc->idVendor;
            if (desc->idProduct) info.pid = desc->idProduct;
            uvc_free_device_descriptor(desc);
        }
        else throw std::runtime_error("call to uvc_get_device_descriptor() failed");
    }

    class UVCCamera : public rs_camera
    {
        NO_MOVE(UVCCamera);

    protected:
        
        struct StreamInterface
        {
            UVCCamera * camera = nullptr;
            uvc_device_handle_t * uvcHandle = nullptr;
            uvc_frame_format fmt = UVC_FRAME_FORMAT_UNKNOWN;
            uvc_stream_ctrl_t ctrl = uvc_stream_ctrl_t{}; // {0};
        };

        uvc_context_t * internalContext;
        uvc_device_t * hardware = nullptr;
        std::unique_ptr<StreamInterface> streams[2];

        virtual int GetDepthCameraNumber() const = 0;
        virtual int GetColorCameraNumber() const = 0;
        virtual void RetrieveCalibration() = 0;
        virtual void SetStreamIntent(bool depth, bool color) = 0;

    public:

        UVCCamera(uvc_context_t * ctx, uvc_device_t * device, int num);
        ~UVCCamera();

        void EnableStream(int stream, int width, int height, int fps, FrameFormat format) override final;
        void StartStreaming() override final;
        void StopStreaming() override final;
    };
    
} // end namespace rs
#endif

#endif
