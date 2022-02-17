MODULE="$1"
if [ -z "$MODULE" ]
then
    echo "Must select a module to add: ANDROID, IOS"
    exit 0
fi

if [ "ANDROID" == "$MODULE" ]
then
    if [[ -d "targets/android" ]]
    then
        echo "Module already configured"
        exit 0
    fi

    cd targets && git clone https://github.com/bovacu/GameEngineAndroid && mv GameEngineAndroid android
    cd android/sdl/jni && ./buildAndroidLibs.sh
elif [ "IOS" == "$MODULE" ]
then
    echo "IOS not supported yet"
else
    echo "$MODULE" is not a supported module, modules are [ANDROID, IOS]
fi



