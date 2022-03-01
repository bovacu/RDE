PROJECT_DIR="$1"
NDK_DIR="$2"
GDE_PATH="$3"
cd "$PROJECT_DIR"

if [[ -d "targets/GDEAndroid" ]]
then
    echo "Module already configured"
    exit 0
fi

mkdir targets

ln -s "$GDE_PATH"/submodules/freetype freetype
ln -s "$GDE_PATH"/submodules/glm glm

cd targets && git clone https://github.com/bovacu/GDEAndroid
cd GDEAndroid/sdl/jni
"$NDK_DIR"/ndk-build APP_PLATFORM=android-21 NDK_DEBUG=1 NDK_OUT="$PROJECT_DIR"/targets/GDEAndroid/sdl/build/intermediates/ndkBuild/debug/object NDK_LIBS_OUT="$PROJECT_DIR"/targets/GDEAndroid/sdl/build/intermediates/ndkBuild/debug/lib