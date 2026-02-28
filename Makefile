# Scoundrel for Haiku OS
# Makefile

NAME = Scoundrel
TYPE = APP
SRCS = \
	src/main.cpp \
	src/App.cpp \
	src/models/Card.cpp \
	src/models/Deck.cpp \
	src/models/Player.cpp \
	src/models/Room.cpp \
	src/models/Game.cpp \
	src/views/GameWindow.cpp \
	src/views/MainMenuView.cpp \
	src/views/GameBoardView.cpp \
	src/views/CardView.cpp \
	src/views/RoomView.cpp \
	src/views/TopBarView.cpp \
	src/views/StatsBarView.cpp \
	src/views/modals/CardActionWindow.cpp \
	src/views/modals/PauseWindow.cpp \
	src/views/modals/GameOverWindow.cpp \
	src/views/modals/DungeonBeatWindow.cpp \
	src/views/modals/HowToPlayWindow.cpp \
	src/helpers/ResourceLoader.cpp \
	src/helpers/SoundPlayer.cpp \
	src/helpers/SaveManager.cpp

RDEFS = resources/Scoundrel.rdef

LIBS = be media game translation tracker stdc++
LIBPATHS =
SYSTEM_INCLUDE_PATHS =
LOCAL_INCLUDE_PATHS = src
OPTIMIZE := FULL
LOCALES =
DEFINES =
WARNINGS = ALL
SYMBOLS := FALSE
DEBUGGER := FALSE
COMPILER_FLAGS =
LINKER_FLAGS =

DEVEL_DIRECTORY := /boot/system/develop

include $(DEVEL_DIRECTORY)/etc/makefile-engine
