#include "GameOverWindow.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Button.h>
#include <StringView.h>
#include <View.h>

class GameOverView : public BView {
public:
	GameOverView(BRect frame, int score, int dungeonsBeaten)
		: BView(frame, "gameOverView", B_FOLLOW_ALL, B_WILL_DRAW),
		  fScore(score),
		  fDungeonsBeaten(dungeonsBeaten)
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

		// Draw title "Game Over"
		BFont titleFont;
		titleFont.SetSize(36);
		titleFont.SetFace(B_BOLD_FACE);
		SetFont(&titleFont);
		SetHighColor(kDarkTextColor);

		const char* title = "Game Over";
		float titleWidth = StringWidth(title);
		DrawString(title, BPoint(bounds.Width() / 2 - titleWidth / 2, 60));

		// Draw score
		BFont bodyFont;
		bodyFont.SetSize(24);
		SetFont(&bodyFont);
		SetHighColor(kDarkTextColor);

		BString scoreLabel = "Score:";
		BString scoreValue;
		scoreValue.SetToFormat("%d", fScore);

		float labelX = 50;
		float valueX = bounds.Width() - 50 - StringWidth(scoreValue.String());
		float scoreY = 120;

		DrawString(scoreLabel.String(), BPoint(labelX, scoreY));
		DrawString(scoreValue.String(), BPoint(valueX, scoreY));

		// Draw dungeons
		BString dungeonLabel = "Dungeons:";
		BString dungeonValue;
		dungeonValue.SetToFormat("%d", fDungeonsBeaten);

		valueX = bounds.Width() - 50 - StringWidth(dungeonValue.String());
		float dungeonY = scoreY + 40;

		DrawString(dungeonLabel.String(), BPoint(labelX, dungeonY));
		DrawString(dungeonValue.String(), BPoint(valueX, dungeonY));
	}

private:
	int fScore;
	int fDungeonsBeaten;
};


class PlankButtonGO : public BView {
public:
	PlankButtonGO(BRect frame, const char* label, BMessage* message)
		: BView(frame, "plankBtn", B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual ~PlankButtonGO() {
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


GameOverWindow::GameOverWindow(BWindow* parent, int score, int dungeonsBeaten)
	:
	BWindow(BRect(0, 0, 300, 400), "Game Over",
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
	GameOverView* mainView = new GameOverView(bounds, score, dungeonsBeaten);

	// Buttons
	float buttonWidth = 180;
	float buttonHeight = 40;
	float centerX = bounds.Width() / 2;

	PlankButtonGO* menuBtn = new PlankButtonGO(
		BRect(centerX - buttonWidth / 2, 280,
			centerX + buttonWidth / 2, 280 + buttonHeight),
		"Main Menu", new BMessage(kMsgMainMenu));
	mainView->AddChild(menuBtn);

	PlankButtonGO* newGameBtn = new PlankButtonGO(
		BRect(centerX - buttonWidth / 2, 330,
			centerX + buttonWidth / 2, 330 + buttonHeight),
		"New Game", new BMessage(kMsgNewGame));
	mainView->AddChild(newGameBtn);

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
