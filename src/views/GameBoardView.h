#ifndef GAME_BOARD_VIEW_H
#define GAME_BOARD_VIEW_H

#include <View.h>

class Game;
class RoomView;
class TopBarView;
class StatsBarView;

class GameBoardView : public BView {
public:
						GameBoardView(BRect frame);
	virtual				~GameBoardView();

	virtual void		Draw(BRect updateRect);
	virtual void		AttachedToWindow();
	virtual void		MessageReceived(BMessage* message);

	void				SetGame(Game* game);
	void				Refresh();

private:
	TopBarView*			fTopBarView;
	RoomView*			fRoomView;
	StatsBarView*		fStatsBarView;
	Game*				fGame;
};

#endif // GAME_BOARD_VIEW_H
