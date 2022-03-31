#!/bin/sh

cat ./test_data/record_output | busybox awk '/EV_KEY \/ BTN_/ { print $12 }'
