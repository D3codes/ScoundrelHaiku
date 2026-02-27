#include "TopBarView.h"
#include "models/Game.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <LayoutBuilder.h>

TopBarView::TopBarView(BRect frame)
	:
	BView(frame, "topBarView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW),
	fGame(NULL)
{
	SetViewColor(80, 80, 100);

	// Create pause button
	fPauseButton = new BButton(BRect(5, 10, 55, 40), "pauseBtn", "Pause",
		new BMessage(kMsgPause), B_FOLLOW_LEFT | B_FOLLOW_V_CENTER);

	// Create flee button
	fFleeButton = new BButton(BRect(frame.Width() - 55, 10, frame.Width() - 5, 40),
		"fleeBtn", "Flee", new BMessage(kMsgFlee),
		B_FOLLOW_RIGHT | B_FOLLOW_V_CENTER);

	// Create deck count display
	fDeckCountView = new BStringView(BRect(65, 15, 115, 35), "deckCount", "Deck: 44");
	fDeckCountView->SetHighColor(kTextColor);

	// Create score display
	fScoreView = new BStringView(BRect(125, 15, 205, 35), "score", "Score: 0");
	fScoreView->SetHighColor(kTextColor);

	// Create dungeon counter
	fDungeonView = new BStringView(BRect(215, 15, 300, 35), "dungeon", "Dungeon: 1");
	fDungeonView->SetHighColor(kTextColor);

	AddChild(fPauseButton);
	AddChild(fFleeButton);
	AddChild(fDeckCountView);
	AddChild(fScoreView);
	AddChild(fDungeonView);
}


TopBarView::~TopBarView()
{
}


void
TopBarView::AttachedToWindow()
{
	BView::AttachedToWindow();

	fPauseButton->SetTarget(Window());
	fFleeButton->SetTarget(Window());
}


void
TopBarView::Draw(BRect updateRect)
{
	// Draw stone-like texture background
	BRect bounds = Bounds();

	// Gradient from darker to lighter
	rgb_color topColor = {70, 70, 90, 255};
	rgb_color bottomColor = {90, 90, 110, 255};

	for (float y = bounds.top; y <= bounds.bottom; y++) {
		float ratio = (y - bounds.top) / (bounds.bottom - bounds.top);
		rgb_color color;
		color.red = topColor.red + (bottomColor.red - topColor.red) * ratio;
		color.green = topColor.green + (bottomColor.green - topColor.green) * ratio;
		color.blue = topColor.blue + (bottomColor.blue - topColor.blue) * ratio;
		color.alpha = 255;
		SetHighColor(color);
		StrokeLine(BPoint(bounds.left, y), BPoint(bounds.right, y));
	}

	// Draw bottom border
	SetHighColor(50, 50, 70);
	StrokeLine(BPoint(bounds.left, bounds.bottom),
		BPoint(bounds.right, bounds.bottom));
}


void
TopBarView::SetGame(Game* game)
{
	fGame = game;
	Refresh();
}


void
TopBarView::Refresh()
{
	if (fGame == NULL)
		return;

	// Update deck count
	BString deckStr;
	deckStr.SetToFormat("Deck: %d", fGame->GetDeck()->CardsRemaining());
	fDeckCountView->SetText(deckStr.String());

	// Update score
	BString scoreStr;
	scoreStr.SetToFormat("Score: %d", fGame->Score());
	fScoreView->SetText(scoreStr.String());

	// Update dungeon counter
	BString dungeonStr;
	dungeonStr.SetToFormat("Dungeon: %d", fGame->DungeonDepth() + 1);
	fDungeonView->SetText(dungeonStr.String());

	// Update flee button state
	fFleeButton->SetEnabled(fGame->GetRoom()->CanFlee());
}
