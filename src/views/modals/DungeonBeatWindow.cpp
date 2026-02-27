#include "DungeonBeatWindow.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Button.h>
#include <StringView.h>
#include <View.h>

DungeonBeatWindow::DungeonBeatWindow(BWindow* parent, int score, int dungeonDepth)
	:
	BWindow(BRect(0, 0, 280, 220), "Dungeon Complete",
		B_MODAL_WINDOW_LOOK, B_MODAL_SUBSET_WINDOW_FEEL,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fParent(parent)
{
	AddToSubset(parent);

	// Center on parent
	BRect parentFrame = parent->Frame();
	BRect frame = Frame();
	float x = parentFrame.left + (parentFrame.Width() - frame.Width()) / 2;
	float y = parentFrame.top + (parentFrame.Height() - frame.Height()) / 2;
	MoveTo(x, y);

	// Create main view
	BRect bounds = Bounds();
	BView* mainView = new BView(bounds, "dungeonBeatView", B_FOLLOW_ALL, B_WILL_DRAW);
	mainView->SetViewColor(kBackgroundColor);

	// Title
	BStringView* titleView = new BStringView(
		BRect(0, 20, bounds.Width(), 50),
		"dungeonBeatTitle", "DUNGEON CLEARED!");
	titleView->SetAlignment(B_ALIGN_CENTER);
	titleView->SetFont(be_bold_font);
	titleView->SetFontSize(kHeadingFontSize);
	titleView->SetHighColor(kPotionColor);
	mainView->AddChild(titleView);

	// Congratulations
	BString congratsStr;
	congratsStr.SetToFormat("You conquered dungeon %d!", dungeonDepth + 1);
	BStringView* congratsView = new BStringView(
		BRect(0, 60, bounds.Width(), 85),
		"congratsView", congratsStr.String());
	congratsView->SetAlignment(B_ALIGN_CENTER);
	congratsView->SetHighColor(kTextColor);
	mainView->AddChild(congratsView);

	// Score
	BString scoreStr;
	scoreStr.SetToFormat("Current Score: %d", score);
	BStringView* scoreView = new BStringView(
		BRect(0, 95, bounds.Width(), 120),
		"scoreView", scoreStr.String());
	scoreView->SetAlignment(B_ALIGN_CENTER);
	scoreView->SetHighColor(kTextColor);
	mainView->AddChild(scoreView);

	// Continue button
	float buttonWidth = 180;
	float buttonHeight = 35;
	float centerX = bounds.Width() / 2;

	BButton* continueBtn = new BButton(
		BRect(centerX - buttonWidth / 2, 150,
			centerX + buttonWidth / 2, 150 + buttonHeight),
		"continueBtn", "Continue to Next Dungeon",
		new BMessage(kMsgNextDungeon));
	mainView->AddChild(continueBtn);

	AddChild(mainView);
}


DungeonBeatWindow::~DungeonBeatWindow()
{
}


void
DungeonBeatWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgNextDungeon:
			fParent->PostMessage(message);
			PostMessage(B_QUIT_REQUESTED);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
