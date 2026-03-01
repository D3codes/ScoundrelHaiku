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
		font.SetSize(22);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float textWidth = StringWidth(fLabel.String());
		float textX = (bounds.Width() - textWidth) / 2;
		float textY = bounds.Height() / 2 + 8;

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
		BRect(buttonX, 200, buttonX + buttonWidth, 200 + buttonHeight),
		"Resume", new BMessage(kMsgResumeGame));

	// New Game button
	fNewGameButton = new PlankButtonMenu(
		BRect(buttonX, 260, buttonX + buttonWidth, 260 + buttonHeight),
		"New Game", new BMessage(kMsgNewGame));

	// How to Play button
	fHowToPlayButton = new PlankButtonMenu(
		BRect(buttonX, 320, buttonX + buttonWidth, 320 + buttonHeight),
		"How to Play", new BMessage(kMsgHowToPlay));

	// High Scores button
	fHighScoresButton = new PlankButtonMenu(
		BRect(buttonX, 380, buttonX + buttonWidth, 380 + buttonHeight),
		"High Scores", new BMessage(kMsgHighScores));

	AddChild(fNewGameButton);
	AddChild(fHowToPlayButton);
	AddChild(fHighScoresButton);
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
	float spacing = 55;

	if (hasSaved) {
		// Add resume button and adjust other buttons
		if (fResumeButton->Parent() == NULL)
			AddChild(fResumeButton);

		float startY = 200;
		fResumeButton->MoveTo(buttonX, startY);
		fNewGameButton->MoveTo(buttonX, startY + spacing);
		fHowToPlayButton->MoveTo(buttonX, startY + spacing * 2);
		fHighScoresButton->MoveTo(buttonX, startY + spacing * 3);
	} else {
		// Remove resume button and move others up
		if (fResumeButton->Parent() != NULL)
			fResumeButton->RemoveSelf();

		float startY = 230;
		fNewGameButton->MoveTo(buttonX, startY);
		fHowToPlayButton->MoveTo(buttonX, startY + spacing);
		fHighScoresButton->MoveTo(buttonX, startY + spacing * 2);
	}

	Invalidate();
}
