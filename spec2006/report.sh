#!/bin/bash

if [ $# -lt 1 ] ; then
    echo "Usage: report.sh -e <event0,event1...> <benchmark>"
    echo "Example: report.sh -e branch,misses 400.0"
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

name=$(echo $1 | awk -F. '{print $1;}')                                    
num=$(echo $1 | awk -F. '{print $2;}')                                     
if [ -z "$num" ] ; then                                                    
    num=0                                                                  
fi                                                                         

data_file=perf_out_old/perf.$events.$name.$num.data
data_file_new=perf_out/perf.$events.$name.$num.data
rpt_file=perf_rpt/perf.$events.$name.$num.rpt
if [ -f $data_file ] ; then
    #~/linux-3.10/tools/perf/perf report --objdump=/opt/gcc-7.3.0-lasx-rc4.3/bin/objdump  -i $data_file
    ~/linux-3.10/tools/perf/perf report --objdump="/opt/gcc-7.3.0-lasx-rc4.3/bin/objdump -Mloongson-ext -Mloongson-ext2 -Mloongson-ext3 -Mmsa2 -Mloongson-asx -Mloongson-sx" -i $data_file
elif [ -f $data_file_new ] ; then
    #~/linux-3.10/tools/perf/perf report --objdump=/opt/gcc-7.3.0-lasx-rc4.3/bin/objdump  -i $data_file
    ~/linux-3.10/tools/perf/perf report --objdump="/opt/gcc-7.3.0-lasx-rc4.3/bin/objdump -Mloongson-ext -Mloongson-ext2 -Mloongson-ext3 -Mmsa2 -Mloongson-asx -Mloongson-sx" -i $data_file_new
elif [ -f $rpt_file ] ; then
    less $rpt_file
else
    echo "Error: file perf.$events.$name.$num.(data|rpt) does not exist"
fi
