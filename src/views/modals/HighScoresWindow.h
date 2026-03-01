#ifndef HIGH_SCORES_WINDOW_H
#define HIGH_SCORES_WINDOW_H

#include <Window.h>

class HighScoresWindow : public BWindow {
public:
						HighScoresWindow(BWindow* parent = NULL);
	virtual				~HighScoresWindow();

	virtual void		MessageReceived(BMessage* message);
};

#endif // HIGH_SCORES_WINDOW_H
