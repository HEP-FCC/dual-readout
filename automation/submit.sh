#! /bin/sh

set -e

# counts the total number of rows
TOT_ROWS=`awk 'END{print NR}' calibtheta.txt`
TOWER_NUM=$1
ROW_NUM=$(($1+1))


function submit()
{
    mkdir -p $TOWER_NUM/log
    mkdir $TOWER_NUM/output

    if [ $BATCH_SYS = "HTCondor" ]; then
       condor_submit condor.sub
    elif [ $BATCH_SYS = "Torque" ]; then
        chmod +x ./torque_sub.sh
       ./torque_sub.sh
    else
        echo "No proper submission file"
        exit
    fi
}

function set_vars()
{
    if [ $ROW_NUM -le ${TOT_ROWS} ]; then

        # read 3rd field in the i th row of calibtheta.txt
        theta[ROW_NUM]=`awk 'NR=='$ROW_NUM'{print $3}' calibtheta.txt`
        z0[ROW_NUM]=`awk 'NR=='$ROW_NUM'{print $8}' calibtheta.txt`
        y0[ROW_NUM]=`awk 'NR=='$ROW_NUM'{print $9}' calibtheta.txt`

        MACRO_THETA_ROW=`awk '/\/DRsim\/generator\/theta/{ print NR; exit }' ${MACRO}`
        MACRO_Z0_ROW=`awk '/\/DRsim\/generator\/z0/{ print NR; exit }' ${MACRO}`
        MACRO_Y0_ROW=`awk '/\/DRsim\/generator\/y0/{ print NR; exit }' ${MACRO}`

        sed -i "${MACRO_THETA_ROW}s/-.*/-${theta[ROW_NUM]}/" ${MACRO}
        sed -i "${MACRO_Z0_ROW}s/-.*/-${z0[ROW_NUM]}/" ${MACRO}
        sed -i "${MACRO_Y0_ROW}s/-.*/-${y0[ROW_NUM]}/" ${MACRO}

        if [ $BATCH_SYS = "HTCondor" ]; then
            INITIAL_DIR_ROW=`awk '/initialdir/{ print NR; exit }' condor.sub`
            sed -i "${INITIAL_DIR_ROW}s/=.*/= $TOWER_NUM/" condor.sub

            TRANSFER_INPUT_ROW=`awk '/transfer_input_files/{ print NR; exit }' condor.sub`
            sed -i "${TRANSFER_INPUT_ROW}s?=.*?= ${CEPC_BUILD_DIR}/CMakeFiles, ${CEPC_BUILD_DIR}/CMakeCache.txt, ${CEPC_BUILD_DIR}/HTCondor_env.sh, ${CEPC_BUILD_DIR}/condor_exe.sh, ${CEPC_BUILD_DIR}/init.mac, ${CEPC_BUILD_DIR}/run_ele.mac, ${CEPC_BUILD_DIR}/run_pi.mac, ${CEPC_BUILD_DIR}/run_calib.mac, ${CEPC_BUILD_DIR}/DRsim?" condor.sub

            #SOURCE_ROW=`awk '/source/{ print NR; exit }' condor_exe.sh`
            #sed -i "${SOURCE_ROW}s?.*?source ${CEPC_BUILD_DIR}/HTCondor_env.sh?" condor_exe.sh

            EXE_ROW=`awk '/DRsim/{ print NR; exit }' condor_exe.sh`
            sed -i "${EXE_ROW}s?.*?\./DRsim ${MACRO} \$1?" condor_exe.sh

            submit

        elif [ $BATCH_SYS = "Torque" ]; then
            QSUB_ROW=`awk '/qsub/{ print NR; exit }' torque_sub.sh`
            sed -i "${QSUB_ROW}s?qsub.*?qsub -q large -o ${CEPC_BUILD_DIR}/${TOWER_NUM}/log -e ${CEPC_BUILD_DIR}/${TOWER_NUM}/log -F \"\$i\" ${CEPC_BUILD_DIR}/torque_exe.sh?" torque_sub.sh

            SOURCE_ROW=`awk '/source/{ print NR; exit }' torque_exe.sh`
            sed -i "${SOURCE_ROW}s?.*?source ${CEPC_BUILD_DIR}/Torque_env.sh?" torque_exe.sh

            CD_ROW=`awk '/cd /{ print NR; exit }' torque_exe.sh`
            sed -i "${CD_ROW}s?.*?cd ${CEPC_BUILD_DIR}/${TOWER_NUM}?" torque_exe.sh

            EXE_ROW=`awk '/DRsim/{ print NR; exit }' torque_exe.sh`
            sed -i "${EXE_ROW}s?.*?${CEPC_BUILD_DIR}/DRsim ${CEPC_BUILD_DIR}/${MACRO} \$1?" torque_exe.sh

            submit
        else
            echo "No proper submission file"
        fi
    else
        echo "Wrong tower number"
        exit
    fi
}

if [ $PARTICLE = "Electron" ]; then
    MACRO="run_ele.mac"
    set_vars
elif [ $PARTICLE = "Pion" ]; then
    MACRO="run_pi.mac"
    set_vars
elif [ $PARTICLE = "Calib" ]; then
      MACRO="run_calib.mac"
      set_vars
else
    echo $PARTICLE is an invalid particle
    exit
fi

echo -e "\nUsing $MACRO macro file"
