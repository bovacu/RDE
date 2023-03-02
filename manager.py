from sys import platform
import sys
import os


def extract_argument(args, arg, default):
    matchers = [arg]
    found_arg = [s for s in args if any(xs in s for xs in matchers)]
    if len(found_arg) > 0:
        if arg == "--c_compiler":
            return found_arg[0].replace("--c_compiler=", "")
        if arg == "--cpp_compiler":
            return found_arg[0].replace("--cpp_compiler=", "")
        if arg == "--generator":
            return found_arg[0].replace("--generator=", "")
        if arg == "--build_type":
            return found_arg[0].replace("--build_type=", "")
        if arg == "--action":
            return found_arg[0].replace("--action=", "")
        if arg == "--info":
            return ""
        if arg == "--generate_vs_project":
            return True
    else:
        return default


def buildin_process_windows(args, action):
    build_type = extract_argument(args, "--build_type", "debug")
    generator = extract_argument(args, "--generator", "Ninja")
    c_compiler = extract_argument(args, "--c_compiler", "clang")
    cxx_compiler = extract_argument(args, "--cpp_compiler", "clang++")
    generate_vs_project = extract_argument(args, "--generate_vs_project", "")

    create_build_folder = "if not exist build mkdir build"
    create_windows_folder = "if not exist build\\windows mkdir build\\windows"
    create_windows_build_folder = "if not exist build\\windows\\{} mkdir build\\windows\\{}".format( build_type.lower(), build_type.lower())
    cmake_generate = "cd build\\windows\\{_build_type} && cmake -G \"{_generator}\" -DCMAKE_BUILD_TYPE={_build_type} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER={_c_compiler} -DCMAKE_CXX_COMPILER={_cxx_compiler}  -DCMAKE_TOOLCHAIN_FILE=".format(_generator = generator, _build_type = build_type, _c_compiler = c_compiler, _cxx_compiler = cxx_compiler) + os.getcwd() + "/vcpkg/scripts/buildsystems/vcpkg.cmake ..\\..\\.."
    cmake_build = "cd build\\windows\\{} && cmake --build .".format(build_type.lower())
    delete_compile_commands = "del compile_commands.json"
    copy_compile_commands = "copy build\\windows\\{}\\compile_commands.json .".format(build_type.lower())

    run_command = "start build\\windows\\{}\\RDE.exe".format(build_type.lower())

    if generate_vs_project:
        os.system(create_build_folder)
        os.system("if not exist build\\windows_vs mkdir build\\windows_vs")
        os.system("cd build\\windows_vs && cmake -G \"Visual Studio 17 2022\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake ..\\..")
        os.system("cd build\\windows_vs && cmake --build .")
        exit(0)

    if action == "build":
        os.system(create_build_folder)
        os.system(create_windows_folder)
        os.system(create_windows_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
    elif action == "run":
        os.system(run_command)
    elif action == "build-run":
        os.system(create_build_folder)
        os.system(create_windows_folder)
        os.system(create_windows_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
        os.system(run_command)


def project_lines_number_windows():
    print("This command is not available in Windows, only Linux and Mac")
    pass


def buildin_process_linux(args, action):
    build_type = extract_argument(args, "--build_type", "debug")
    generator = extract_argument(args, "--generator", "Ninja")
    c_compiler = extract_argument(args, "--c_compiler", "clang")
    cxx_compiler = extract_argument(args, "--cpp_compiler", "clang++")

    create_build_folder = "mkdir -p build"
    create_linux_folder = "mkdir -p build/linux"
    create_linux_build_folder = "mkdir -p build/linux/{}".format(build_type.lower())
    cmake_generate = "cd build/linux/{_build_type} && cmake -G \"{_generator}\" -DCMAKE_BUILD_TYPE={_build_type} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER={_c_compiler} -DCMAKE_CXX_COMPILER={_cxx_compiler}  -DCMAKE_TOOLCHAIN_FILE=".format(_generator = generator, _build_type = build_type, _c_compiler = c_compiler, _cxx_compiler = cxx_compiler) + os.getcwd() + "/vcpkg/scripts/buildsystems/vcpkg.cmake ../../.."
    cmake_build = "cd build/linux/{} && cmake --build .".format(build_type.lower())
    delete_compile_commands = "rm compile_commands.json"
    copy_compile_commands = "cp build/linux/{}/compile_commands.json .".format(build_type.lower())

    run_command = "build/linux/{}/RDE".format(build_type.lower())

    if action == "build":
        os.system(create_build_folder)
        os.system(create_linux_folder)
        os.system(create_linux_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
    elif action == "run":
        os.system(run_command)
    elif action == "build-run":
        os.system(create_build_folder)
        os.system(create_linux_folder)
        os.system(create_linux_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
        os.system(run_command)


def project_lines_number_linux():
    origin = os.getcwd()
    os.chdir(origin + "/engine")
    os.system("find . -name '*.cpp' -o -name '*.h' | xargs wc -l")
    os.chdir(origin)


def buildin_process_mac(args, action):
    build_type = extract_argument(args, "--build_type", "debug")
    generator = extract_argument(args, "--generator", "Ninja")
    c_compiler = extract_argument(args, "--c_compiler", "clang")
    cxx_compiler = extract_argument(args, "--cpp_compiler", "clang++")

    create_build_folder = "mkdir -p build"
    create_mac_folder = "mkdir -p build/mac"
    create_mac_build_folder = "mkdir -p build/mac/{}".format(build_type.lower())
    cmake_generate = "cd build/mac/{_build_type} && cmake -G \"{_generator}\" -DCMAKE_BUILD_TYPE={_build_type} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER={_c_compiler} -DCMAKE_CXX_COMPILER={_cxx_compiler}  -DCMAKE_TOOLCHAIN_FILE=".format(_generator = generator, _build_type = build_type, _c_compiler = c_compiler, _cxx_compiler = cxx_compiler) + os.getcwd() + "/vcpkg/scripts/buildsystems/vcpkg.cmake ../../.."
    cmake_build = "cd build/mac/{} && cmake --build -j3 .".format(build_type.lower())
    delete_compile_commands = "rm compile_commands.json"
    copy_compile_commands = "cp build/mac/{}/compile_commands.json .".format(build_type.lower())

    run_command = "build/mac/{}/RDE".format(build_type.lower())

    if action == "build":
        os.system(create_build_folder)
        os.system(create_mac_folder)
        os.system(create_mac_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
    elif action == "run":
        os.system(run_command)
    elif action == "build-run":
        os.system(create_build_folder)
        os.system(create_mac_folder)
        os.system(create_mac_build_folder)
        os.system(cmake_generate)
        os.system(cmake_build)
        os.system(delete_compile_commands)
        os.system(copy_compile_commands)
        os.system(run_command)


def project_lines_number_mac():
    origin = os.getcwd()
    os.chdir(origin + "/engine")
    os.system("find . -name '*.cpp' -o -name '*.h' | xargs wc -l")
    os.chdir(origin)


def windows(args):
    action = extract_argument(args, "--action", "")
    generate_vs_project = extract_argument(args, "--generate_vs_project", "")
    if action == "build" or action == "build-run" or action == "run" or generate_vs_project:
        buildin_process_windows(args, action)
    elif action == "lines":
        project_lines_number_windows()


def linux(args):
    action = extract_argument(args, "--action", "")
    if action == "build" or action == "build-run" or action == "run":
        buildin_process_linux(args, action)
    elif action == "lines":
        project_lines_number_linux()


def mac(args):
    action = extract_argument(args, "--action", "")
    if action == "build" or action == "build-run" or action == "run":
        buildin_process_linux(args, action)
    elif action == "lines":
        project_lines_number_mac()


def help():
    print("======================================= COMPILING AND RUNNING =================================================")
    print("")
    print("In order to compile, run or compile or run --action argument is needed, with the following options:")
    print("     --action=build (compiles the project to the current platform)")
    print("     --action=run (runs the last generated build)")
    print("     --action=build-run (compiles the project to the current platform and runs it)")
    print("")
    print("Additionally the following arguments are available:")
    print("   | Argument         |   Options         |    Description")
    print("   | -----------------------------------------------------------------------------------------------------------------------------")
    print("   | --build_type     |   release/debug   |    (type of build)")
    print("   | --generator      |   Ninja/MSVC      |    (generator for the build. MSVC has a special name for each version, so check it out)")
    print("   | --c_compiler     |   clang/cl/gcc    |    (C compiler)")
    print("   | --cpp_compiler   |   clang++/cl/g++  |    (C++ compiler)")
    print("")
    print("===============================================================================================================")
    print("")
    print("")
    print("======================================= UTILITY COMMANDS =================================================")
    print("")
    print("Here are some options for the --action argument:")
    print("     --action=lines          Returns the number of lines in the project (only counting inner RDE lines, not external libs)")
    print("")
    print("==========================================================================================================")


if len(sys.argv) <= 1:
    help()
    exit(0)

if len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    help()
    exit(0)

if platform == "linux" or platform == "linux2":
    linux(sys.argv)
    exit(0)
elif platform == "darwin":
    mac(sys.argv)
    exit(0)
elif platform == "win32":
    windows(sys.argv)
    exit(0)

print("Argument not recognized, please run the script with 'help', '--help' or '-h' to get information about how to use the script")
