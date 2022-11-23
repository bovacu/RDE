BUILD_TYPE=$1
if [ -z "$BUILD_TYPE" ]
then
    echo "You need to specify release or debug"
    exit 0
fi
cd build && cmake -G "Ninja" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_TOOLCHAIN_FILE=/home/borja/Documents/RDE/vcpkg/scripts/buildsystems/vcpkg.cmake .. && cmake --build . && ./RDE
