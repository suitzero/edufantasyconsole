all: main.o

main.o: main.c lua
	emcc -Ilua-5.3.4/src main.c lua-5.3.4/src/liblua.a -s WASM=1 -O2 -o main.js -s \
    EXPORTED_FUNCTIONS="['_run_lua','_main','_cancel_run']" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'\
    -s ASYNCIFY -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file assets -s USE_SDL_TTF=2 --embed-file font.ttf

lua:
	cd lua-5.3.4/src && make generic CC='emcc -s WASM=1'
