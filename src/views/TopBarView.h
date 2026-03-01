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
	virtual void		MouseMoved(BPoint where, uint32 transit,
							const BMessage* dragMessage);

	void				SetGame(Game* game);
	void				Refresh();
	void				SetVisualDeckCount(int count);
	void				AdjustVisualDeckCount(int delta);
	void				SyncVisualDeckCount();

private:
	void				DrawIconBox(BRect boxRect, const char* iconName, int value);
	void				DrawScoreBox(BRect boxRect);
	BRect				GetDeckBoxRect();
	BRect				GetDungeonBoxRect();
	void				UpdateTooltipForPoint(BPoint point);

	BView*				fPauseButton;
	BView*				fFleeButton;
	BView*				fSlashOverlay;

	Game*				fGame;
	int					fVisualDeckCount;
};

#endif // TOP_BAR_VIEW_H
