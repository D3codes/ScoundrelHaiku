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
	BView(frame, "gameBoardView", B_FOLLOW_ALL,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fGame(NULL),
	fBackgroundIndex(1),
	fTopBarView(NULL),
	fRoomView(NULL),
	fStatsBarView(NULL)
{
	// Don't use B_TRANSPARENT_COLOR - we draw our own background
	SetViewColor(kBackgroundColor);

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

	// Now randomize background after views are created
	RandomizeBackground();
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

	// Calculate deck position for card animations
	// The deck icon is in the top bar, centered in the first icon box
	float padding = 10;
	float buttonSize = 50;
	float spacing = 8;
	float iconBoxSize = 50;
	float scoreBoxWidth = 80;
	float topBarWidth = Bounds().Width();

	float totalBoxesWidth = iconBoxSize + spacing + scoreBoxWidth + spacing + iconBoxSize;
	float leftEdge = padding + buttonSize + spacing;
	float rightEdge = topBarWidth - padding - buttonSize - spacing;
	float availableWidth = rightEdge - leftEdge;
	float startX = leftEdge + (availableWidth - totalBoxesWidth) / 2;

	// Deck box is the first one, center of it
	BPoint deckPos(startX + iconBoxSize / 2, 15 + iconBoxSize / 2);
	fRoomView->SetDeckPosition(deckPos);

	Refresh();
}


void
GameBoardView::Refresh()
{
	if (fGame == NULL)
		return;

	// Refresh child views
	if (fTopBarView != NULL)
		fTopBarView->Refresh();
	if (fRoomView != NULL)
		fRoomView->Refresh();
	if (fStatsBarView != NULL)
		fStatsBarView->Refresh();
	Invalidate();

	// Force synchronous redraw
	if (Window() != NULL) {
		Window()->UpdateIfNeeded();
		Sync();
	}
}


void
GameBoardView::RefreshWithAnimation()
{
	if (fGame == NULL)
		return;

	// Refresh top bar and stats bar normally
	if (fTopBarView != NULL)
		fTopBarView->Refresh();
	if (fStatsBarView != NULL)
		fStatsBarView->Refresh();

	// Animate the room cards
	if (fRoomView != NULL)
		fRoomView->RefreshWithAnimation();

	Invalidate();
}
