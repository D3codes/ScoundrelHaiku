#include "MainMenuView.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Window.h>

MainMenuView::MainMenuView(BRect frame)
	:
	BView(frame, "mainMenuView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(kBackgroundColor);

	float centerX = frame.Width() / 2;
	float buttonWidth = 150;
	float buttonHeight = 35;
	float buttonX = centerX - buttonWidth / 2;

	// Create title
	fTitleView = new BStringView(BRect(0, 80, frame.Width(), 130),
		"title", "SCOUNDREL");
	fTitleView->SetAlignment(B_ALIGN_CENTER);
	fTitleView->SetFont(be_bold_font);
	fTitleView->SetFontSize(kTitleFontSize * 1.5);
	fTitleView->SetHighColor(kTextColor);
	AddChild(fTitleView);

	// Subtitle
	BStringView* subtitleView = new BStringView(
		BRect(0, 130, frame.Width(), 160),
		"subtitle", "A Dungeon Card Solitaire");
	subtitleView->SetAlignment(B_ALIGN_CENTER);
	subtitleView->SetHighColor(150, 150, 160);
	AddChild(subtitleView);

	// New Game button
	fNewGameButton = new BButton(
		BRect(buttonX, 200, buttonX + buttonWidth, 200 + buttonHeight),
		"newGameBtn", "New Game", new BMessage(kMsgNewGame));
	AddChild(fNewGameButton);

	// Resume button (initially hidden)
	fResumeButton = new BButton(
		BRect(buttonX, 250, buttonX + buttonWidth, 250 + buttonHeight),
		"resumeBtn", "Resume", new BMessage(kMsgResumeGame));
	fResumeButton->Hide();
	AddChild(fResumeButton);

	// How to Play button
	fHowToPlayButton = new BButton(
		BRect(buttonX, 300, buttonX + buttonWidth, 300 + buttonHeight),
		"howToPlayBtn", "How to Play", new BMessage(kMsgHowToPlay));
	AddChild(fHowToPlayButton);
}


MainMenuView::~MainMenuView()
{
}


void
MainMenuView::AttachedToWindow()
{
	BView::AttachedToWindow();

	fNewGameButton->SetTarget(Window());
	fResumeButton->SetTarget(Window());
	fHowToPlayButton->SetTarget(Window());
}


void
MainMenuView::Draw(BRect updateRect)
{
	// Draw gradient background
	BRect bounds = Bounds();

	rgb_color topColor = {30, 30, 40, 255};
	rgb_color bottomColor = {50, 50, 70, 255};

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
}


void
MainMenuView::SetHasSavedGame(bool hasSaved)
{
	if (hasSaved) {
		fResumeButton->Show();
		// Move How to Play button down
		fHowToPlayButton->MoveTo(fHowToPlayButton->Frame().left, 300);
	} else {
		fResumeButton->Hide();
		// Move How to Play button up
		fHowToPlayButton->MoveTo(fHowToPlayButton->Frame().left, 250);
	}
}
