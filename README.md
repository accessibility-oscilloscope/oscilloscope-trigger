# oscilloscope-trigger

## Using

### Setup
```
...
export SIGPROC_FIFO=/run/sigproc-in
export OSCOPE_FIFO=/run/oscope-in
...
mkfifo $SIGPROC_FIFO
mkfifo $OSCOPE_FIFO
```

### Active Components

```
get-btn.sh &
python3 -u trigger_data.py -i $OSCOPE_FIFO -o $SIGPROC_FIFO --serial /dev/ttyUSB0 &
```


## Testing
```
...
export SIGPROC_FIFO=/run/sigproc-in
export OSCOPE_FIFO=/run/oscope-in
...
mkfifo $SIGPROC_FIFO
mkfifo $OSCOPE_FIFO

### Terminal 1
get-btn-test.sh
### Terminal 2
python3 -u trigger_data.py -i $OSCOPE_FIFO -o $SIGPROC_FIFO --serial /dev/ttyUSB0 -v
### Terminal 3
cat $SIGPROC_FIFO | hexdump -C

```

## Detailed Description
Python program will read in libinput output via `$OSCOPE_FIFO`. 
- Reads in one byte at a time and checks if it is a `1`
- Then triggers the oscope to print back 480 bytes for each channel and only prints 480 from channel 0 to `$SIGPROC_FIFO`
`get-btn.sh` will output into the `$OSCOPE_FIFO`.
- Specifically uses busybox awk for better cross compatibility
- Output for a single press and release is `49 0a 48 0a`

### Notes
 - As far as I can tell, it looks like you need to get data from all four channels on the Haasoscope and then separate them yourself. 
 - Awk on Raspbian seems to love to buffer data and not return in a timely manner. 
   - Solution: use Busybox on Raspbian since that's what we're using on Buildroot anyway, and it doesn't have this problem for some reason.