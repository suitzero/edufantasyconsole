#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>

#include "lua.h"
#include <lauxlib.h>
#include <lualib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

lua_State *g_lua = NULL;

int callfirst = 1;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const int MAX_IMAGES = 7;
SDL_Surface* images[MAX_IMAGES];
static int quit = 0;
int SCREEN_WIDTH=256, SCREEN_HEIGHT=256;
int state = -1;

int fillRect(lua_State *l){
    //printf("fillrect\n");
    SDL_Rect r_scr;
    r_scr.x = lua_tonumber(l, 1);
    r_scr.y = lua_tonumber(l, 2);
    r_scr.w = lua_tonumber(l, 3);
    r_scr.h = lua_tonumber(l, 4);
    int r = lua_tonumber(l, 5);
    int g = lua_tonumber(l, 6);
    int b = lua_tonumber(l, 7);
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
    SDL_RenderFillRect(renderer, &r_scr);
    return 0;
}

SDL_Surface* loadimg(const char* filename)
{
	SDL_Surface *image = IMG_Load(filename);
	if(!image)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
	}
	//SDL_Rect srcrect = { sprite * 32, 0, 32, 64 };
	return image;
}


void loadImages()
{
	char filenames[7][30] = {"assets/banana.png","assets/explosion.png","assets/gorilla0.png",
					"assets/gorilla1.png", "assets/gorilla2.png", "assets/gorilla3.png","assets/sun.png"};
	for( int i = 0 ; i < MAX_IMAGES ; ++i )
	{
		images[i] =  loadimg(filenames[i]);
	}
}

TTF_Font* Sans;

int write_text(lua_State *l)
{
	size_t len;
	const char *str = luaL_tolstring(l,1,&len);
    int x = lua_tonumber(l, 2);
    int y = lua_tonumber(l, 3);
    int w = lua_tonumber(l, 4);
    int h = lua_tonumber(l, 5);
	SDL_Color White = {0, 0, 0}; 	

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, str, White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
	Message_rect.w = w; // controls the width of the rect
	Message_rect.h = h; // controls the height of the rect

	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	return 0;
}

int spr(lua_State *l){
	int num =lua_tonumber(l, 1);
    int x = lua_tonumber(l, 2);
    int y = lua_tonumber(l, 3);
    int w = lua_tonumber(l, 4);
    int h = lua_tonumber(l, 5);
	if( num > MAX_IMAGES )
	{
		printf("no sprte with the number");
		return -1;
	}
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer,
			images[num-1]);
	SDL_Rect dstrect = { x, y, w, h };
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	return 0;
}

int btn(lua_State *l){
	int num=lua_tonumber(l, 1);
	return num == state;
}
void render()
{
    SDL_RenderPresent(renderer);
}

#if __EMSCRIPTEN__
void main_tick() {
#else
int main_tick() {
#endif
    SDL_Event event;

	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
		{
			quit = 1;
			break;
		}
		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_UP:
				{
					state=0;
					printf("up\n");
					break;
				}
				case SDLK_DOWN:
				{
					state=1;
					printf("down\n");
					break;
				}
				case SDLK_LEFT:
				{
					state=2;
					printf("left\n");
					break;
				}
				case SDLK_RIGHT:
				{
					state=3;
					printf("right\n");
					break;
				}
				case SDLK_SPACE:
				{
					state=4;
					printf("space\n");
					break;
				}
				default:
				{
					state=-1;
					break;
				}
			}
			break;
			
		}
		case SDL_KEYUP:
		{
			state=-1;
			break;
		}
		}

	}

	lua_getglobal(g_lua, "_update");
	if (lua_pcall(g_lua, 0, 0, 0)){
	}else{
	}

	lua_getglobal(g_lua, "_draw");
	if (lua_pcall(g_lua, 0, 0, 0)){
	}else{
	}

    render();
    SDL_UpdateWindowSurface(window);

#if !__EMSCRIPTEN__
    return 0;
#endif
}

void main_loop()
{

#if __EMSCRIPTEN__
    emscripten_set_main_loop(main_tick, -1, 1);
#else
    while (0 == quit)
    {
        main_tick();
    }
#endif
}

int main()
{
	/*
	g_lua = luaL_newstate();
	luaL_openlibs(g_lua);
	//lua_register(g_lua, "colorful_render", render);
	lua_register(g_lua, "fill_rect", fillRect);
	lua_register(g_lua, "spr", fillRect);

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "WEBASM",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
*/
    return 0;
}

int run_lua(const char* script) {

	g_lua = luaL_newstate();
	luaL_openlibs(g_lua);
	//lua_register(g_lua, "colorful_render", render);
	lua_register(g_lua, "fill_rect", fillRect);
	lua_register(g_lua, "spr", spr);
	lua_register(g_lua, "btn", btn);
	lua_register(g_lua, "write_text", write_text);
	loadImages();
    SDL_Init(SDL_INIT_VIDEO);

	TTF_Init();
	Sans = TTF_OpenFont("font.ttf",25);
	if(!Sans){
		printf("font loading failed\n");
	}

    window = SDL_CreateWindow(
        "WEBASM",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

	int res = luaL_dostring(g_lua, script);

	size_t len = 0;
	const char* value = lua_tolstring(g_lua, lua_gettop(g_lua), &len);
	printf("run_code:%s\n", value);

	main_loop();

	return 0;
}

int cancel_run(const char* script) {
	quit = 1;
	emscripten_cancel_main_loop();

	TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}


