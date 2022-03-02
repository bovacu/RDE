#!/bin/bash

PROJECT_PATH="$1"
APK_NAME="$2"

echo "" > "$PROJECT_PATH"/targets/GDEAndroid/build.logs

(cd "$PROJECT_PATH"/targets/GDEAndroid && touch build.logs && ./gradlew :app:assembleDebug && cd app/build/outputs/apk/debug) >> "$PROJECT_PATH"/targets/GDEAndroid/build.logs 2>&1
if [ "$APK_NAME" != "app-debug" ]
then
    mv "$PROJECT_PATH"/targets/GDEAndroid/app/build/outputs/apk/debug/app-debug.apk "$PROJECT_PATH"/targets/GDEAndroid/app/build/outputs/apk/debug/"$APK_NAME".apk
fi