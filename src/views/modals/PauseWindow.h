#ifndef PAUSE_WINDOW_H
#define PAUSE_WINDOW_H

#include <Button.h>
#include <Window.h>

class PauseWindow : public BWindow {
public:
						PauseWindow(BWindow* parent);
	virtual				~PauseWindow();

	virtual void		MessageReceived(BMessage* message);

private:
	BWindow*			fParent;
};

#endif // PAUSE_WINDOW_H
