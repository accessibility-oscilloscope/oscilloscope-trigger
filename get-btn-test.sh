#!/bin/sh

# DEVICE=$(libinput list-devices | grep "Wacom Intuos BT M Pad" -A1 | awk '/Kernel:/ {print $2}')
# libinput debug-events "$DEVICE"
cat ./test_data/record_output | busybox awk '/EV_KEY \/ BTN_/ { print $12 }'
