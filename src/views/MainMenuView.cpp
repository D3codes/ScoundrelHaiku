#include "MainMenuView.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Window.h>
#include <stdlib.h>

// Custom plank-style button for main menu
class PlankButtonMenu : public BView {
public:
	PlankButtonMenu(BRect frame, const char* label, BMessage* message)
		: BView(frame, "plankBtn", B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message)
	{
		SetViewColor(100, 70, 50);
	}

	virtual ~PlankButtonMenu() {
		delete fMessage;
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Always draw solid background first
		SetHighColor(100, 70, 50);
		FillRoundRect(bounds, 5, 5);

		BBitmap* plankBg = ResourceLoader::Instance()->GetUIImage("plank1");
		if (plankBg != NULL) {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(plankBg, plankBg->Bounds(), bounds);
			SetDrawingMode(B_OP_COPY);
		}

		BFont font;
		font.SetSize(22);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);
		SetHighColor(kTextColor);

		float textWidth = StringWidth(fLabel.String());
		DrawString(fLabel.String(),
			BPoint((bounds.Width() - textWidth) / 2, bounds.Height() / 2 + 8));
	}

	virtual void MouseDown(BPoint where) {
		Window()->PostMessage(fMessage);
	}

private:
	BString fLabel;
	BMessage* fMessage;
};


MainMenuView::MainMenuView(BRect frame)
	:
	BView(frame, "mainMenuView", B_FOLLOW_ALL, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fHasSavedGame(false),
	fBackgroundIndex(1)
{
	// Use solid color - we draw our own background in Draw()
	SetViewColor(30, 30, 40);

	// Randomize background
	fBackgroundIndex = (rand() % 5) + 1;

	float centerX = frame.Width() / 2;
	float buttonWidth = 200;
	float buttonHeight = 45;
	float buttonX = centerX - buttonWidth / 2;

	// Resume button (initially hidden)
	fResumeButton = new PlankButtonMenu(
		BRect(buttonX, 220, buttonX + buttonWidth, 220 + buttonHeight),
		"Resume", new BMessage(kMsgResumeGame));

	// New Game button
	fNewGameButton = new PlankButtonMenu(
		BRect(buttonX, 280, buttonX + buttonWidth, 280 + buttonHeight),
		"New Game", new BMessage(kMsgNewGame));

	// How to Play button
	fHowToPlayButton = new PlankButtonMenu(
		BRect(buttonX, 340, buttonX + buttonWidth, 340 + buttonHeight),
		"How to Play", new BMessage(kMsgHowToPlay));

	AddChild(fNewGameButton);
	AddChild(fHowToPlayButton);
}


MainMenuView::~MainMenuView()
{
	// Remove resume button if not added to view
	if (fResumeButton->Parent() == NULL)
		delete fResumeButton;
}


void
MainMenuView::AttachedToWindow()
{
	BView::AttachedToWindow();
}


void
MainMenuView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();

	// Draw dark background first as fallback
	SetHighColor(30, 30, 40);
	FillRect(bounds);

	// Draw dungeon background
	BString bgName;
	bgName.SetToFormat("dungeon%d", fBackgroundIndex);
	BBitmap* background = ResourceLoader::Instance()->GetBackground(bgName.String());

	if (background != NULL) {
		SetDrawingMode(B_OP_COPY);
		DrawBitmap(background, background->Bounds(), bounds);
	}

	// Draw title bar with stoneSlab2 background
	BRect titleBarRect(0, 0, bounds.Width(), 80);

	BBitmap* stoneBg = ResourceLoader::Instance()->GetUIImage("stoneSlab2");
	if (stoneBg != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(stoneBg, stoneBg->Bounds(), titleBarRect);
		SetDrawingMode(B_OP_COPY);
	} else {
		SetHighColor(70, 70, 90);
		FillRect(titleBarRect);
	}

	// Draw title shadow
	SetHighColor(0, 0, 0, 150);
	StrokeLine(BPoint(0, titleBarRect.bottom + 1),
		BPoint(bounds.Width(), titleBarRect.bottom + 1));
	StrokeLine(BPoint(0, titleBarRect.bottom + 2),
		BPoint(bounds.Width(), titleBarRect.bottom + 2));

	// Draw title "SCOUNDREL"
	BFont titleFont;
	titleFont.SetSize(40);
	titleFont.SetFace(B_BOLD_FACE);
	SetFont(&titleFont);

	const char* title = "SCOUNDREL";
	float titleWidth = StringWidth(title);
	float titleX = (bounds.Width() - titleWidth) / 2;
	float titleY = 55;

	// Draw shadow
	SetHighColor(0, 0, 0, 200);
	DrawString(title, BPoint(titleX + 2, titleY + 2));

	// Draw title
	SetHighColor(kTextColor);
	DrawString(title, BPoint(titleX, titleY));

	// Draw subtitle
	BFont subtitleFont;
	subtitleFont.SetSize(16);
	SetFont(&subtitleFont);
	SetHighColor(150, 150, 160);

	const char* subtitle = "A Dungeon Card Solitaire";
	float subtitleWidth = StringWidth(subtitle);
	DrawString(subtitle, BPoint((bounds.Width() - subtitleWidth) / 2, 160));
}


void
MainMenuView::SetHasSavedGame(bool hasSaved)
{
	if (hasSaved == fHasSavedGame)
		return;

	fHasSavedGame = hasSaved;

	float centerX = Bounds().Width() / 2;
	float buttonWidth = 200;
	float buttonHeight = 45;
	float buttonX = centerX - buttonWidth / 2;

	if (hasSaved) {
		// Add resume button and adjust other buttons
		if (fResumeButton->Parent() == NULL)
			AddChild(fResumeButton);

		fResumeButton->MoveTo(buttonX, 220);
		fNewGameButton->MoveTo(buttonX, 280);
		fHowToPlayButton->MoveTo(buttonX, 340);
	} else {
		// Remove resume button and move others up
		if (fResumeButton->Parent() != NULL)
			fResumeButton->RemoveSelf();

		fNewGameButton->MoveTo(buttonX, 250);
		fHowToPlayButton->MoveTo(buttonX, 310);
	}

	Invalidate();
}
