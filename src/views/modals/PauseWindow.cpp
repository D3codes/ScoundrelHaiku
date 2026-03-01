#include "PauseWindow.h"
#include "HowToPlayWindow.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <StringView.h>
#include <View.h>

class PauseView : public BView {
public:
	PauseView(BRect frame)
		: BView(frame, "pauseView", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(40, 40, 50);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Draw solid dark background
		SetHighColor(40, 40, 50);
		FillRect(bounds);

		// Draw paper background for whole modal
		BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
		if (paperBg != NULL) {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(paperBg, paperBg->Bounds(), bounds);
			SetDrawingMode(B_OP_COPY);
		} else {
			SetHighColor(kCardBackgroundColor);
			FillRect(bounds);
		}

		// Draw title "Pause"
		BFont titleFont;
		titleFont.SetSize(36);
		titleFont.SetFace(B_BOLD_FACE);
		SetFont(&titleFont);
		SetHighColor(kDarkTextColor);

		const char* title = "Paused";
		float titleWidth = StringWidth(title);
		DrawString(title, BPoint(bounds.Width() / 2 - titleWidth / 2, 60));
	}
};


class PlankButtonPause : public BView {
public:
	PlankButtonPause(BRect frame, const char* label, BMessage* message)
		: BView(frame, "plankBtn", B_FOLLOW_NONE, B_WILL_DRAW),
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
		font.SetSize(20);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);
		SetHighColor(kTextColor);

		float textWidth = StringWidth(fLabel.String());
		DrawString(fLabel.String(),
			BPoint((bounds.Width() - textWidth) / 2, bounds.Height() / 2 + 7));
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
	BWindow(BRect(0, 0, 300, 450), "Paused",
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
	PauseView* mainView = new PauseView(bounds);

	// Buttons
	float buttonWidth = 180;
	float buttonHeight = 40;
	float buttonSpacing = 50;
	float centerX = bounds.Width() / 2;
	float startY = 120;

	PlankButtonPause* newGameBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, startY,
			centerX + buttonWidth / 2, startY + buttonHeight),
		"New Game", new BMessage(kMsgNewGame));
	mainView->AddChild(newGameBtn);

	PlankButtonPause* howToPlayBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, startY + buttonSpacing,
			centerX + buttonWidth / 2, startY + buttonSpacing + buttonHeight),
		"How to Play", new BMessage(kMsgHowToPlay));
	mainView->AddChild(howToPlayBtn);

	PlankButtonPause* menuBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, startY + buttonSpacing * 2,
			centerX + buttonWidth / 2, startY + buttonSpacing * 2 + buttonHeight),
		"Main Menu", new BMessage(kMsgMainMenu));
	mainView->AddChild(menuBtn);

	float continueY = startY + buttonSpacing * 3 + 20; // Extra padding before Continue
	PlankButtonPause* continueBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, continueY,
			centerX + buttonWidth / 2, continueY + buttonHeight),
		"Continue", new BMessage(kMsgContinue));
	mainView->AddChild(continueBtn);

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
		case kMsgHowToPlay:
		{
			HowToPlayWindow* howToPlay = new HowToPlayWindow(this);
			howToPlay->Show();
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
