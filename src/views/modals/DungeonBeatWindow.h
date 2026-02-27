#ifndef DUNGEON_BEAT_WINDOW_H
#define DUNGEON_BEAT_WINDOW_H

#include <Window.h>

class DungeonBeatWindow : public BWindow {
public:
						DungeonBeatWindow(BWindow* parent, int score,
							int dungeonDepth);
	virtual				~DungeonBeatWindow();

	virtual void		MessageReceived(BMessage* message);

private:
	BWindow*			fParent;
};

#endif // DUNGEON_BEAT_WINDOW_H
