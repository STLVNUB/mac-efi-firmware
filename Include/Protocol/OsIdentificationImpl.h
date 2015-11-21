// 18/07/2015

#ifndef __OS_IDENTIFICATION_IMPL_H__
#define __OS_IDENTIFICATION_IMPL_H__

// OS_IDENTIFICATION_PROTOCOL_REVISION
#define OS_IDENTIFICATION_PROTOCOL_REVISION  0x01

// OS_IDENTIFICATION_VENDOR_NAME
#define OS_IDENTIFICATION_VENDOR_NAME  "Apple Inc."

// OSName
VOID
EFIAPI
AppleOsIdentificationOSName (
	IN CHAR8 *OSName
	);

// OSVendor
VOID
EFIAPI
AppleOsIdentificationOSVendor (
	IN CHAR8 *OSVendor
	);

#endif // ifndef __OS_IDENTIFICATION_IMPL_H__
