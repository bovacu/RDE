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
ar -x ../vcpkg/installed/x64-linux/lib/libyaml-cpp.a 
ar -x libGDE.a 
rm libGDE.a
ar -qc libGDE.a *.o
rm *.o
echo "Post processing finished!"