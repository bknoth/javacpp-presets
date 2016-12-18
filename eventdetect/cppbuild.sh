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
    cp -r /home/mosaiq/src/cpp/eventdetect/* .
    chmod +x build.sh
fi

case $PLATFORM in
    linux-x86_64)
        ./build.sh
	sudo mkdir -p /usr/local/include/eventdetect
	sudo mkdir -p /usr/local/lib/eventdetect
	sudo cp *.h /usr/local/include/eventdetect
	sudo cp *.so /usr/local/lib/eventdetect
        ;;
    *)
        echo "Error: Platform \"$PLATFORM\" is not supported"
        ;;
esac

cd ../..
