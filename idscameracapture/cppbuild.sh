#!/bin/bash
# This file is meant to be included by the parent cppbuild.sh script
if [[ -z "$PLATFORM" ]]; then
    pushd ..
    bash cppbuild.sh "$@" idscamera
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
    mkdir -p bin
fi

case $PLATFORM in
    linux-x86_64)
        ./build-all-thread.sh
	sudo mkdir -p /usr/local/include/idscameracapture
	sudo mkdir -p /usr/local/lib/idscameracapture
	sudo cp *.h /usr/local/include/idscameracapture
	sudo cp *.so /usr/local/lib/idscameracapture
        ;;
    *)
        echo "Error: Platform \"$PLATFORM\" is not supported"
        ;;
esac

cd ../..
