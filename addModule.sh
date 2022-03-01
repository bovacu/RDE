MODULE="$1"
BASE_DIR="$PWD"
if [ -z "$MODULE" ]
then
    echo "Must select a module to add: ANDROID, IOS, FIREBASE"
    exit 0
fi

function android() {
#   TODO install JDK 8 sudo --askpass apt install openjdk-8-jre-headless or https://www.oracle.com/br/java/technologies/javase/javase8-archive-downloads.html
#   TODO download android-studio  https://redirector.gvt1.com/edgedl/android/studio/ide-zips/2021.1.1.22/android-studio-2021.1.1.22-linux.tar.gz
#   TODO NDK 20.1.5948944 https://androidsdkoffline.blogspot.com/p/android-ndk-side-by-side-direct-download.html
    if [[ -d "targets/android" ]]
        then
          echo "Module already configured"
          exit 0
    fi
    mkdir targets
    cd targets && git clone https://github.com/bovacu/GDEAndroid
    cd android/sdl/jni && ./buildAndroidLibs.sh
    cd "$BASE_DIR"
}

function firebase() {
    cd "$BASE_DIR"
    if [[ -d "targets/GDEFirebase" ]]
        then
            echo "Module already configured"
            exit 0
        fi

    mkdir targets
    cd targets && git submodule add https://github.com/bovacu/GDEFirebase
    cd GDEFirebase && ./setup.sh
    cd "$BASE_DIR"
}

if [ "ANDROID" == "$MODULE" ]
then
    android
elif [ "IOS" == "$MODULE" ]
then
    echo "IOS not supported yet"
elif [ "FIREBASE" == "$MODULE" ]
then
    firebase
fi

