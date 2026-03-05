#include "MainMenuView.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <String.h>
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
	fBackgroundIndex(1),
	fDeckCount(0),
	fScore(0),
	fDungeon(0),
	fHealth(0),
	fShield(0),
	fSword(0)
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

	// Draw saved game stats if there's a saved game
	if (fHasSavedGame)
		DrawSavedGameStats();
}


void
MainMenuView::DrawSavedGameStats()
{
	BRect bounds = Bounds();
	float centerX = bounds.Width() / 2;
	float statsY = 110;  // Below title bar

	float iconSize = 24;
	float spacing = 8;
	float groupSpacing = 20;

	BFont font;
	font.SetSize(16);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	// Calculate total width for centering
	// Top row: deck, score, dungeon
	// Bottom row: heart, shield, sword

	// Draw top row: deck, "Score", dungeon
	float topRowY = statsY;

	// Deck icon and count
	BBitmap* deckIcon = ResourceLoader::Instance()->GetGlyph("deck");
	BString deckStr;
	deckStr.SetToFormat("%d", fDeckCount);
	float deckTextWidth = StringWidth(deckStr.String());

	// Score label and value
	BString scoreStr;
	scoreStr.SetToFormat("%d", fScore);
	float scoreLabelWidth = StringWidth("Score");
	float scoreTextWidth = StringWidth(scoreStr.String());

	// Dungeon icon and count
	BBitmap* dungeonIcon = ResourceLoader::Instance()->GetGlyph("dungeonGlyph");
	BString dungeonStr;
	dungeonStr.SetToFormat("%d", fDungeon);
	float dungeonTextWidth = StringWidth(dungeonStr.String());

	// Calculate positions for top row (3 groups)
	float topRowWidth = (iconSize + spacing + deckTextWidth) + groupSpacing +
		(scoreLabelWidth + spacing + scoreTextWidth) + groupSpacing +
		(iconSize + spacing + dungeonTextWidth);
	float topRowStartX = centerX - topRowWidth / 2;

	float x = topRowStartX;

	// Draw deck
	if (deckIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(deckIcon, deckIcon->Bounds(),
			BRect(x, topRowY, x + iconSize, topRowY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}
	x += iconSize + spacing;
	SetHighColor(kTextColor);
	DrawString(deckStr.String(), BPoint(x, topRowY + iconSize - 5));
	x += deckTextWidth + groupSpacing;

	// Draw score (text label instead of icon)
	SetHighColor(200, 180, 140);  // Lighter color for label
	DrawString("Score", BPoint(x, topRowY + iconSize - 5));
	x += scoreLabelWidth + spacing;
	SetHighColor(kTextColor);
	DrawString(scoreStr.String(), BPoint(x, topRowY + iconSize - 5));
	x += scoreTextWidth + groupSpacing;

	// Draw dungeon
	if (dungeonIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(dungeonIcon, dungeonIcon->Bounds(),
			BRect(x, topRowY, x + iconSize, topRowY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}
	x += iconSize + spacing;
	SetHighColor(kTextColor);
	DrawString(dungeonStr.String(), BPoint(x, topRowY + iconSize - 5));

	// Draw bottom row: heart, shield, sword
	float bottomRowY = statsY + iconSize + 12;

	// Heart icon and health
	BBitmap* heartIcon = ResourceLoader::Instance()->GetGlyph("heart1");
	BString healthStr;
	healthStr.SetToFormat("%d", fHealth);
	float healthTextWidth = StringWidth(healthStr.String());

	// Shield icon and value
	BBitmap* shieldIcon = ResourceLoader::Instance()->GetGlyph("shield1");
	BString shieldStr;
	shieldStr.SetToFormat("%d", fShield);
	float shieldTextWidth = StringWidth(shieldStr.String());

	// Sword icon and value
	BBitmap* swordIcon = ResourceLoader::Instance()->GetGlyph("sword1");
	BString swordStr;
	swordStr.SetToFormat("%d", fSword);
	float swordTextWidth = StringWidth(swordStr.String());

	// Calculate positions for bottom row
	float bottomRowWidth = (iconSize + spacing + healthTextWidth) + groupSpacing +
		(iconSize + spacing + shieldTextWidth) + groupSpacing +
		(iconSize + spacing + swordTextWidth);
	float bottomRowStartX = centerX - bottomRowWidth / 2;

	x = bottomRowStartX;

	// Draw heart
	if (heartIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(heartIcon, heartIcon->Bounds(),
			BRect(x, bottomRowY, x + iconSize, bottomRowY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}
	x += iconSize + spacing;
	SetHighColor(kTextColor);
	DrawString(healthStr.String(), BPoint(x, bottomRowY + iconSize - 5));
	x += healthTextWidth + groupSpacing;

	// Draw shield
	if (shieldIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(shieldIcon, shieldIcon->Bounds(),
			BRect(x, bottomRowY, x + iconSize, bottomRowY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}
	x += iconSize + spacing;
	SetHighColor(kTextColor);
	DrawString(shieldStr.String(), BPoint(x, bottomRowY + iconSize - 5));
	x += shieldTextWidth + groupSpacing;

	// Draw sword
	if (swordIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(swordIcon, swordIcon->Bounds(),
			BRect(x, bottomRowY, x + iconSize, bottomRowY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}
	x += iconSize + spacing;
	SetHighColor(kTextColor);
	DrawString(swordStr.String(), BPoint(x, bottomRowY + iconSize - 5));
}


void
MainMenuView::SetSavedGameStats(int deckCount, int score, int dungeon,
	int health, int shield, int sword)
{
	fDeckCount = deckCount;
	fScore = score;
	fDungeon = dungeon;
	fHealth = health;
	fShield = shield;
	fSword = sword;
	Invalidate();
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
