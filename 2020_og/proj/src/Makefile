# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c resources.c ev_handler.c lib/devices/timer/timer.c lib/devices/kbc/kbc.c lib/devices/kbc/keyboard.c lib/devices/kbc/mouse.c lib/devices/graphics/vbe.c lib/devices/rtc/rtc.c game/player/player.c game/map/map.c lib/utils/utils.c lib/sprite/sprite.c lib/sprite/asprite.c lib/font/font.c lib/panel/panel.c lib/panel/button.c menu/menu.c menu/instructions.c menu/settings.c game/game.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CPPFLAGS += -pedantic -DPROJ
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# object files linking
lib/devices/timer/timer.o: lib/devices/timer/timer.c lib/devices/timer/timer.h
	cc $(CFLAGS) -o lib/devices/timer/timer.o -c lib/devices/timer/timer.c

lib/devices/kbc/kbc.o: lib/devices/kbc/kbc.c lib/devices/kbc/kbc.h
	cc $(CFLAGS) -o lib/devices/kbc/kbc.o -c lib/devices/kbc/kbc.c

lib/devices/kbc/keyboard.o: lib/devices/kbc/keyboard.c lib/devices/kbc/keyboard.h
	cc $(CFLAGS) -o lib/devices/kbc/keyboard.o -c lib/devices/kbc/keyboard.c

lib/devices/kbc/mouse.o: lib/devices/kbc/mouse.c lib/devices/kbc/mouse.h
	cc $(CFLAGS) -o lib/devices/kbc/mouse.o -c lib/devices/kbc/mouse.c

lib/devices/graphics/vbe.o: lib/devices/graphics/vbe.c lib/devices/graphics/vbe.h
	cc $(CFLAGS) -o lib/devices/graphics/vbe.o -c lib/devices/graphics/vbe.c

lib/devices/rtc/rtc.o: lib/devices/rtc/rtc.c lib/devices/rtc/rtc.h
	cc $(CFLAGS) -o lib/devices/rtc/rtc.o -c lib/devices/rtc/rtc.c

game/player/player.o: game/player/player.c game/player/player.h
	cc $(CFLAGS) -o game/player/player.o -c game/player/player.c

game/map/map.o: game/map/map.c game/map/map.h
	cc $(CFLAGS) -o game/map/map.o -c game/map/map.c

menu/menu.o: menu/menu.c menu/menu.h
	cc $(CFLAGS) -o menu/menu.o -c menu/menu.c

menu/instructions.o: menu/instructions.c menu/instructions.h
	cc $(CFLAGS) -o menu/instructions.o -c menu/instructions.c

menu/settings.o: menu/settings.c menu/settings.h
	cc $(CFLAGS) -o menu/settings.o -c menu/settings.c

game/game.o: game/game.c game/game.h
	cc $(CFLAGS) -o game/game.o -c game/game.c

lib/utils/utils.o: lib/utils/utils.c lib/utils/utils.h
	cc $(CFLAGS) -o lib/utils/utils.o -c lib/utils/utils.c

lib/sprite/sprite.o: lib/sprite/sprite.c lib/sprite/sprite.h
	cc $(CFLAGS) -o lib/sprite/sprite.o -c lib/sprite/sprite.c

lib/sprite/asprite.o: lib/sprite/asprite.c lib/sprite/asprite.h
	cc $(CFLAGS) -o lib/sprite/asprite.o -c lib/sprite/asprite.c

lib/font/font.o: lib/font/font.c lib/font/font.h
	cc $(CFLAGS) -o lib/font/font.o -c lib/font/font.c

lib/panel/panel.o: lib/panel/panel.c lib/panel/panel.h
	cc $(CFLAGS) -o lib/panel/panel.o -c lib/panel/panel.c

lib/panel/button.o: lib/panel/button.c lib/panel/button.h
	cc $(CFLAGS) -o lib/panel/button.o -c lib/panel/button.c

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
