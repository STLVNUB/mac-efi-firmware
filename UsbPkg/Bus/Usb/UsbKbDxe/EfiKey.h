/*++
Copyright (c) 2004 - 2008, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

    EfiKey.h

Abstract:

    Header file for USB Keyboard Driver's Data Structures

Revision History
--*/
#ifndef _USB_KB_H
#define _USB_KB_H

#include <AppleMacEfi.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/UefiUsbLib.h>
#include <IndustryStandard/AppleHid.h>
#include <Protocol/AppleKeyMapDatabase.h>
#include <Protocol/KeyboardInfo.h>
#include <Protocol/ApplePlatformInfoDatabase.h>
#include <Guid/ApplePlatformInfo.h>
#include <Library/DevicePathLib.h>

//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/DevicePath.h>
#include <Protocol/UsbIo.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/HotPlugDevice.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Protocol/StatusCode.h>

//
// Driver Produced Protocol Prototypes
//
#include <Protocol/DriverBinding.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>

#include <IndustryStandard/Usb.h>

#define MAX_KEY_ALLOWED     32

#define HZ                  1000 * 1000 * 10
#define USBKBD_REPEAT_DELAY ((HZ) / 2)
#define USBKBD_REPEAT_RATE  ((HZ) / 50)

#define CLASS_HID           3
#define SUBCLASS_BOOT       1
#define PROTOCOL_KEYBOARD   1

#define BOOT_PROTOCOL       0
#define REPORT_PROTOCOL     1

typedef struct {
  UINT8 Down;
  UINT8 KeyCode;
} USB_KEY;

typedef struct {
  USB_KEY buffer[MAX_KEY_ALLOWED + 1];
  UINT8   bHead;
  UINT8   bTail;
} USB_KB_BUFFER;

#define USB_KB_DEV_SIGNATURE  SIGNATURE_32 ('u', 'k', 'b', 'd')

typedef struct {
  UINTN                         Signature;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_EVENT                     DelayedRecoveryEvent;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL SimpleInput;
  APPLE_KEY_MAP_DATABASE_PROTOCOL *KeyMapDb;
  UINTN                           KeyMapDbIndex;
  EFI_USB_IO_PROTOCOL           *UsbIo;

  EFI_USB_DEVICE_DESCRIPTOR     DeviceDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR   IntEndpointDescriptor;

  USB_KB_BUFFER                 KeyboardBuffer;
  UINT8                         CtrlOn;
  UINT8                         AltOn;
  UINT8                         ShiftOn;
  UINT8                         NumLockOn;
  UINT8                         CapsOn;
  UINT8                         ScrollOn;
  UINT8                         LastKeyCodeArray[8];
  UINT8                         CurKeyChar;

  UINT8                         RepeatKey;
  EFI_EVENT                     RepeatTimer;

  EFI_UNICODE_STRING_TABLE      *ControllerNameTable;

  UINT8                         Unknown;
} USB_KB_DEV;

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL  gUsbKeyboardDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL  gUsbKeyboardComponentName;
extern EFI_GUID                     gEfiUsbKeyboardDriverGuid;

#define USB_KB_DEV_FROM_THIS(a) \
    CR(a, USB_KB_DEV, SimpleInput, USB_KB_DEV_SIGNATURE)

#define MOD_CONTROL_L           0x01
#define MOD_CONTROL_R           0x10
#define MOD_SHIFT_L             0x02
#define MOD_SHIFT_R             0x20
#define MOD_ALT_L               0x04
#define MOD_ALT_R               0x40
#define MOD_WIN_L               0x08
#define MOD_WIN_R               0x80

typedef struct {
  UINT8 Mask;
  UINT8 Key;
} KB_MODIFIER;

#define USB_KEYCODE_MAX_MAKE      0x64

#define USBKBD_VALID_KEYCODE(key) ((UINT8) (key) > 3)

typedef struct {
  UINT8 NumLock : 1;
  UINT8 CapsLock : 1;
  UINT8 ScrollLock : 1;
  UINT8 Resrvd : 5;
} LED_MAP;


#endif
