#!/bin/sh

DEVICE=$(libinput list-devices | grep "Wacom Intuos BT M Pad" -A1 | awk '/Kernel:/ {print $2}')
libinput record "$DEVICE" | awk '/EV_KEY / { print $11,$12 }'
