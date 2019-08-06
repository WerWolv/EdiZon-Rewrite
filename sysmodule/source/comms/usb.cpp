/**
 * Copyright (C) 2019 WerWolv
 * 
 * This file is part of EdiZon.
 * 
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "comms/usb.hpp"

#include <cstring>
#include <malloc.h>

namespace edz::comms {

    EResult USBManager::usbCommsInterfaceInit5x() {
        EResult res;
        
        struct usb_interface_descriptor interfaceDescriptor = {
            .bLength = USB_DT_INTERFACE_SIZE,
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 4,
            .bNumEndpoints = 2,
            .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
            .bInterfaceSubClass = USB_CLASS_VENDOR_SPEC,
            .bInterfaceProtocol = USB_CLASS_VENDOR_SPEC,
        };

        struct usb_endpoint_descriptor endpointDescriptorIn = {
            .bLength = USB_DT_ENDPOINT_SIZE,
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = USB_ENDPOINT_IN,
            .bmAttributes = USB_TRANSFER_TYPE_BULK,
            .wMaxPacketSize = 0x40,
        };

        struct usb_endpoint_descriptor endpointDescriptorOut = {
            .bLength = USB_DT_ENDPOINT_SIZE,
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = USB_ENDPOINT_OUT,
            .bmAttributes = USB_TRANSFER_TYPE_BULK,
            .wMaxPacketSize = 0x40,
        };
        
        struct usb_ss_endpoint_companion_descriptor endpointCompanion = {
            .bLength = sizeof(struct usb_ss_endpoint_companion_descriptor),
            .bDescriptorType = USB_DT_SS_ENDPOINT_COMPANION,
            .bMaxBurst = 0x0F,
            .bmAttributes = 0x00,
            .wBytesPerInterval = 0x00,
        };

        USBManager::s_initialized = true;

        //The buffer for PostBufferAsync commands must be 0x1000-byte aligned.
        USBManager::s_endpointInBuffer = static_cast<u8*>(memalign(0x1000, 0x1000));
        if (USBManager::s_endpointInBuffer == nullptr) res = EResult(Module_Libnx, LibnxError_OutOfMemory);

        if (res.succeeded()) {
            USBManager::s_endpointOutBuffer = static_cast<u8*>(memalign(0x1000, 0x1000));
            if (USBManager::s_endpointOutBuffer == nullptr) res = EResult(Module_Libnx, LibnxError_OutOfMemory);
        }

        if (res.succeeded()) {
            memset(USBManager::s_endpointInBuffer, 0, 0x1000);
            memset(USBManager::s_endpointOutBuffer, 0, 0x1000);
        }

        if (res.failed()) return res;
        
        res = usbDsRegisterInterface(&USBManager::s_interface);
        if (res.failed()) return res;
        
        interfaceDescriptor.bInterfaceNumber = USBManager::s_interface->interface_index;
        endpointDescriptorIn.bEndpointAddress += interfaceDescriptor.bInterfaceNumber + 1;
        endpointDescriptorOut.bEndpointAddress += interfaceDescriptor.bInterfaceNumber + 1;
        
        // Full Speed Config
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Full, &interfaceDescriptor, USB_DT_INTERFACE_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Full, &endpointDescriptorIn, USB_DT_ENDPOINT_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Full, &endpointDescriptorOut, USB_DT_ENDPOINT_SIZE);
        if (res.failed()) return res;
        
        // High Speed Config
        endpointDescriptorIn.wMaxPacketSize = 0x200;
        endpointDescriptorOut.wMaxPacketSize = 0x200;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_High, &interfaceDescriptor, USB_DT_INTERFACE_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_High, &endpointDescriptorIn, USB_DT_ENDPOINT_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_High, &endpointDescriptorOut, USB_DT_ENDPOINT_SIZE);
        if (res.failed()) return res;
        
        // Super Speed Config
        endpointDescriptorIn.wMaxPacketSize = 0x400;
        endpointDescriptorOut.wMaxPacketSize = 0x400;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Super, &interfaceDescriptor, USB_DT_INTERFACE_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Super, &endpointDescriptorIn, USB_DT_ENDPOINT_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Super, &endpointCompanion, USB_DT_SS_ENDPOINT_COMPANION_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Super, &endpointDescriptorOut, USB_DT_ENDPOINT_SIZE);
        if (res.failed()) return res;
        res = usbDsInterface_AppendConfigurationData(USBManager::s_interface, UsbDeviceSpeed_Super, &endpointCompanion, USB_DT_SS_ENDPOINT_COMPANION_SIZE);
        if (res.failed()) return res;
        
        //Setup endpoints.    
        res = usbDsInterface_RegisterEndpoint(USBManager::s_interface, &USBManager::s_endpointIn, endpointDescriptorIn.bEndpointAddress);
        if (res.failed()) return res;
        
        res = usbDsInterface_RegisterEndpoint(USBManager::s_interface, &USBManager::s_endpointOut, endpointDescriptorOut.bEndpointAddress);
        if (res.failed()) return res;

        res = usbDsInterface_EnableInterface(USBManager::s_interface);
        
        return res;
    }


    EResult USBManager::usbCommsInterfaceInit1x() {
        EResult res;

        struct usb_interface_descriptor interfaceDescriptor = {
            .bLength = USB_DT_INTERFACE_SIZE,
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 0,
            .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
            .bInterfaceSubClass = USB_CLASS_VENDOR_SPEC,
            .bInterfaceProtocol = USB_CLASS_VENDOR_SPEC,
        };

        struct usb_endpoint_descriptor endpointDescriptorIn = {
            .bLength = USB_DT_ENDPOINT_SIZE,
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = USB_ENDPOINT_IN,
            .bmAttributes = USB_TRANSFER_TYPE_BULK,
            .wMaxPacketSize = 0x200,
        };

        struct usb_endpoint_descriptor endpointDescriptorOut = {
            .bLength = USB_DT_ENDPOINT_SIZE,
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = USB_ENDPOINT_OUT,
            .bmAttributes = USB_TRANSFER_TYPE_BULK,
            .wMaxPacketSize = 0x200,
        };

        USBManager::s_initialized = 1;

        //The buffer for PostBufferAsync commands must be 0x1000-byte aligned.
        USBManager::s_endpointInBuffer = static_cast<u8*>(memalign(0x1000, 0x1000));
        if (USBManager::s_endpointInBuffer == nullptr) res = EResult(Module_Libnx, LibnxError_OutOfMemory);

        if (res.succeeded()) {
            USBManager::s_endpointOutBuffer = static_cast<u8*>(memalign(0x1000, 0x1000));
            if (USBManager::s_endpointOutBuffer) res = EResult(Module_Libnx, LibnxError_OutOfMemory);
        }

        if (res.succeeded()) {
            memset(USBManager::s_endpointInBuffer, 0, 0x1000);
            memset(USBManager::s_endpointOutBuffer, 0, 0x1000);
        }

        if (res.failed()) return res;

        //Setup interface.
        res = usbDsGetDsInterface(&USBManager::s_interface, &interfaceDescriptor, "usb");
        if (res.failed()) return res;

        //Setup endpoints.
        res = usbDsInterface_GetDsEndpoint(USBManager::s_interface, &USBManager::s_endpointIn, &endpointDescriptorIn);//device->host
        if (res.failed()) return res;

        res = usbDsInterface_GetDsEndpoint(USBManager::s_interface, &USBManager::s_endpointOut, &endpointDescriptorOut);//host->device
        if (res.failed()) return res;

        res = usbDsInterface_EnableInterface(USBManager::s_interface);

        return res;
    }

    EResult USBManager::initialize() {
        EResult res;

        if (USBManager::s_initialized)
            res = EResult(Module_Libnx, LibnxError_AlreadyInitialized); 
        else
            res = usbDsInitialize();

        rwlockInit(&USBManager::s_usbLock);
        rwlockInit(&USBManager::s_interfaceLock);
        rwlockInit(&USBManager::s_inLock);
        rwlockInit(&USBManager::s_outLock);

        rwlockWriteLock(&USBManager::s_usbLock);

        if (res.succeeded()) {
            if (hosversionAtLeast(5, 0, 0)) {
                u8 iManufacturer, iProduct, iSerialNumber;
                static const u16 supportedLangs[1] = { 0x0409 };

                res = usbDsAddUsbLanguageStringDescriptor(nullptr, supportedLangs, sizeof(supportedLangs) / sizeof(u16));
                
                if (res.succeeded()) 
                    res = usbDsAddUsbStringDescriptor(&iManufacturer, "Nintendo");
                
                if (res.succeeded())
                    res = usbDsAddUsbStringDescriptor(&iProduct, "Nintendo Switch [ EdiZon ]");
                
                if (res.succeeded())
                    res = usbDsAddUsbStringDescriptor(&iSerialNumber, "EdiZon");
                    
                struct usb_device_descriptor deviceDescriptor = {
                    .bLength = USB_DT_DEVICE_SIZE,
                    .bDescriptorType = USB_DT_DEVICE,
                    .bcdUSB = 0x0110,
                    .bDeviceClass = 0x00,
                    .bDeviceSubClass = 0x00,
                    .bDeviceProtocol = 0x00,
                    .bMaxPacketSize0 = 0x40,
                    .idVendor = 0x057e,
                    .idProduct = 0x3001,
                    .bcdDevice = 0x0100,
                    .iManufacturer = iManufacturer,
                    .iProduct = iProduct,
                    .iSerialNumber = iSerialNumber,
                    .bNumConfigurations = 0x01
                };

                // Full Speed (USB 1.1) support
                if (res.succeeded())
                    res = usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed_Full, &deviceDescriptor);

                // High Speed (USB 2.0) support
                if (res.succeeded())
                    res = usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed_High, &deviceDescriptor);

                // Super Speed (USB 3.0) support
                if (res.succeeded()) {
                    deviceDescriptor.bcdUSB = 0x0300;
                    deviceDescriptor.bMaxPacketSize0 = 0x09;    // Set package size to 512
                    res = usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed_Super, &deviceDescriptor);
                }

                u8 binaryObjectStore[0x16] = {
                    0x05,                       // .bLength
                    USB_DT_BOS,                 // .bDescriptorType
                    0x16, 0x00,                 // .wTotalLength
                    0x02,                       // .bNumDeviceCaps
                    
                    // USB 2.0
                    0x07,                       // .bLength
                    USB_DT_DEVICE_CAPABILITY,   // .bDescriptorType
                    0x02,                       // .bDevCapabilityType
                    0x02, 0x00, 0x00, 0x00,     // dev_capability_data
                    
                    // USB 3.0
                    0x0A,                       // .bLength
                    USB_DT_DEVICE_CAPABILITY,   // .bDescriptorType
                    0x03,                       // .bDevCapabilityType
                    0x00, 0x0E, 0x00, 0x03, 0x00, 0x00, 0x00
                };

                if (res.succeeded())
                    res = usbDsSetBinaryObjectStore(binaryObjectStore, sizeof(binaryObjectStore));

                if (res.succeeded()) {
                    rwlockWriteLock(&USBManager::s_interfaceLock);
                    rwlockWriteLock(&USBManager::s_inLock);
                    rwlockWriteLock(&USBManager::s_outLock);

                    if (hosversionAtLeast(5, 0, 0))
                        USBManager::usbCommsInterfaceInit5x();
                    else
                        USBManager::usbCommsInterfaceInit1x();

                    rwlockWriteUnlock(&USBManager::s_outLock);
                    rwlockWriteUnlock(&USBManager::s_inLock);
                    rwlockWriteUnlock(&USBManager::s_interfaceLock);
                }

                if (res.succeeded() && hosversionAtLeast(5,0,0)) {
                    res = usbDsEnable();
                }
                
                if (res.failed()) {
                    USBManager::exit();
                }
            }
        }
    }

    void USBManager::exit() {
        rwlockWriteLock(&USBManager::s_usbLock);
        usbDsExit();

        rwlockWriteUnlock(&USBManager::s_usbLock);

        if (!USBManager::s_initialized) {
            rwlockWriteUnlock(&USBManager::s_interfaceLock);
            return;
        }

        rwlockWriteLock(&USBManager::s_inLock);
        rwlockWriteLock(&USBManager::s_outLock);

        USBManager::s_initialized = false;

        USBManager::s_endpointIn = nullptr;
        USBManager::s_endpointOut = nullptr;
        USBManager::s_interface = nullptr;

        free(USBManager::s_endpointInBuffer);
        free(USBManager::s_endpointOutBuffer);  
        USBManager::s_endpointInBuffer = nullptr;
        USBManager::s_endpointOutBuffer = nullptr;

        rwlockWriteUnlock(&USBManager::s_outLock);
        rwlockWriteUnlock(&USBManager::s_inLock);
        rwlockWriteUnlock(&USBManager::s_interfaceLock);

    }


    void USBManager::process() {

    }


    EResult USBManager::read(std::function<void(u8 *buffer, size_t size)> callback, size_t bufferSize, u32 timeout) {

    }

    EResult USBManager::write(u8 *buffer, size_t size, u32 timeout) {

    }


    bool USBManager::isBusy() {
        return USBManager::s_busy;
    }

    EResult USBManager::abort() {
        USBManager::s_listening = false;
    }

}