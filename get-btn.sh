#!/bin/sh

DEVICE=$(libinput list-devices | grep "Wacom Intuos BT M Pad" -A1 | awk '/Kernel:/ {print $2}')
libinput record "$DEVICE" | busybox awk '/EV_KEY \/ BTN_/ { print $12 }'
