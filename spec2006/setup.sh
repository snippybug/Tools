#!/bin/bash

if [ $# -lt 1 ] ; then
    echo "Usage: setup <benchmarks...>"
    exit 0
fi

list="
400.perlbench 401.bzip2 403.gcc 410.bwaves 416.gamess 429.mcf
433.milc 434.zeusmp 435.gromacs 436.cactusADM 437.leslie3d 444.namd
445.gobmk 447.dealII 450.soplex 453.povray 454.calculix 456.hmmer 458.sjeng
459.GemsFDTD 462.libquantum 464.h264ref 465.tonto 470.lbm 471.omnetpp 473.astar
481.wrf 482.sphinx3 483.xalancbmk
"
prefix=benchspec/CPU2006

for bench in $* ; do
    if [ $bench = "all" ] ; then
        for item in $list ; do
            ./myrun.sh $item &
            sleep 7s
            kill -9 -$(pgrep runspec)
        done
        exit 0
    fi
    ./myrun.sh $bench &
    sleep 7s
    kill -9 -$(pgrep runspec)
done
