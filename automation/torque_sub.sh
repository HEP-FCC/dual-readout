#! /bin/sh

for i in {1..500}
do
    qsub -q large -o /path/to/build/0/log -e /path/to/build/0/log -F "$i" /path/to/build/torque_exe.sh
    echo job $i submitted
done
