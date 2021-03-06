// Targeted by JavaCPP version 1.3: DO NOT EDIT THIS FILE

package org.bytedeco.javacpp;

import java.nio.*;
import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

import static org.bytedeco.javacpp.opencv_core.*;
import static org.bytedeco.javacpp.opencv_imgproc.*;

public class eventdetect extends org.bytedeco.javacpp.presets.ED {
    static { Loader.load(); }

// Parsed from eventDetector.h

// #ifndef SHARED_ED_H__
// #define SHARED_ED_H__


// #include "opencv2/imgproc.hpp"

public static class ED extends Pointer {
    static { Loader.load(); }
    /** Pointer cast constructor. Invokes {@link Pointer#Pointer(Pointer)}. */
    public ED(Pointer p) { super(p); }
    /** Native array allocator. Access with {@link Pointer#position(long)}. */
    public ED(long size) { super((Pointer)null); allocateArray(size); }
    private native void allocateArray(long size);
    @Override public ED position(long position) {
        return (ED)super.position(position);
    }

        public ED() { super((Pointer)null); allocate(); }
        private native void allocate();

        public native @StdString BytePointer version();
        public native int detectEvent( @ByRef MatVector frames, @StdString BytePointer identifier, @Cast("bool") boolean saveOutput);
        public native int detectEvent( @ByRef MatVector frames, @StdString String identifier, @Cast("bool") boolean saveOutput);
        public native int detectFromFile(@StdString BytePointer filename, @StdString BytePointer identifier, @Cast("bool") boolean saveOutput);
        public native int detectFromFile(@StdString String filename, @StdString String identifier, @Cast("bool") boolean saveOutput);
        public native int detectFromFileWithMask(@StdString BytePointer filename, @StdString BytePointer maskfilename, @StdString BytePointer identifier, @Cast("bool") boolean saveOutput);
        public native int detectFromFileWithMask(@StdString String filename, @StdString String maskfilename, @StdString String identifier, @Cast("bool") boolean saveOutput);
}

// #endif


}
