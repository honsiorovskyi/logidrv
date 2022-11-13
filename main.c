/*
Copyright (c) 2022 logidrv developers

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDValue.h>
#include <IOKit/hid/IOHIDManager.h>
#include "logitech.h"

#define UP_LOGITECH_MOUSE 0xff00

void hidCallback(void* context,  IOReturn result,  void* sender,  IOHIDValueRef value) {
	uint32_t usagePage = IOHIDElementGetUsagePage(IOHIDValueGetElement(value));
	switch (usagePage) {
		case UP_LOGITECH_MOUSE:
			logitechHIDHandler(value);
			return;
	}
}

CFMutableDictionaryRef createMatchingDict(UInt32 usagePage,  UInt32 usage) {
	CFMutableDictionaryRef dict = CFDictionaryCreateMutable(
		kCFAllocatorDefault,
		0,
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks
	);

	if (!dict)
		return NULL;

	if (usagePage != -1) {
		CFNumberRef pageNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);
		if (!pageNumberRef) {
			CFRelease(dict);
			return NULL;
		}

		CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsagePageKey), pageNumberRef);
		CFRelease(pageNumberRef);
	}

	if (usage != -1) {
		CFNumberRef usageNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);

		if (!usageNumberRef) {
			CFRelease(dict);
			return NULL;
		}

		CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsageKey), usageNumberRef);
		CFRelease(usageNumberRef);
	}

	return dict;
}

CFArrayRef createMatchings() {
	const CFMutableDictionaryRef matchesList[] = {
		createMatchingDict(UP_LOGITECH_MOUSE, -1),
		// createMatchingDict(0x01, 6) // keyboard
		// createMatchingDict(0x01, 7) // keypad
	};

	int matchesLen = sizeof(matchesList) / sizeof(CFMutableDictionaryRef);
	return CFArrayCreate(kCFAllocatorDefault, (const void **)matchesList, matchesLen, NULL);
}

void setupHIDHandler() {
	IOHIDManagerRef hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

	// IOHIDManagerSetDeviceMatching(hidManager, NULL); // catch all
	IOHIDManagerSetDeviceMatchingMultiple(hidManager, createMatchings());
	IOHIDManagerRegisterInputValueCallback(hidManager, hidCallback, NULL);
	IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
	IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
}

CGEventRef eventTapCallback (CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    switch (type) {
    	case kCGEventLeftMouseUp:
        case kCGEventLeftMouseDown:
			return logitechEventTapHandler(type, event);
    }
    
    return event;
}

void setupCGEventTap() {
    // CGEventMask eventMask = kCGEventMaskForAllEvents;
	CGEventMask eventMask = (
		CGEventMaskBit(kCGEventLeftMouseUp) |
		CGEventMaskBit(kCGEventLeftMouseDown)
	);

	CFMachPortRef eventTap = CGEventTapCreate(
		kCGHIDEventTap,
 		kCGHeadInsertEventTap,
 		kCGEventTapOptionDefault,
 		eventMask,
 		&eventTapCallback,
 		NULL
 	);
    
    if (!eventTap) {
        fprintf(stderr, "failed to create event tap\n");
        exit(1);
    }
    
    CFRunLoopSourceRef runLoopSourceRef = CFMachPortCreateRunLoopSource(NULL, eventTap, 0);
    CFRelease(eventTap);
    
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSourceRef, kCFRunLoopDefaultMode);
    CFRelease(runLoopSourceRef);
    
    CGEventTapEnable(eventTap, true);
}

int main(void) {
	setupHIDHandler();
	setupCGEventTap();

	CFRunLoopRun();
}
