#!/bin/bash

if [ $# -lt 1 ] ; then
    echo "Usage: open.sh -e <event0,event1...> <benchmark>"
    echo "Example: open.sh -e branch,misses 400.0"
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
                                                                           
vi perf_rpt/perf.$events.$name.$num.rpt
