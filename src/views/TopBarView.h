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
	virtual void		MessageReceived(BMessage* message);

	void				SetGame(Game* game);
	void				Refresh();
	void				SetVisualDeckCount(int count);
	void				AdjustVisualDeckCount(int delta);
	void				SyncVisualDeckCount();

private:
	void				DrawIconBox(BRect boxRect, const char* iconName, int value);
	void				DrawScoreBox(BRect boxRect);

	BView*				fPauseButton;
	BView*				fFleeButton;

	Game*				fGame;
	int					fVisualDeckCount;
};

#endif // TOP_BAR_VIEW_H
