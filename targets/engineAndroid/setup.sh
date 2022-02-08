#!/bin/bash

# We will assume that the Android SDK is in the default location that Android Studio installs it to.
ANDROID_SDK="/home/borja/Android/Sdk"
echo "Using Android SDK at: $ANDROID_SDK"

# We will assume that we'll use Java that is bundled with Android Studio.
export JAVA_HOME="/home/borja/android-studio/jre/jdk1.8.0_321"

# We will be using a specific version of the Android NDK.
NDK_VERSION="20.1.5948944"
ANDROID_NDK="$ANDROID_SDK/ndk/$NDK_VERSION"
echo "Using Android NDK at: $ANDROID_NDK"

# This command will automatically answer 'yes' to each of the licences that a user normally has to manually agree to.
echo "Auto accepting Android SDK licenses ..."
yes | $ANDROID_SDK/tools/bin/sdkmanager --licenses

# This is the collection of Android SDK components that our code base will need.
echo "Installing required Android SDK components ..."
$ANDROID_SDK/tools/bin/sdkmanager \
    "platform-tools" \
    "build-tools;30.0.3" \
    "tools" \
    "platforms;android-28" \
    "cmake;3.18.1" \
    "ndk;$NDK_VERSION"
