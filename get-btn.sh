#!/bin/sh

DEVICE=$(libinput list-devices | grep "Wacom Intuos BT M Pad" -A1 | awk '/Kernel:/ {print $2}')
libinput debug-events "$DEVICE" | awk '/EV_KEY \/ BTN_/ { print $11,$12 }'
