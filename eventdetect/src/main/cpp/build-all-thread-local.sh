g++ -c  eventDetector.cpp -fpic -std=c++0x -pthread `pkg-config --cflags --libs /home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/opencv-3.1.0/unix-install/opencv.pc` -L/home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/share/OpenCV/3rdparty/lib
g++ -shared -o libed.so eventDetector.o  -std=c++0x -pthread `pkg-config --cflags --libs /home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/opencv-3.1.0/unix-install/opencv.pc` -L/home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/share/OpenCV/3rdparty/lib
g++ -o bin/ed ed.cpp -ggdb -std=c++0x -pthread `pkg-config --cflags --libs /home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/opencv-3.1.0/unix-install/opencv.pc`  -L. -L/home/mosaiq/src/java/javacpp-presets/opencv/cppbuild/linux-x86_64/share/OpenCV/3rdparty/lib -led
