echo "Starting post processing..."
WD=$1
LIB_OUT=$2
BUILD_TYPE=$3

mkdir -p build
mkdir -p build/"$3"
rm -f "$1"/build/"$3"/libGDE.a

cd $2
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libSDL2.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libSDL2main.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libSDL2_image.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libSDL2_mixer.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libbox2d.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libfreetype.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libglad.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libimgui.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libpng.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libpng16.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libjpeg.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libmpg123.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libogg.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libout123.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libturbojpeg.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libvorbis.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libvorbisenc.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libvorbisfile.a
ar -x "$WD"/vcpkg/installed/x64-linux/lib/libz.a
ar -x libGDE.a
rm libGDE.a
ar -qc libGDE.a *.o
rm *.o
cp libGDE.a "$WD"/build/"$3"
echo "Post processing finished!"