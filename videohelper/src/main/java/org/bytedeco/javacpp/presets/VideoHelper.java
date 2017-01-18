package org.bytedeco.javacpp.presets;

import org.bytedeco.javacpp.annotation.*;
import org.bytedeco.javacpp.tools.*;

@Properties(target="org.bytedeco.javacpp.videohelper", value={
    @Platform(value="linux", include="videoHelper.h", link="vh", includepath = "/usr/local/include/videohelper/", linkpath = "/usr/local/lib/videohelper/" )
}, inherit = opencv_imgproc.class)
public class VideoHelper implements InfoMapper {
    public void map(InfoMap infoMap) {
        infoMap.put(new Info("FAR").cppText("#define FAR"))
               .put(new Info("OF").cppText("#define OF(args) args"))
               .put(new Info("_IplImage").pointerTypes("IplImage"))
               .put(new Info("_IplTileInfo").pointerTypes("IplTileInfo"))
               .put(new Info("IplImage").base("AbstractIplImage"))
               .put(new Info("Byte", "Bytef", "charf").cast().valueTypes("byte").pointerTypes("BytePointer"))
               .put(new Info("uInt", "uIntf").cast().valueTypes("int").pointerTypes("IntPointer"))
               .put(new Info("uLong", "uLongf", "z_crc_t", "z_off_t").cast().valueTypes("long").pointerTypes("CLongPointer"))
               .put(new Info("z_off64_t").cast().valueTypes("long").pointerTypes("LongPointer"))
               .put(new Info("voidp", "voidpc", "voidpf").valueTypes("Pointer"));
    }
}

