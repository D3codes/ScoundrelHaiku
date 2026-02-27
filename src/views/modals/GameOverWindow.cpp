#include "GameOverWindow.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Button.h>
#include <StringView.h>
#include <View.h>

GameOverWindow::GameOverWindow(BWindow* parent, int score, int dungeonsBeaten)
	:
	BWindow(BRect(0, 0, 250, 250), "Game Over",
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
	BView* mainView = new BView(bounds, "gameOverView", B_FOLLOW_ALL, B_WILL_DRAW);
	mainView->SetViewColor(kBackgroundColor);

	// Title
	BStringView* titleView = new BStringView(
		BRect(0, 20, bounds.Width(), 50),
		"gameOverTitle", "GAME OVER");
	titleView->SetAlignment(B_ALIGN_CENTER);
	titleView->SetFont(be_bold_font);
	titleView->SetFontSize(kTitleFontSize);
	titleView->SetHighColor(kMonsterColor);
	mainView->AddChild(titleView);

	// Score
	BString scoreStr;
	scoreStr.SetToFormat("Final Score: %d", score);
	BStringView* scoreView = new BStringView(
		BRect(0, 70, bounds.Width(), 95),
		"scoreView", scoreStr.String());
	scoreView->SetAlignment(B_ALIGN_CENTER);
	scoreView->SetHighColor(kTextColor);
	mainView->AddChild(scoreView);

	// Dungeons beaten
	BString dungeonStr;
	dungeonStr.SetToFormat("Dungeons Beaten: %d", dungeonsBeaten);
	BStringView* dungeonView = new BStringView(
		BRect(0, 100, bounds.Width(), 125),
		"dungeonView", dungeonStr.String());
	dungeonView->SetAlignment(B_ALIGN_CENTER);
	dungeonView->SetHighColor(kTextColor);
	mainView->AddChild(dungeonView);

	// Buttons
	float buttonWidth = 120;
	float buttonHeight = 30;
	float centerX = bounds.Width() / 2;

	BButton* newGameBtn = new BButton(
		BRect(centerX - buttonWidth / 2, 150,
			centerX + buttonWidth / 2, 150 + buttonHeight),
		"newGameBtn", "New Game", new BMessage(kMsgNewGame));
	mainView->AddChild(newGameBtn);

	BButton* menuBtn = new BButton(
		BRect(centerX - buttonWidth / 2, 190,
			centerX + buttonWidth / 2, 190 + buttonHeight),
		"menuBtn", "Main Menu", new BMessage(kMsgMainMenu));
	mainView->AddChild(menuBtn);

	AddChild(mainView);
}


GameOverWindow::~GameOverWindow()
{
}


void
GameOverWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgNewGame:
		case kMsgMainMenu:
			fParent->PostMessage(message);
			PostMessage(B_QUIT_REQUESTED);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
