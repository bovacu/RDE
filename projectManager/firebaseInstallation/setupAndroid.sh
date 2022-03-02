#!/bin/bash

PROJECT_PATH="$1"
FIREBASE_PATH="$2"
PACKAGE="$3"
GDE_PATH="$4"

cd "$PROJECT_PATH"/targets/GDEAndroid
sed -i -e "s|#GDEFirebase|include($GDE_PATH/modules/GDEFirebase/CMakeLists.txt)|g" CMakeLists.txt
sed -i -e 's|compileSdkVersion: [0-9][0-9],|compileSdkVersion: 32,|g' build.gradle
sed -i -e "s|// *GOOGLE_SERVICES|\t\tclasspath 'com.google.gms:google-services:4.3.10'|g" build.gradle
sed -i -e "s|// *FIREBASE|def firebase_cpp_sdk_dir = System.getProperty('firebase_cpp_sdk.dir')\ngradle.ext.firebase_cpp_sdk_dir = \"\$firebase_cpp_sdk_dir\"\nincludeBuild \"\$firebase_cpp_sdk_dir\"|g" settings.gradle
ln -s "$FIREBASE_PATH"/firebase_cpp_sdk .

if ! grep -q "systemProp.firebase_cpp_sdk.dir=$FIREBASE_PATH/firebase_cpp_sdk" "gradle.properties"; then
    printf "\n" >> gradle.properties
    echo "systemProp.firebase_cpp_sdk.dir=$FIREBASE_PATH/firebase_cpp_sdk" >> gradle.properties
fi

cd app
sed -i -e "s|// *FIREBASE_APPLY_GOOGLE_SERVICES|apply plugin: 'com.google.gms.google-services'|g" build.gradle
sed -i -e "s|// *FIREBASE_DEPENDENCIES|\timplementation platform('com.google.firebase:firebase-bom:29.1.0')\n\timplementation 'com.google.firebase:firebase-analytics'\n\timplementation 'com.google.firebase:firebase-auth:21.0.1'\n\timplementation 'com.google.android.gms:play-services-ads:19.0.0'|g" build.gradle
sed -i -e "s|arguments \"-DANDROID_STL=c++_shared\", \"-DAPP_PLATFORM=android-21\"|arguments \"-DANDROID_STL=c++_shared\", \"-DAPP_PLATFORM=android-21 -DFIREBASE_CPP_SDK_DIR=\$gradle.firebase_cpp_sdk_dir\"|g" build.gradle
sed -i -e "s|// *FIREBASE_APPLY_AND_DEPENDENCIES|apply from: \"\$gradle.firebase_cpp_sdk_dir/Android/firebase_dependencies.gradle\"\nfirebaseCpp.dependencies { \n\tanalytics\n\tauth\n\tdatabase\n\tadmob\n}|g" build.gradle
cd ..

PACKAGE_PATH="app/src/main/java"
IFS='.'
read -a strarr <<< $PACKAGE
for val in "${strarr[@]}";
do
  PACKAGE_PATH="${PACKAGE_PATH}/${val}"
done

echo "$PACKAGE_PATH"

cd "$PACKAGE_PATH"
sed -i -e "s|//import com.google.android.gms.ads.MobileAds;|import com.google.android.gms.ads.MobileAds;|g" MainActivity.java
sed -i -e "s|// *FIREBASE_INIT|\t\tinitFireBaseAdds();|g" MainActivity.java
sed -i -e "s|// *FIREBASE_DEFINE|\t@Override\n\tprotected void finalize() throws Throwable {\n\t\tsuper.finalize();\n\t\tdestroy();\n\t}\n\n\tpublic native void initFireBaseAdds();\n\tpublic native void destroy();\n|g" MainActivity.java