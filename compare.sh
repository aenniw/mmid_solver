#!/usr/bin/env bash
# Compare computation speed of of different approaches

cmake ./CMakeLists.txt && make || exit 1

MMID_CL=./mmid_OpenCL/bin/mmid_OpenCL
MMID_MP=./mmid_OpenMP/bin/mmid_OpenMP
MMID_CL_LOG=./mmid_OpenCL/bin/mmid_OpenCL.log
MMID_MP_LOG=./mmid_OpenMP/bin/mmid_OpenMP.log

for SAMPLES in `seq 5000 5000 20000`;do
    for INTERACTIONS in `seq 50 50 100`;do
        expect -c "set timeout -1; spawn ${MMID_MP}; expect \":\"; send \"${SAMPLES}\n\"; expect \":\"; send \"${INTERACTIONS}\n\"; expect \"elapsed=\"" > ${MMID_MP_LOG}
        MMID_MP_TIME=`cat -v ${MMID_MP_LOG} | grep -i "time elapsed="`; MMID_MP_TIME=${MMID_MP_TIME#*=*}; MMID_MP_TIME=${MMID_MP_TIME%*s*}

        expect -c "set timeout -1; spawn ${MMID_CL}; expect \":\"; send \"${SAMPLES}\n\"; expect \":\"; send \"${INTERACTIONS}\n\"; expect \"n]\"; send \"y\n\"; expect \"elapsed=\"" > ${MMID_CL_LOG}
        MMID_CL_GPU_TIME=`cat -v ${MMID_CL_LOG} | grep -i "time elapsed="`; MMID_CL_GPU_TIME=${MMID_CL_GPU_TIME#*=*}; MMID_CL_GPU_TIME=${MMID_CL_GPU_TIME%*s*}

        echo -e "MP ${MMID_MP_TIME}s\tCL ${MMID_CL_GPU_TIME}s\tINTERACTIONS=${INTERACTIONS};\tSAMPLES=${SAMPLES}"
    done
done
rm ${MMID_MP_LOG} ${MMID_CL_LOG}