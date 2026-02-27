#ifndef TOP_BAR_VIEW_H
#define TOP_BAR_VIEW_H

#include <Button.h>
#include <StringView.h>
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
	BButton*			fPauseButton;
	BButton*			fFleeButton;
	BStringView*		fDeckCountView;
	BStringView*		fScoreView;
	BStringView*		fDungeonView;

	Game*				fGame;
};

#endif // TOP_BAR_VIEW_H
