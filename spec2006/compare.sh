#!/bin/bash

if [ $# -ne 2 ] ; then
    echo "Usage: compare.sh ref_dir obj_dir"
    exit
fi

ref_dir=$1
obj_dir=$2

for file in $obj_dir/* ; do
    file=$(basename $file)
    if [ -f $ref_dir/$file ] ; then
        echo $file
        ./compare.pl $ref_dir/$file $obj_dir/$file 2>/dev/null
    fi
done
