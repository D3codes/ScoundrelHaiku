#include "PauseWindow.h"
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
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Draw semi-transparent overlay
		SetHighColor(0, 0, 0, 180);
		FillRect(bounds);

		// Draw paper background for modal
		BRect modalRect(bounds.Width() / 2 - 150, bounds.Height() / 2 - 200,
			bounds.Width() / 2 + 150, bounds.Height() / 2 + 200);

		BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
		if (paperBg != NULL) {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(paperBg, paperBg->Bounds(), modalRect);
			SetDrawingMode(B_OP_COPY);
		} else {
			SetHighColor(kCardBackgroundColor);
			FillRoundRect(modalRect, 20, 20);
		}

		// Draw title "Pause"
		BFont titleFont;
		titleFont.SetSize(36);
		titleFont.SetFace(B_BOLD_FACE);
		SetFont(&titleFont);
		SetHighColor(kTextColor);

		const char* title = "Pause";
		float titleWidth = StringWidth(title);
		DrawString(title, BPoint(bounds.Width() / 2 - titleWidth / 2,
			modalRect.top + 60));
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

		BBitmap* plankBg = ResourceLoader::Instance()->GetUIImage("plank1");
		if (plankBg != NULL) {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(plankBg, plankBg->Bounds(), bounds);
			SetDrawingMode(B_OP_COPY);
		} else {
			SetHighColor(100, 70, 50);
			FillRoundRect(bounds, 5, 5);
		}

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
	BWindow(BRect(0, 0, 300, 400), "Paused",
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

	// Buttons (order matches iOS: Main Menu, New Game, Continue)
	float buttonWidth = 180;
	float buttonHeight = 40;
	float centerX = bounds.Width() / 2;

	PlankButtonPause* menuBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, 180,
			centerX + buttonWidth / 2, 180 + buttonHeight),
		"Main Menu", new BMessage(kMsgMainMenu));
	mainView->AddChild(menuBtn);

	PlankButtonPause* newGameBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, 230,
			centerX + buttonWidth / 2, 230 + buttonHeight),
		"New Game", new BMessage(kMsgNewGame));
	mainView->AddChild(newGameBtn);

	PlankButtonPause* continueBtn = new PlankButtonPause(
		BRect(centerX - buttonWidth / 2, 300,
			centerX + buttonWidth / 2, 300 + buttonHeight),
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
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
