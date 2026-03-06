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

	// Initial button positions (no saved game - centered layout)
	float startY = 280;
	float spacing = 55;

	// Resume button (initially hidden)
	fResumeButton = new PlankButtonMenu(
		BRect(buttonX, startY, buttonX + buttonWidth, startY + buttonHeight),
		"Resume", new BMessage(kMsgResumeGame));

	// New Game button
	fNewGameButton = new PlankButtonMenu(
		BRect(buttonX, startY, buttonX + buttonWidth, startY + buttonHeight),
		"New Game", new BMessage(kMsgNewGame));

	// How to Play button
	fHowToPlayButton = new PlankButtonMenu(
		BRect(buttonX, startY + spacing, buttonX + buttonWidth, startY + spacing + buttonHeight),
		"How to Play", new BMessage(kMsgHowToPlay));

	// High Scores button
	fHighScoresButton = new PlankButtonMenu(
		BRect(buttonX, startY + spacing * 2, buttonX + buttonWidth, startY + spacing * 2 + buttonHeight),
		"High Scores", new BMessage(kMsgHighScores));

	// Settings button
	fSettingsButton = new PlankButtonMenu(
		BRect(buttonX, startY + spacing * 3, buttonX + buttonWidth, startY + spacing * 3 + buttonHeight),
		"Settings", new BMessage(kMsgSettings));

	AddChild(fNewGameButton);
	AddChild(fHowToPlayButton);
	AddChild(fHighScoresButton);
	AddChild(fSettingsButton);
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
	float statsY = 95;  // Below title bar

	// Box dimensions
	float boxSize = 50;
	float boxSpacing = 10;
	float boxRadius = 8;
	float iconSize = 32;

	// Calculate grid position (3 columns, 2 rows)
	float gridWidth = boxSize * 3 + boxSpacing * 2;
	float gridStartX = centerX - gridWidth / 2;

	BFont font;
	font.SetSize(14);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	// Helper lambda to draw a stat box
	auto drawStatBox = [&](float x, float y, BBitmap* icon, const char* label,
		const char* value) {
		BRect boxRect(x, y, x + boxSize, y + boxSize);

		// Draw semi-transparent dark background
		SetDrawingMode(B_OP_ALPHA);
		SetHighColor(0, 0, 0, 160);
		FillRoundRect(boxRect, boxRadius, boxRadius);

		// Draw subtle border
		SetHighColor(80, 80, 80, 200);
		StrokeRoundRect(boxRect, boxRadius, boxRadius);
		SetDrawingMode(B_OP_COPY);

		// Draw icon centered horizontally, near top
		float iconX = x + (boxSize - iconSize) / 2;
		float iconY = y + 4;
		if (icon != NULL) {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(icon, icon->Bounds(),
				BRect(iconX, iconY, iconX + iconSize, iconY + iconSize));
			SetDrawingMode(B_OP_COPY);
		} else if (label != NULL) {
			// For "Score" which has no icon, draw text label
			float labelWidth = StringWidth(label);
			SetHighColor(180, 160, 130);
			DrawString(label, BPoint(x + (boxSize - labelWidth) / 2, iconY + 16));
		}

		// Draw value centered horizontally, at bottom
		float valueWidth = StringWidth(value);
		float valueX = x + (boxSize - valueWidth) / 2;
		float valueY = y + boxSize - 8;
		SetHighColor(kTextColor);
		DrawString(value, BPoint(valueX, valueY));
	};

	// Prepare values
	BString deckStr, scoreStr, dungeonStr, healthStr, shieldStr, swordStr;
	deckStr.SetToFormat("%d", fDeckCount);
	scoreStr.SetToFormat("%d", fScore);
	dungeonStr.SetToFormat("%d", fDungeon);
	healthStr.SetToFormat("%d", fHealth);
	shieldStr.SetToFormat("%d", fShield);
	swordStr.SetToFormat("%d", fSword);

	// Get icons
	BBitmap* deckIcon = ResourceLoader::Instance()->GetGlyph("deck");
	BBitmap* dungeonIcon = ResourceLoader::Instance()->GetGlyph("dungeonGlyph");
	BBitmap* heartIcon = ResourceLoader::Instance()->GetGlyph("heart1");
	BBitmap* shieldIcon = ResourceLoader::Instance()->GetGlyph("shield1");
	BBitmap* swordIcon = ResourceLoader::Instance()->GetGlyph("sword1");

	// Top row: Deck, Score, Dungeon
	float row1Y = statsY;
	drawStatBox(gridStartX, row1Y, deckIcon, NULL, deckStr.String());
	drawStatBox(gridStartX + boxSize + boxSpacing, row1Y, NULL, "Score",
		scoreStr.String());
	drawStatBox(gridStartX + (boxSize + boxSpacing) * 2, row1Y, dungeonIcon, NULL,
		dungeonStr.String());

	// Bottom row: Heart, Shield, Sword
	float row2Y = statsY + boxSize + boxSpacing;
	drawStatBox(gridStartX, row2Y, heartIcon, NULL, healthStr.String());
	drawStatBox(gridStartX + boxSize + boxSpacing, row2Y, shieldIcon, NULL,
		shieldStr.String());
	drawStatBox(gridStartX + (boxSize + boxSpacing) * 2, row2Y, swordIcon, NULL,
		swordStr.String());
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
	float buttonX = centerX - buttonWidth / 2;
	float spacing = 55;

	if (hasSaved) {
		// Add resume button and adjust other buttons
		// Stats grid ends around y=205, start buttons below
		if (fResumeButton->Parent() == NULL)
			AddChild(fResumeButton);

		float startY = 210;
		float btnSpacing = 48;
		float extraGap = 18;  // Extra space between New Game and How to Play
		float y = startY;
		fResumeButton->MoveTo(buttonX, y);
		y += 45 + btnSpacing;
		fNewGameButton->MoveTo(buttonX, y);
		y += 45 + btnSpacing + extraGap;  // Extra gap after New Game
		fHowToPlayButton->MoveTo(buttonX, y);
		y += 45 + btnSpacing;
		fHighScoresButton->MoveTo(buttonX, y);
		y += 45 + btnSpacing;
		fSettingsButton->MoveTo(buttonX, y);
	} else {
		// Remove resume button and move others up
		// Center buttons vertically between title bar (80) and bottom (600)
		// Available space: 520px, content height: ~310px
		if (fResumeButton->Parent() != NULL)
			fResumeButton->RemoveSelf();

		float btnSpacing = 55;
		float extraGap = 20;  // Extra space between New Game and How to Play
		float startY = 185;   // Centered position
		float y = startY;
		fNewGameButton->MoveTo(buttonX, y);
		y += 45 + btnSpacing + extraGap;  // Extra gap after New Game
		fHowToPlayButton->MoveTo(buttonX, y);
		y += 45 + btnSpacing;
		fHighScoresButton->MoveTo(buttonX, y);
		y += 45 + btnSpacing;
		fSettingsButton->MoveTo(buttonX, y);
	}

	Invalidate();
}
