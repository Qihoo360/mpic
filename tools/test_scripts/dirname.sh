#! /bin/sh
#
# file: dir_name.sh
# auth: mymtom
# date: 2008-02-27

x=`echo $0 | grep "^/"`
if test "${x}"; then
        dirname $0
else
        dirname `pwd`/$0
fi
