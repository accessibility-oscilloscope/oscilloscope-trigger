# oscilloscope-trigger

## Testing
```
make

mkfifo oscope
mkfifo output

./trigger_data oscope output get-btn-test.sh
```
