#!/bin/bash

TRACE=/data/alan/traces/speccpu/602.gcc_s-1850B.champsimtrace.xz

cd ../dramsim3
DRAMSIM3_PATH=`pwd`
mkdir -p build
cd build
cmake ..
make -j8
export LD_LIBRARY_PATH=$DRAMSIM3_PATH:$LD_LIBRARY_PATH
cd ../../champsim

./config.py configs/timeliness/prefetch/inldnl_nl_no.json
make -j8
cd bin/timeliness/prefetch

./inldnl_nl_no --simulation_instruction=100000 \
--warmup_instructions=1000000 \
--traces $TRACE | tee test_output.txt

if cat test_output.txt | grep -q "ChampSim completed all CPUs"; then
    echo "SETUP TESTED SUCCESSFULLY"
else
    echo "SETUP TEST FAILED"
fi
