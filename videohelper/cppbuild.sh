#!/bin/bash
# This file is meant to be included by the parent cppbuild.sh script
if [[ -z "$PLATFORM" ]]; then
    pushd ..
    bash cppbuild.sh "$@" eventdetect
    popd
    exit
fi

if [[ $PLATFORM == windows* ]]; then
    mkdir -p $PLATFORM
    cd $PLATFORM
else
    mkdir -p $PLATFORM
    cd $PLATFORM
    cp -r ../../src/main/cpp/* .
    chmod +x build-all-thread.sh
    wget https://github.com/bknoth/jsoncpp/archive/master.zip
    unzip master.zip
    rm master.zip
fi

case $PLATFORM in
    linux-x86_64)
        cd jsoncpp-master
        python amalgamate.py
        cp -r dist/* ..
        cd ..
        rm -rf jsoncpp-master
        ./build-all-thread.sh
	sudo mkdir -p /usr/local/include/videohelper
	sudo mkdir -p /usr/local/lib/videohelper
	sudo cp *.h /usr/local/include/videohelper
	sudo cp *.so /usr/local/lib/videohelper
        ;;
    *)
        echo "Error: Platform \"$PLATFORM\" is not supported"
        ;;
esac

cd ../..
