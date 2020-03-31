# Automation for Calibration and other jobs

## How-to
### Use
After compile the dual-readout package, In build/DRsim, do

    cp ../../automation/* ./
    chmod +x run_CEPC.sh
    ./run_CEPC.sh

### Stop
If you want to stop auto submit, do

    ps -ef | grep manage_number.sh

check process ID number

   kill -9 <process_ID>


