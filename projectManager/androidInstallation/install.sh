#ANDROID_SDK_VAR=$(sed -n -e '/^ANDROID_SDK=/p' assets/data.config)
#
#if [ -z "$ANDROID_SDK_VAR" ]
#then
#    echo "ANDROID_SDK_VAR=$1" >> assets/data.config
#fi
#
#ANDROID_SDK_ARR=$(${ANDROID_SDK_VAR//=/ })
#ANDROID_SDK_PATH=${ANDROID_SDK_ARR[1]}

DIR="$PWD"

ANDROID_STUDIO_PATH="$1"
ANDROID_SDK_PATH="$2"
ANDROID_NDK_PATH="$3"
JDK_8_PATH="$4"

echo "AS -> $ANDROID_STUDIO_PATH"
echo "SDK -> $ANDROID_SDK_PATH"
echo "NDK -> $ANDROID_NDK_PATH"
echo "JDK -> $JDK_8_PATH"

#JDK_8_VAR=$(sed -n -e '/^JDK_8=/p' data.config)
#JDK_8_ARR=$(${JDK_8_VAR//=/ })
#JDK_8_PATH=${JDK_8_ARR[1]}

cd "$ANDROID_SDK_PATH"/tools/bin
export JAVA_HOME="$JDK_8_PATH"
echo "JAVA_HOME -> $JAVA_HOME"

touch ~/.android/repositories.cfg
touch "$DIR"/assets/androidInstallationLogs.txt

echo "" > "$DIR"/assets/androidInstallationLogs.txt 2>&1

./sdkmanager --install "platforms;android-28" >> "$DIR"/assets/androidInstallationLogs.txt 2>&1
if [ "$?" != 0 ]
then
    exit 1
fi

echo "platforms 28 installed"

./sdkmanager --install "build-tools;28.0.3" >> "$DIR"/assets/androidInstallationLogs.txt 2>&1
if [ "$?" != 0 ]
then
    exit 2
fi

echo "build tools installed"

./sdkmanager --install "cmake;3.6.4111459" >> "$DIR"/assets/androidInstallationLogs.txt 2>&1
if [ "$?" != 0 ]
then
    exit 3
fi

echo "cmake installed"

./sdkmanager --install "cmdline-tools;latest" >> "$DIR"/assets/androidInstallationLogs.txt 2>&1
if [ "$?" != 0 ]
then
    exit 4
fi

echo "cmdline tools installed"

yes | ./sdkmanager --licenses >> "$DIR"/assets/androidInstallationLogs.txt 2>&1
if [ "$?" != 0 ]
then
    exit 5
fi

echo "All licenses agreed installed"

{
    echo ""
    echo "ANDROID_SDK_PATH=$ANDROID_SDK_PATH"
    echo "ANDROID_NDK_PATH=$ANDROID_NDK_PATH"
    echo "ANDROID_STUDIO_PATH=$ANDROID_STUDIO_PATH"
    echo "JDK_8_PATH=$JDK_8_PATH"
} >> "$DIR"/assets/data.config

exit 0