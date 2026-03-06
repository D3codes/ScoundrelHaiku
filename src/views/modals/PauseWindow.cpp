#include "PauseWindow.h"
#include "helpers/ResourceLoader.h"
#include "helpers/SoundPlayer.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <StringView.h>
#include <View.h>


// Title bar with close button and title (same style as CardActionWindow)
class PauseTitleBar : public BView {
public:
	PauseTitleBar(BRect frame)
		: BView(frame, "titleBar", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Draw stone background
		BBitmap* stoneBg = ResourceLoader::Instance()->GetUIImage("stoneSlab2");
		if (stoneBg != NULL) {
			SetDrawingMode(B_OP_COPY);
			DrawBitmap(stoneBg, stoneBg->Bounds(), bounds);
		} else {
			SetHighColor(80, 80, 100);
			FillRect(bounds);
		}

		// Draw close button (X) on the left
		float buttonSize = 30;
		float buttonMargin = 5;
		BRect closeRect(buttonMargin, (bounds.Height() - buttonSize) / 2,
			buttonMargin + buttonSize, (bounds.Height() + buttonSize) / 2);

		SetHighColor(60, 60, 80);
		FillRoundRect(closeRect, 5, 5);
		SetHighColor(100, 100, 120);
		StrokeRoundRect(closeRect, 5, 5);

		// Draw X
		SetHighColor(kTextColor);
		SetPenSize(2);
		float inset = 8;
		StrokeLine(BPoint(closeRect.left + inset, closeRect.top + inset),
			BPoint(closeRect.right - inset, closeRect.bottom - inset));
		StrokeLine(BPoint(closeRect.right - inset, closeRect.top + inset),
			BPoint(closeRect.left + inset, closeRect.bottom - inset));
		SetPenSize(1);

		// Draw title centered
		BFont font;
		font.SetSize(20);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		const char* title = "Pause";
		float titleWidth = StringWidth(title);
		float titleX = (bounds.Width() - titleWidth) / 2;
		float titleY = bounds.Height() / 2 + 7;

		// Draw shadow
		SetHighColor(0, 0, 0, 180);
		DrawString(title, BPoint(titleX + 2, titleY + 2));

		// Draw title
		SetHighColor(kTextColor);
		DrawString(title, BPoint(titleX, titleY));
	}

	virtual void MouseDown(BPoint where) {
		// Check if close button was clicked
		float buttonSize = 30;
		float buttonMargin = 5;
		BRect closeRect(buttonMargin, (Bounds().Height() - buttonSize) / 2,
			buttonMargin + buttonSize, (Bounds().Height() + buttonSize) / 2);

		if (closeRect.Contains(where)) {
			Window()->PostMessage(new BMessage(kMsgContinue));
		}
	}
};


// Content view with background color matching card modal
class PauseContentView : public BView {
public:
	PauseContentView(BRect frame)
		: BView(frame, "pauseContent", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(222, 210, 190);
	}
};


class PlankButtonPause : public BView {
public:
	PlankButtonPause(BRect frame, const char* label, BMessage* message)
		: BView(frame, label, B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual ~PlankButtonPause() {
		delete fMessage;
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();
		float radius = 8;

		// Draw solid rounded background first (shows in corners)
		SetHighColor(100, 70, 50);
		FillRoundRect(bounds, radius, radius);

		// Draw plank background inset to show rounded corners
		BBitmap* plankBg = ResourceLoader::Instance()->GetUIImage("plank1");
		if (plankBg != NULL) {
			BRect insetBounds = bounds.InsetByCopy(2, 2);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(plankBg, plankBg->Bounds(), insetBounds);
			SetDrawingMode(B_OP_COPY);
		}

		// Draw rounded border
		SetHighColor(120, 90, 60);
		StrokeRoundRect(bounds, radius, radius);

		BFont font;
		font.SetSize(18);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float textWidth = StringWidth(fLabel.String());
		float textX = (bounds.Width() - textWidth) / 2;
		float textY = bounds.Height() / 2 + 6;

		// Draw shadow
		SetHighColor(0, 0, 0, 180);
		DrawString(fLabel.String(), BPoint(textX + 2, textY + 2));

		// Draw text
		SetHighColor(kTextColor);
		DrawString(fLabel.String(), BPoint(textX, textY));
	}

	virtual void MouseDown(BPoint where) {
		Window()->PostMessage(fMessage);
	}

private:
	BString fLabel;
	BMessage* fMessage;
};


PauseWindow::PauseWindow(BWindow* parent)
	:
	BWindow(BRect(0, 0, 220, 275), "Pause",
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

	float titleBarHeight = 40;
	float buttonWidth = 190;
	float buttonHeight = 36;
	float buttonSpacing = 10;
	float windowWidth = 220;

	// Create title bar
	BRect titleRect(0, 0, windowWidth, titleBarHeight);
	PauseTitleBar* titleBar = new PauseTitleBar(titleRect);
	AddChild(titleBar);

	// Create content view for buttons
	BRect contentRect(0, titleBarHeight, windowWidth, Bounds().Height());
	PauseContentView* contentView = new PauseContentView(contentRect);

	float centerX = windowWidth / 2;
	float btnY = 10;

	// Buttons in order: Main Menu, How to Play, High Scores, Settings, New Game
	PlankButtonPause* menuBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, btnY,
			centerX + buttonWidth / 2, btnY + buttonHeight),
		"Main Menu", new BMessage(kMsgMainMenu));
	contentView->AddChild(menuBtn);
	btnY += buttonHeight + buttonSpacing;

	PlankButtonPause* howToPlayBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, btnY,
			centerX + buttonWidth / 2, btnY + buttonHeight),
		"How to Play", new BMessage(kMsgHowToPlay));
	contentView->AddChild(howToPlayBtn);
	btnY += buttonHeight + buttonSpacing;

	PlankButtonPause* highScoresBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, btnY,
			centerX + buttonWidth / 2, btnY + buttonHeight),
		"High Scores", new BMessage(kMsgHighScores));
	contentView->AddChild(highScoresBtn);
	btnY += buttonHeight + buttonSpacing;

	PlankButtonPause* settingsBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, btnY,
			centerX + buttonWidth / 2, btnY + buttonHeight),
		"Settings", new BMessage(kMsgSettings));
	contentView->AddChild(settingsBtn);
	btnY += buttonHeight + buttonSpacing;

	PlankButtonPause* newGameBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, btnY,
			centerX + buttonWidth / 2, btnY + buttonHeight),
		"New Game", new BMessage(kMsgNewGame));
	contentView->AddChild(newGameBtn);

	AddChild(contentView);
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
		case kMsgHowToPlay:
		case kMsgHighScores:
			// Forward to parent GameWindow which manages the singleton windows
			fParent->PostMessage(message);
			break;
		case kMsgSettings:
			// Play a sound to verify click is received, then forward
			SoundPlayer::Instance()->PlaySound(SFX_PAGE);
			fParent->PostMessage(message);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
