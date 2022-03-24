#!/usr/bin/env python3
import syslog
from serial import Serial
from struct import unpack
import time
import argparse
import os

# Seems that all four channels must be queried, so need to multiply by 4
NUM_SAMPLES = 480
NS_BYTE_1 = NUM_SAMPLES >> 8
NS_BYTE_2 = NUM_SAMPLES % 255

# Reference https://github.com/drandyhaas/Haasoscope/blob/master/software/serial_read.py
INIT_SEQ = [
    [0, 20],  # Set board id to 0
    [135, 0, 100],  # serialdelaytimerwait of 100
    [122, NS_BYTE_1, NS_BYTE_2],  # number of samples = NS_BYTE_1 * 255 + NS_BYTE_2
    [123, 0],  # send increment
    [124, 3],  # downsample 3
    [125, 1],  # tickstowait 1
    [136, 2, 32, 0, 0, 255, 200],  # io expanders on (!)
    [136, 2, 32, 1, 0, 255, 200],  # io expanders on (!)
    [136, 2, 33, 0, 0, 255, 200],  # , io expanders on (!)
    [136, 2, 33, 1, 0, 255, 200],  # io expanders on (!)
    [136, 2, 32, 18, 240, 255, 200],  # init
    [136, 2, 32, 19, 15, 255, 200],  # init (and turn on ADCs!)
    [136, 2, 33, 18, 0, 255, 200],  # init
    [136, 2, 33, 19, 0, 255, 200],  # init
    [131, 8, 0],  # adc offset
    [131, 6, 16],  # offset binary output
    # [131, 6, 80], #test pattern output
    [131, 4, 36],  # 300 Ohm termination A
    [131, 5, 36],  # 300 Ohm termination B
    [131, 1, 0],  # not multiplexed
    [136, 3, 96, 80, 136, 22, 0],  # channel 0 , board 0 calib
    [136, 3, 96, 82, 136, 22, 0],  # channel 1 , board 0 calib
    [136, 3, 96, 84, 136, 22, 0],  # channel 2 , board 0 calib
    [136, 3, 96, 86, 136, 22, 0],  # channel 3 , board 0 calib
]

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('-i', metavar='i', dest='input_path', type=str, help='input fifo')
    parser.add_argument('-o', metavar='o', dest='output_path', type=str, help='output fifo', default='/dev/stdout')
    parser.add_argument('--serial', metavar='s', dest='oscope_path', type=str, help='Oscope path',
                        default='/dev/ttyUSB0')
    parser.add_argument('-v', dest='verbose', type=bool, default=False, help='verbose',
                        action=argparse.BooleanOptionalAction)

    args = parser.parse_args()
    input_path = args.input_path
    output_path = args.output_path
    oscope_path = args.oscope_path
    verbose = args.verbose

    input_fifo = os.open(input_path, os.O_RDONLY)
    output_fifo = os.open(output_path, os.O_WRONLY)

    if verbose:
        syslog.syslog("reading")
    result = os.read(input_fifo, 1)
    if len(result) != 1:
        syslog.syslog("read " + str(len(result)) + " bytes")
        exit(1)

    if verbose:
        syslog.syslog(str(unpack('%dB' % len(result), result)))
    if result == b'1':
        if verbose:
            syslog.syslog("writing to oscope")

        ser = Serial(oscope_path, 1500000, timeout=1.0)
        for command in INIT_SEQ:
            ser.write(bytearray(command))
            time.sleep(.1)  # pessimistic init sequence delay

        ser.write(bytearray([100, 10]))  # arm trigger and get an event

        if verbose:
            syslog.syslog("reading from oscope")
        read_result = ser.read(NUM_SAMPLES * 4)  # read values
        ser.close()

        if len(read_result) != NUM_SAMPLES * 4:
            exit(1)
        channel = []
        for i in range(0, 4):
            channel.append(read_result[NUM_SAMPLES * i:NUM_SAMPLES * i + NUM_SAMPLES])
        if verbose:
            unpacked_data = unpack('%dB' % len(read_result), read_result)
            syslog.syslog(str(unpacked_data[0: int(len(unpacked_data)/10)]))

        amount_written = os.write(output_fifo, channel[0])
        if verbose:
            syslog.syslog("wrote "+str(amount_written)+" bytes")
        if len(channel[0]) != amount_written:
            syslog.syslog("failed to write correct amount")
            exit(1)

    os.close(input_fifo)
    os.close(output_fifo)

    exit(0)
