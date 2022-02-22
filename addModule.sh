MODULE="$1"
BASE_DIR="$PWD"
if [ -z "$MODULE" ]
then
    echo "Must select a module to add: ANDROID, IOS, FIREBASE"
    exit 0
fi

function android() {
    if [[ -d "targets/android" ]]
      then
          echo "Module already configured"
          exit 0
    fi
    cd targets && git clone https://github.com/bovacu/GameEngineAndroid && mv GameEngineAndroid android
    cd android/sdl/jni && ./buildAndroidLibs.sh
}

function firebase() {
    cd "$BASE_DIR"
    if [[ -d "targets/GDEFirebase" ]]
        then
            echo "Module already configured"
            exit 0
        fi

    cd targets
    git submodule add https://github.com/bovacu/GDEFirebase
    git submodule init
    git submodule update
    cd GDEFirebase && ./setup.sh
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

