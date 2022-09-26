echo "Starting post processing..."
cd build
ar -x ../vcpkg/installed/x64-linux/lib/libSDL2.a 
ar -x ../vcpkg/installed/x64-linux/lib/libSDL2main.a 
ar -x ../vcpkg/installed/x64-linux/lib/libSDL2_image.a 
ar -x ../vcpkg/installed/x64-linux/lib/libSDL2_mixer.a 
ar -x ../vcpkg/installed/x64-linux/lib/libbox2d.a 
ar -x ../vcpkg/installed/x64-linux/lib/libfreetype.a 
ar -x ../vcpkg/installed/x64-linux/lib/libglad.a 
ar -x ../vcpkg/installed/x64-linux/lib/libimgui.a
ar -x ../vcpkg/installed/x64-linux/lib/libpng.a
ar -x ../vcpkg/installed/x64-linux/lib/libpng16.a
ar -x ../vcpkg/installed/x64-linux/lib/libjpeg.a
ar -x ../vcpkg/installed/x64-linux/lib/libmpg123.a
ar -x ../vcpkg/installed/x64-linux/lib/libogg.a
ar -x ../vcpkg/installed/x64-linux/lib/libout123.a
ar -x ../vcpkg/installed/x64-linux/lib/libturbojpeg.a
ar -x ../vcpkg/installed/x64-linux/lib/libvorbis.a
ar -x ../vcpkg/installed/x64-linux/lib/libvorbisenc.a
ar -x ../vcpkg/installed/x64-linux/lib/libvorbisfile.a
ar -x ../vcpkg/installed/x64-linux/lib/libz.a
ar -x libGDE.a
rm libGDE.a
ar -qc libGDE.a *.o
rm *.o
echo "Post processing finished!"