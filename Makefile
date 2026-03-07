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
	src/views/modals/DungeonBeatWindow.cpp \
	src/views/modals/HowToPlayWindow.cpp \
	src/views/modals/HighScoresWindow.cpp \
	src/views/modals/NameEntryWindow.cpp \
	src/views/modals/SettingsWindow.cpp \
	src/helpers/ResourceLoader.cpp \
	src/helpers/SoundPlayer.cpp \
	src/helpers/MusicPlayer.cpp \
	src/helpers/SaveManager.cpp \
	src/helpers/HighScoreManager.cpp

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

.DEFAULT_GOAL := copy-to-base

.PHONY: copy-to-base
copy-to-base: default
	@cp -f objects.*/$(NAME) ./$(NAME)
	@xres -o ./$(NAME) objects.*/$(NAME) -a VICN:101:BEOS:ICON data/images/icon
	@addattr -f data/images/icon -t icon BEOS:ICON ./$(NAME)
	@mimeset ./$(NAME)
	@echo "$(NAME) installed to base directory"

clean: clean-base

.PHONY: clean-base
clean-base:
	@rm -f ./$(NAME)
