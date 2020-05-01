# This makefile is actually for emscripten, but you could probably adapt it for Linux
TARGET = rvmscd

CFLAGS = -DLINUX -DSDL_DISABLE_IMMINTRIN_H -DSDL_DISABLE_MMINTRIN_H -Irvm/Core/ -std=c99 -s USE_SDL=2 -s USE_SDL_MIXER=2

CSRC := rvm/main_linux.c rvm/Core/AnimationSystem.c rvm/Core/GifLoader.c rvm/Core/ObjectSystem.c  \
rvm/Core/StageSystem.c rvm/Core/AudioPlayback.c rvm/Core/GlobalAppDefinitions.c rvm/Core/PlayerSystem.c \
rvm/Core/TextSystem.c rvm/Core/EngineCallbacks.c rvm/Core/GraphicsSystem.c rvm/Core/RenderDevice.c \
rvm/Core/FileIO.c rvm/Core/InputSystem.c rvm/Core/Scene3D.c

COBJ = $(patsubst %.c, %.o, $(CSRC))

OBJS = $(COBJ)

all: rm-elf $(TARGET)

clean:
	-rm -f $(TARGET) $(OBJS)

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p webOut
	emcc -o webOut/$(TARGET).html $(OBJS) $(OBJEXTRA) -lSDL2 -lSDL2_Mixer -lGL -lm -s USE_OGG=1 -s USE_VORBIS=1 -s LEGACY_GL_EMULATION=1 -s ALLOW_MEMORY_GROWTH=1 -s --preload-file ../Data.rsdk@Data.rsdk

run: $(TARGET)
	$(TARGET)

dist:
	rm -f $(OBJS)
	$(TARGET)
