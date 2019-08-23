#!/bin/bash

if [ $# -lt 1 ] ; then
    echo "Usage: convert.sh file0 file1 ..."
    exit
fi

output_dir=perf_rpt

mkdir -p $output_dir

for file in $* ; do
    data_name=$(basename $file)
    txt_name=${data_name/data/rpt}
    echo "Handle $file"
    perf report -i $file > $output_dir/$txt_name
done
