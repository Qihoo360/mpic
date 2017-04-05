
#!/bin/sh 

ln -fs /home/s/safe/bin/dirname.sh
PWD=`sh dirname.sh`

CMD="./monitor_worker_test -n op --log-dir=$PWD --cfg-dir=$PWD/../etc --pid-dir=$PWD"

$CMD
sleep 1
PROCESS_COUNT=`ps aux|grep monitor_worker_test|grep -v grep|grep -v vim |grep -v monitor_worker_test.sh |wc -l`
echo "proc count=$PROCESS_COUNT"
if [ $PROCESS_COUNT -ne 2 ]; then
    echo "monitor_worker_test process started failed!"
    exit 1;
fi

$CMD -r
sleep 1
$CMD -r
sleep 1

# kill worker process directly
ps aux|grep 'monitor_worker_test(op): worker process' |grep -v grep|awk '{print $2}'|xargs kill
sleep 1

$CMD -r
sleep 1

if [ $PROCESS_COUNT -ne 2 ]; then
    echo "monitor_worker_test process reload failed!"
    exit 2;
fi

$CMD -k
sleep 1
    
PROCESS_COUNT=`ps aux|grep monitor_worker_test|grep -v grep|grep -v vim |grep -v monitor_worker_test.sh |wc -l`
if [ $PROCESS_COUNT -ne 0 ]; then
    echo "monitor_worker_test process cannot not stopped!"
    exit 3;
fi

rm -rf dirname.sh
