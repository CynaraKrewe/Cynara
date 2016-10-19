/*
The MIT License (MIT)

Copyright (c) 2016 Cynara Krewe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software, hardware and associated documentation files (the "Solution"), to deal
in the Solution without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Solution, and to permit persons to whom the Solution is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Solution.

THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
SOLUTION.
 */

#ifndef TM4C_USB_CDC_H_
#define TM4C_USB_CDC_H_

#include "flow/flow.h"
#include "flow/utility.h"

#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"

// See http://pid.codes/1209/4096/
#define VID 0x1209
#define PID 0x4096

class UsbCdc
:	public Flow::Component
{
public:
	Flow::InPort<char> in;
	Flow::OutPort<char> out;

	UsbCdc();
	~UsbCdc();

	void run() final override;

	static uint32_t ReceiveHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData);
	static uint32_t TransmitHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData);
	static uint32_t ControlHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData);

private:
	void* device;

	// The languages supported by this device.
	static const uint8_t languageDescriptor[];
	// The manufacturer string.
	static const uint8_t manufacturerString[];
	// The product string.
	static const uint8_t productString[];
	// The serial number string.
	static const uint8_t serialNumberString[];
	// The control interface description string.
	static const uint8_t controlInterfaceString[];
	// The configuration description string.
	static const uint8_t configString[];
	// The descriptor string table.
	static const uint8_t* const stringDescriptors[];

	tUSBDCDCDevice cdcDevice =
	{
		VID,
		PID,
		100,
		USB_CONF_ATTR_SELF_PWR,
		ControlHandler,
		this,
		ReceiveHandler,
		this,
		TransmitHandler,
		this,
		stringDescriptors,
		ArraySizeOf(*stringDescriptors)
	};
};

#endif // TM4C_USB_CDC_H_
