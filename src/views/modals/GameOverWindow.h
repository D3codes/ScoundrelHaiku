#ifndef GAME_OVER_WINDOW_H
#define GAME_OVER_WINDOW_H

#include <Window.h>

class GameOverWindow : public BWindow {
public:
						GameOverWindow(BWindow* parent, int score,
							int dungeonsBeaten);
	virtual				~GameOverWindow();

	virtual void		MessageReceived(BMessage* message);

private:
	BWindow*			fParent;
};

#endif // GAME_OVER_WINDOW_H
