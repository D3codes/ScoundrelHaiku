#include "PauseWindow.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <StringView.h>
#include <View.h>

PauseWindow::PauseWindow(BWindow* parent)
	:
	BWindow(BRect(0, 0, 200, 200), "Paused",
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
	BView* mainView = new BView(bounds, "pauseView", B_FOLLOW_ALL, B_WILL_DRAW);
	mainView->SetViewColor(kBackgroundColor);

	// Title
	BStringView* titleView = new BStringView(
		BRect(0, 20, bounds.Width(), 50),
		"pauseTitle", "PAUSED");
	titleView->SetAlignment(B_ALIGN_CENTER);
	titleView->SetFont(be_bold_font);
	titleView->SetFontSize(kHeadingFontSize);
	titleView->SetHighColor(kTextColor);
	mainView->AddChild(titleView);

	// Buttons
	float buttonWidth = 120;
	float buttonHeight = 30;
	float centerX = bounds.Width() / 2;

	BButton* continueBtn = new BButton(
		BRect(centerX - buttonWidth / 2, 70,
			centerX + buttonWidth / 2, 70 + buttonHeight),
		"continueBtn", "Continue", new BMessage(kMsgContinue));
	mainView->AddChild(continueBtn);

	BButton* newGameBtn = new BButton(
		BRect(centerX - buttonWidth / 2, 110,
			centerX + buttonWidth / 2, 110 + buttonHeight),
		"newGameBtn", "New Game", new BMessage(kMsgNewGame));
	mainView->AddChild(newGameBtn);

	BButton* menuBtn = new BButton(
		BRect(centerX - buttonWidth / 2, 150,
			centerX + buttonWidth / 2, 150 + buttonHeight),
		"menuBtn", "Main Menu", new BMessage(kMsgMainMenu));
	mainView->AddChild(menuBtn);

	AddChild(mainView);
}


PauseWindow::~PauseWindow()
{
}


void
PauseWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgContinue:
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
