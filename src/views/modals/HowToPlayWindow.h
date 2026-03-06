#ifndef HOW_TO_PLAY_WINDOW_H
#define HOW_TO_PLAY_WINDOW_H

#include <Window.h>

class HowToPlayWindow : public BWindow {
public:
						HowToPlayWindow(BWindow* parent);
	virtual				~HowToPlayWindow();

	virtual void		MessageReceived(BMessage* message);
	virtual bool		QuitRequested();

private:
	BWindow*			fParent;
};

#endif // HOW_TO_PLAY_WINDOW_H
