#!/bin/bash

ulimit -s unlimited
ulimit -c unlimited

if [ $# -lt 1 ] ; then
    echo "Usage: perf.sh -e <event0,event1...> <bench0 bench1 ...>"
    echo "Example: perf.sh -e branch,misses 400.0 401"
    exit 0
fi

events=

while getopts "e:" option; do
    case $option in
        e)  events=$OPTARG
            ;;
    esac
done

# Skip -e <events>
shift 2

list="
400.perlbench 401.bzip2 403.gcc 410.bwaves 416.gamess 429.mcf
433.milc 434.zeusmp 435.gromacs 436.cactusADM 437.leslie3d 444.namd
445.gobmk 447.dealII 450.soplex 453.povray 454.calculix 456.hmmer 458.sjeng
459.GemsFDTD 462.libquantum 464.h264ref 465.tonto 470.lbm 471.omnetpp 473.astar
481.wrf 482.sphinx3 483.xalancbmk
"
prefix=benchspec/CPU2006
rundir=run_base_ref_gcc720.Of.0002

mkdir -p perf_out

for bench in $* ; do
    if [ $bench = "all" ] ; then
        for item in $list ; do
            #./myrun.sh $item &
            #sleep 7s
            #kill -9 -$(pgrep runspec)
            echo "AA"
        done
        exit 0
    fi
    name=$(echo $bench | awk -F. '{print $1;}')
    num=$(echo $bench  | awk -F. '{print $2;}')
    if [ -z "$num" ] ; then
        num=0   # default
    fi
    target=$(echo $prefix/$name*)
    output=perf.$events.$name.$num.data
    pushd $target/run/$rundir > /dev/null
    line=$(cat ./speccmds.cmd | sed -n "$(($num+2))p")
    if [ $name = 445 -o $name = 416 -o $name = 433 -o $name = 437 ] ; then
        exe=$(echo "$line" | cut -d ' ' -f 7-)
        cmd="perf record -e $events -o $output $exe < $(echo $line | cut -d ' ' -f 2) > /dev/null 2>err.$num"
    elif [ $name = 410 -o $name = 435 ] ; then
        exe=$(echo "$line" | cut -d ' ' -f 3-)
        cmd="perf record -e $events -o $output $exe > /dev/null 2>err.$num"
    else
        exe=$(echo "$line" | cut -d ' ' -f 5-)
        cmd="perf record -e $events -o $output $exe > /dev/null 2>err.$num"
    fi
    echo "In $target/run/$rundir"
    echo "$cmd"
    eval $cmd
    popd > /dev/null
    mv $target/run/$rundir/$output perf_out/
done
