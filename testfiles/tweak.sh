#!/bin/bash

if [ $# -lt 1 ]
then
    echo "Syntax error: $0 <test_number>"
fi

src=$(( $1 ))
dst=$(( src + 1 ))
src_file=$(printf 'testfiles/test_%04d.depc' ${src})
dst_file=$(printf 'testfiles/test_%04d.depc' ${dst})
[ -f "${dst_file}" ] || cp ${src_file} ${dst_file}
vi -O ${src_file} ${dst_file}
