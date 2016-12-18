g++ -c  eventDetector.cpp -fpic -std=c++11 `pkg-config --cflags --libs /home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/opencv-3.1.0/unix-install/opencv.pc`
g++ -shared -o libed.so eventDetector.o  -std=c++11 `pkg-config --cflags --libs /home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/opencv-3.1.0/unix-install/opencv.pc`
