TARGET = rvmscd.elf

CFLAGS = -DDREAMCAST -ICore/ -std=c99

CSRC := main.c Core/AnimationSystem.c Core/GifLoader.c Core/ObjectSystem.c  \
Core/StageSystem.c Core/AudioPlayback_dc.c Core/GlobalAppDefinitions.c Core/PlayerSystem.c \
Core/TextSystem.c Core/EngineCallbacks.c Core/GraphicsSystem.c Core/RenderDevice_dc.c \
Core/FileIO.c Core/InputSystem_dc.c Core/Scene3D.c

COBJ = $(patsubst %.c, %.o, $(CSRC))

OBJS = $(COBJ)

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) -L$(KOS_BASE)/lib -lGL -lm $(KOS_LIBS)

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS)
	$(KOS_STRIP) $(TARGET)

