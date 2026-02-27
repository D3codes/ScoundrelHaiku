#include "GameBoardView.h"
#include "TopBarView.h"
#include "RoomView.h"
#include "StatsBarView.h"
#include "models/Game.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <stdlib.h>
#include <Window.h>

GameBoardView::GameBoardView(BRect frame)
	:
	BView(frame, "gameBoardView", B_FOLLOW_ALL, B_WILL_DRAW),
	fGame(NULL),
	fBackgroundIndex(1)
{
	SetViewColor(B_TRANSPARENT_COLOR);
	RandomizeBackground();

	// Create top bar
	BRect topBarRect(0, 0, frame.Width(), kTopBarHeight);
	fTopBarView = new TopBarView(topBarRect);
	AddChild(fTopBarView);

	// Create room view (center area)
	float roomTop = kTopBarHeight + 10;
	float roomBottom = frame.Height() - kStatsBarHeight - 10;
	BRect roomRect(0, roomTop, frame.Width(), roomBottom);
	fRoomView = new RoomView(roomRect);
	AddChild(fRoomView);

	// Create stats bar at bottom
	BRect statsRect(0, frame.Height() - kStatsBarHeight,
		frame.Width(), frame.Height());
	fStatsBarView = new StatsBarView(statsRect);
	AddChild(fStatsBarView);
}


GameBoardView::~GameBoardView()
{
}


void
GameBoardView::AttachedToWindow()
{
	BView::AttachedToWindow();
}


void
GameBoardView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();

	// Draw dungeon background
	BString bgName;
	bgName.SetToFormat("dungeon%d", fBackgroundIndex);
	BBitmap* background = ResourceLoader::Instance()->GetBackground(bgName.String());

	if (background != NULL) {
		SetDrawingMode(B_OP_COPY);
		DrawBitmap(background, background->Bounds(), bounds);
	} else {
		// Fallback to solid color
		SetHighColor(kBackgroundColor);
		FillRect(bounds);
	}
}


void
GameBoardView::RandomizeBackground()
{
	fBackgroundIndex = (rand() % 5) + 1; // dungeon1 through dungeon5
	Invalidate();
}


void
GameBoardView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgCardSelected:
		{
			// Forward to window for modal handling
			Window()->PostMessage(message);
			break;
		}
		default:
			BView::MessageReceived(message);
			break;
	}
}


void
GameBoardView::SetGame(Game* game)
{
	fGame = game;

	fTopBarView->SetGame(game);
	fRoomView->SetRoom(game->GetRoom());
	fStatsBarView->SetPlayer(game->GetPlayer());

	Refresh();
}


void
GameBoardView::Refresh()
{
	if (fGame == NULL)
		return;

	fTopBarView->Refresh();
	fRoomView->Refresh();
	fStatsBarView->Refresh();
}
