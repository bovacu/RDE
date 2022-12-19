from sys import platform
import sys
import os

def windows(args):
    build_type = "debug"
    if len(args) > 2:
        build_type = args[2]

    generator = "Ninja"
    if len(args) > 3:
        generator = args[3]

    c_compiler = "clang"
    if len(args) > 4:
        generator = args[4]

    cxx_compiler = "clang++"
    if len(args) > 5:
        generator = args[5]

    create_build_folder = "if not exist build mkdir build"
    create_windows_folder = "if not exist build\\windows mkdir build\\windows"
    create_windows_build_folder = "if not exist build\\windows\\{} mkdir build\\windows\\{}".format( build_type.lower(), build_type.lower())
    
    cmake_generate = "cd build\\windows\\{_build_type} && cmake -G \"{_generator}\" -DCMAKE_BUILD_TYPE={_build_type} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER={_c_compiler} -DCMAKE_CXX_COMPILER={_cxx_compiler}  -DCMAKE_TOOLCHAIN_FILE=".format(_generator = generator, _build_type = build_type, _c_compiler = c_compiler, _cxx_compiler = cxx_compiler) + os.getcwd() + "/vcpkg/scripts/buildsystems/vcpkg.cmake ..\\..\\.."
    cmake_build = "cd build\\windows\\{} && cmake --build .".format(build_type.lower())
    
    delete_compile_commands = "del compile_commands.json"
    copy_compile_commands = "copy build\\windows\\{}\\compile_commands.json .".format(build_type.lower())

    run_command = "start build\\windows\\{}\\RDE.exe".format(build_type.lower())

    if args[1] == "compile":
        os.system(create_build_folder)
        os.system(create_windows_folder)
        os.system(create_windows_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
    elif args[1] == "run":
        os.system(run_command)
    elif args[1] == "compile-run":
        os.system(create_build_folder)
        os.system(create_windows_folder)
        os.system(create_windows_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
        os.system(run_command)


def linux(args):
    build_type = "debug"
    if len(args) > 2:
        build_type = args[2]

    generator = "Ninja"
    if len(args) > 3:
        generator = args[3]

    c_compiler = "clang"
    if len(args) > 4:
        generator = args[4]

    cxx_compiler = "clang++"
    if len(args) > 5:
        generator = args[5]

    create_build_folder = "mkdir -p build"
    create_linux_folder = "mkdir -p build/linux"
    create_linux_build_folder = "mkdir -p build/linux/{}".format(build_type.lower())
    
    cmake_generate = "cd build/linux/{_build_type} && cmake -G \"{_generator}\" -DCMAKE_BUILD_TYPE={_build_type} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER={_c_compiler} -DCMAKE_CXX_COMPILER={_cxx_compiler}  -DCMAKE_TOOLCHAIN_FILE=".format(_generator = generator, _build_type = build_type, _c_compiler = c_compiler, _cxx_compiler = cxx_compiler) + os.getcwd() + "/vcpkg/scripts/buildsystems/vcpkg.cmake ../../.."
    cmake_build = "cd build/linux/{} && cmake --build .".format(build_type.lower())
    
    delete_compile_commands = "rm compile_commands.json"
    copy_compile_commands = "cp build/linux/{}/compile_commands.json .".format(build_type.lower())

    run_command = "build/linux/{}/RDE".format(build_type.lower())

    if args[1] == "compile":
        os.system(create_build_folder)
        os.system(create_linux_folder)
        os.system(create_linux_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
    elif args[1] == "run":
        os.system(run_command)
    elif args[1] == "compile-run":
        os.system(create_build_folder)
        os.system(create_linux_folder)
        os.system(create_linux_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
        os.system(run_command)



def mac(args):
    build_type = "debug"
    if len(args) > 2:
        build_type = args[2]

    generator = "Ninja"
    if len(args) > 3:
        generator = args[3]

    c_compiler = "clang"
    if len(args) > 4:
        generator = args[4]

    cxx_compiler = "clang++"
    if len(args) > 5:
        generator = args[5]

    create_build_folder = "mkdir -p build"
    create_mac_folder = "mkdir -p build/mac"
    create_mac_build_folder = "mkdir -p build/mac/{}".format(build_type.lower())
    
    cmake_generate = "cd build/mac/{_build_type} && cmake -G \"{_generator}\" -DCMAKE_BUILD_TYPE={_build_type} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER={_c_compiler} -DCMAKE_CXX_COMPILER={_cxx_compiler}  -DCMAKE_TOOLCHAIN_FILE=".format(_generator = generator, _build_type = build_type, _c_compiler = c_compiler, _cxx_compiler = cxx_compiler) + os.getcwd() + "/vcpkg/scripts/buildsystems/vcpkg.cmake ../../.."
    cmake_build = "cd build/mac/{} && cmake --build .".format(build_type.lower())
    
    delete_compile_commands = "rm compile_commands.json"
    copy_compile_commands = "cp build/mac/{}/compile_commands.json .".format(build_type.lower())

    run_command = "build/mac/{}/RDE".format(build_type.lower())

    if args[1] == "compile":
        os.system(create_build_folder)
        os.system(create_mac_folder)
        os.system(create_mac_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
    elif args[1] == "run":
        os.system(run_command)
    elif args[1] == "compile-run":
        os.system(create_build_folder)
        os.system(create_mac_folder)
        os.system(create_mac_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
        os.system(run_command)



if platform == "linux" or platform == "linux2":
    linux(sys.argv)
elif platform == "darwin":
    pass
elif platform == "win32":
    windows(sys.argv)
