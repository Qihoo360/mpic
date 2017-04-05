
#!/bin/sh 

set -x

PWD=`sh dirname.sh`

cd $PWD

touch $PWD/mpic.conf

CMD="$PWD/../../build/bin/examples_mpic_framework -n op -p 20 --log_dir=$PWD --cfg=$PWD/mpic.conf --pid=$PWD/mpic.pid --mod=$PWD/../../build/lib/libexamples_module_echo.so"

$CMD
sleep 1
PROCESS_COUNT=`ps aux|grep examples_mpic_framework|grep -v grep|grep -v vim |grep -v examples_mpic_framework.sh |wc -l`
echo "proc count=$PROCESS_COUNT"
if [ $PROCESS_COUNT -ne 21 ]; then
    echo "examples_mpic_framework process started failed!"
    exit 1;
fi

$CMD -r
sleep 1
$CMD -r
sleep 1

# kill worker process directly
ps aux|grep 'examples_mpic_framework(op): worker process' |grep -v grep|awk '{print $2}'|xargs kill
sleep 1

$CMD -r
sleep 1

if [ $PROCESS_COUNT -ne 21 ]; then
    echo "examples_mpic_framework process reload failed!"
    exit 2;
fi

$CMD -k
sleep 5
    
PROCESS_COUNT=`ps aux|grep examples_mpic_framework|grep -v grep|grep -v vim |grep -v examples_mpic_framework.sh |wc -l`
if [ $PROCESS_COUNT -ne 0 ]; then
    echo "examples_mpic_framework process cannot not stopped!"
    exit 3;
fi


rm *.log.* *.ERROR *.INFO *.WARNING
