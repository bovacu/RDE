#include "engine/engine.h"
#include "SDL2/SDL.h"

#if IS_WINDOWS()
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

#define ENGINE_DEBUG
#include "core.h"

int main(int _argc, char** _argv){
	UNUSED(_argc)
	UNUSED(_argv)


	#if IS_WINDOWS()
		#ifdef ENGINE_DEBUG
			if(AllocConsole())
			{
				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
				freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
				std::ios::sync_with_stdio(1);
			}
		#endif
	#endif

	rde_engine* _engine = rde_engine_create_engine(0, nullptr);
	rde_engine_on_run(_engine);

	return 0;
}