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

#include <stdio.h>
#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDValue.h>
#include "logitech.h"

// There's a problem with the device: once multiple buttons are pressed
// an then released, the device generates only one mouse up event for the first
// released button, but doesn't generate anything on subsequent releases.
// It also acts very weird when 3 buttons are pressed together.
// So, in order to tackle this, I'll be maintaining the last pressed button state
// and release it immediately once another button is pressed or released.

long lastPressedButton = 0;

CGMouseButton mapDeviceButton(long deviceButton) {
    switch (deviceButton) {
        case BTN_MIDDLE:
            return kCGMouseButtonCenter;
        case BTN_SIDE_1:
            return 5; // starting from 5 because 3 & 4 already taken by the wheel rocker
        case BTN_SIDE_2:
            return 6;
    }

    return 0; // left button as fallback sounds kinda okay :)
}

void postEvent(CGEventType eventType, CGMouseButton button) {
    // get cursor position
    CGEventRef whyDoWeEvenNeedThisThing = CGEventCreate(NULL);
    CGPoint point = CGEventGetLocation(whyDoWeEvenNeedThisThing);
    CFRelease(whyDoWeEvenNeedThisThing);

    // post the event
    CGEventRef mouseEvent = CGEventCreateMouseEvent(NULL, eventType, point, button);
    CGEventPost(kCGHIDEventTap, mouseEvent);
    CFRelease(mouseEvent);
}

void pressButton(long deviceButton) {
    if (deviceButton == 0) 
        return; // ignore as it's a release

    // save state
    lastPressedButton = deviceButton;
    
    // emulate mouse down
    CGMouseButton cgButton = mapDeviceButton(deviceButton);
    postEvent(kCGEventOtherMouseDown, cgButton);

    // printf driven development:
    // printf("down: %u\n", cgButton);
}

void releaseButton(long deviceButton)
{
    if (deviceButton == 0) 
        return; // ignore as no button has been pressed previously

    // reset state
    lastPressedButton = 0;

    // emulate mouse up
    CGMouseButton cgButton = mapDeviceButton(deviceButton);
    postEvent(kCGEventOtherMouseUp, cgButton);

    // printf driven development:
    // printf("up:   %u\n", cgButton);
}

void logitechHandler(IOHIDValueRef value) {
	int len = IOHIDValueGetLength(value);
	long intValue = IOHIDValueGetIntegerValue(value);

    // printf driven development:
	// printf("len: %02d, value: %016lx\n", len, intValue);

	if (len != 1)
		return; // skip the other payload; it's useless anyway

    releaseButton(lastPressedButton);
    pressButton(intValue);
}