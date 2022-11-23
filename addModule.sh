#!/bin/bash

MODULE="$1"
BASE_DIR="$PWD"
if [ -z "$MODULE" ]
then
    echo "Must select a module to add: ANDROID, IOS, FIREBASE"
    exit 0
fi

function android {
    if [[ -d "targets/RDEAndroid" ]]
        then
          echo "Module already configured"
          exit 0
    fi
    mkdir targets
    cd targets && git clone https://github.com/bovacu/RDEAndroid
    cd RDEAndroid/sdl/jni && ./buildAndroidLibs.sh
    cd "$BASE_DIR"
}

function firebase {
    cd "$BASE_DIR"
    if [[ -d "targets/RDEFirebase" ]]
        then
            echo "Module already configured"
            exit 0
        fi

    mkdir targets
    cd targets && git submodule add https://github.com/bovacu/RDEFirebase
    cd RDEFirebase && ./setup.sh
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

