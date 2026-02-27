#include "GameBoardView.h"
#include "TopBarView.h"
#include "RoomView.h"
#include "StatsBarView.h"
#include "models/Game.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <OS.h>
#include <stdlib.h>
#include <Window.h>

GameBoardView::GameBoardView(BRect frame)
	:
	BView(frame, "gameBoardView", B_FOLLOW_ALL,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fGame(NULL),
	fBackgroundIndex(1)
{
	// Don't use B_TRANSPARENT_COLOR - we draw our own background
	SetViewColor(kBackgroundColor);
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

	// Always fill with solid background first to prevent show-through
	SetHighColor(kBackgroundColor);
	FillRect(bounds);

	// Draw dungeon background on top
	BString bgName;
	bgName.SetToFormat("dungeon%d", fBackgroundIndex);
	BBitmap* background = ResourceLoader::Instance()->GetBackground(bgName.String());

	if (background != NULL) {
		SetDrawingMode(B_OP_COPY);
		DrawBitmap(background, background->Bounds(), bounds);
	}
}


void
GameBoardView::RandomizeBackground()
{
	fBackgroundIndex = (rand() % 5) + 1; // dungeon1 through dungeon5
	if (fRoomView != NULL)
		fRoomView->SetBackgroundIndex(fBackgroundIndex);
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

	if (Window() == NULL)
		return;

	// Check if we need to lock (window might already be locked during MessageReceived)
	bool needsUnlock = false;
	thread_id currentThread = find_thread(NULL);
	if (Window()->LockingThread() != currentThread) {
		if (Window()->LockWithTimeout(50000) != B_OK) {
			return; // Can't get lock, skip this refresh
		}
		needsUnlock = true;
	}

	fTopBarView->Refresh();
	fRoomView->Refresh();
	fStatsBarView->Refresh();
	Invalidate();

	if (needsUnlock)
		Window()->Unlock();
}
