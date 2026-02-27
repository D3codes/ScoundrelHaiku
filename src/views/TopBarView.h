#ifndef TOP_BAR_VIEW_H
#define TOP_BAR_VIEW_H

#include <View.h>

class Game;

class TopBarView : public BView {
public:
						TopBarView(BRect frame);
	virtual				~TopBarView();

	virtual void		Draw(BRect updateRect);
	virtual void		AttachedToWindow();

	void				SetGame(Game* game);
	void				Refresh();

private:
	void				DrawIconBox(BRect boxRect, const char* iconName, int value);
	void				DrawScoreBox(BRect boxRect);

	BView*				fPauseButton;
	BView*				fFleeButton;

	Game*				fGame;
};

#endif // TOP_BAR_VIEW_H
