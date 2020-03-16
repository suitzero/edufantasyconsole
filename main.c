#include <stdio.h>
#include <SDL/SDL.h>

#include "lua.h"
#include <lauxlib.h>
#include <lualib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int render(lua_State *l){
	  SDL_Init(SDL_INIT_VIDEO);
	  SDL_Surface *screen = SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE);

#ifdef TEST_SDL_LOCK_OPTS
	  EM_ASM("SDL.defaults.copyOnLock = false; SDL.defaults.discardOnLock = true; SDL.defaults.opaqueFrontBuffer = false;");
#endif

	  if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	  for (int i = 0; i < 256; i++) {
	    for (int j = 0; j < 256; j++) {
#ifdef TEST_SDL_LOCK_OPTS
	      // Alpha behaves like in the browser, so write proper opaque pixels.
	      int alpha = 255;
#else
	      // To emulate native behavior with blitting to screen, alpha component is ignored. Test that it is so by outputting
	      // data (and testing that it does get discarded)
	      int alpha = (i+j) % 255;
#endif
	      *((Uint32*)screen->pixels + i * 256 + j) = SDL_MapRGBA(screen->format, i, j, 255-i, alpha);
	    }
	  }
	  if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
	  SDL_Flip(screen); 

	  printf("you should see a smoothly-colored square - no sharp lines but the square borders!\n");
	  printf("and here is some text that should be HTML-friendly: amp: |&| double-quote: |\"| quote: |'| less-than, greater-than, html-like tags: |<cheez></cheez>|\nanother line.\n");

	  SDL_Quit();
	  return 0;
}	

int run_lua(const char* script) {
	lua_State* lua = luaL_newstate();
	luaL_openlibs(lua);

    lua_register(lua, "colorful_render", render);

	int res = luaL_dostring(lua, script);

	size_t len = 0;
	const char* value = lua_tolstring(lua, lua_gettop(lua), &len);

	printf("%s\n", value);

	lua_close(lua);

	return 0;
}
