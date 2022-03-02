GDE_PATH="$1"
DIR="$PWD"
cd "$GDE_PATH"

if [ -d "modules/GDEFirebase" ]
then
    echo "Module already configured"
    exit 0
fi

mkdir modules

cd modules && git clone https://github.com/bovacu/GDEFirebase && cd GDEFirebase

if ! [ -d firebase_cpp_sdk ]
then
    sudo apt install wget
    wget https://firebase.google.com/download/cpp && unzip cpp && rm cpp
fi

echo "FIREBASE_SDK_PATH=$GDE_PATH/modules/GDEFirebase" >> "$DIR"/assets/data.config

cd "$GDE_PATH" && sed -i -e 's|#GDEFirebase|include(modules/GDEFirebase/CMakeLists.txt)|g' CMakeLists.txt

exit 0