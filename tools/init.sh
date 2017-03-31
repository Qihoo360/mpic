
set -x
TOOLSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
THIRTPARTYDIR=$TOOLSDIR/../3rdparty

cd $TOOLSDIR


#######################################################
# initialize
if test -f initialized.txt;then 
    echo "Already initialized."
    exit 0
fi

#######################################################
# libdaemon
cd $THIRTPARTYDIR
pwd
wget "http://0pointer.de/lennart/projects/libdaemon/libdaemon-0.14.tar.gz"
tar zxvf libdaemon-0.14.tar.gz
rm -f libdaemon-0.14.tar.gz
cd $TOOLSDIR

######################################################
# glog
git clone -b v0.3.4  --depth 1  https://github.com/google/glog.git
mv glog $THIRTPARTYDIR/glog-0.3.4


echo -n "Initialized time : " > initialized.txt
echo `date` >> initialized.txt
