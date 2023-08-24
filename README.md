*HOW TO BUILD THE LIBRARY*

* Windows: 	
		
		mkdir build
		cd build
		cmake -G "Visual Studio 17 2022" -DRDE_LIBRARY=ON .. #use any VS generator you prefer, but DO use a VS generator + any other option needed (release, debug...)
		cmake --build .

		The result .lib + .dll should be in build/Debug or build/Release, dependeing on your flags.

* Linux: 	

		mkdir build
		cd build
		cmake -G "Ninja" -DRDE_LIBRARY=ON .. #here use the generator preferred, Ninja and GNU are tested to work + any other option needed (release, debug...)
		cmake --build .

		The result .a should be in build or build, dependeing on your flags.
		If you want a .so file, just modify 

		if(UNIX AND NOT APPLE)
		set(OS "linux")
		set(LIB_TYPE)
		...

		for

		if(UNIX AND NOT APPLE)
		set(OS "linux")
		set(LIB_TYPE SHARED)
		...

		In order to only have to use libRDE.a, the following is recommended, otherwhise you will
		have to link your program with all the other static libraries (SDL2, SDL2_image, SDL2_mixer, imgui, glm and glad):
		
		ar -x external/linux/lib/*.a
		ar -x libRDE.a
		ar -c libRDE.a *.o

		* Note that 'ar -x external/linux/lib/*.a' won't work, is a way to show that you should do ar -x on every lib in that folder

		Alternatively, using CMakeLists and linking to all .a mentioned above, should also work.

*HOW TO USE THE LIBRARY*

This will be our example code, test.cpp:

```
#include "rde.h"

void on_update(float _dt) {
	UNUSED(_dt)
}

void on_fixed_update(float _dt) {
	UNUSED(_dt)
}

void on_late_update(float _dt) {
	UNUSED(_dt)
}

void on_render(float _dt) {
	UNUSED(_dt)
}

int main(int _argc, char** _argv){
	UNUSED(_argc)
	UNUSED(_argv)


#if IS_WINDOWS()
		RDE_SHOW_WINDOWS_CONSOLE
#endif

	rde_end_user_mandatory_callbacks _callbacks;
	_callbacks.on_update = &on_update;
	_callbacks.on_fixed_update = &on_fixed_update;
	_callbacks.on_late_update = &on_late_update;
	_callbacks.on_render = &on_render;

	rde_setup_initial_info(_callbacks);

	rde_engine* _engine = rde_engine_create_engine(0, nullptr);
	rde_engine_on_run(_engine);

	return 0;
}
```

* Windows:

	To build on windows you will need to call something like this


	clang++ .\test.cpp -IPATH_TO_RDE_INCLUDE -LPATH_TO_RDE.lib -lRDE  -o build/test.exe


* Linux:

	To build on linux you will need to call something like this


	shared:
		clang++ .\test.cpp -IPATH_TO_RDE_INCLUDE -LPATH_TO_RDE.so -lRDE  -o build/test


	static:
		if ar -x process was done -> clang++ .\test.cpp -IPATH_TO_RDE_INCLUDE PATH_TO_RDE/libRDE.a -lRDE  -o build/test
		if NOT ar -x process was done -> clang++ .\test.cpp -IPATH_TO_RDE_INCLUDE PATH_TO_RDE/libRDE.a PATH_TO_RDE/external/linux/build/*.a -lRDE  -o build/test