#include "NameEntryWindow.h"
#include "helpers/HighScoreManager.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <String.h>
#include <TextControl.h>
#include <View.h>

static const uint32 kMsgMainMenuWithSave = 'MMWS';
static const uint32 kMsgNewGameWithSave = 'NGWS';


// Title bar
class GameOverTitleBar : public BView {
public:
	GameOverTitleBar(BRect frame)
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

		// Draw title centered
		BFont font;
		font.SetSize(20);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		const char* title = "Game Over";
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
};


// Content view
class GameOverContentView : public BView {
public:
	GameOverContentView(BRect frame, int score, int dungeonsBeaten, bool isHighScore)
		: BView(frame, "gameOverContent", B_FOLLOW_ALL, B_WILL_DRAW),
		  fScore(score),
		  fDungeonsBeaten(dungeonsBeaten),
		  fIsHighScore(isHighScore)
	{
		SetViewColor(222, 210, 190);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		SetHighColor(222, 210, 190);
		FillRect(bounds);

		float leftMargin = 30;
		float rightMargin = bounds.Width() - 30;
		float y = 30;

		BFont font;

		// Show "New High Score!" if applicable
		if (fIsHighScore) {
			font.SetSize(18);
			font.SetFace(B_BOLD_FACE);
			SetFont(&font);
			SetHighColor(180, 100, 40);  // Golden/orange color

			const char* highScoreText = "New High Score!";
			float textWidth = StringWidth(highScoreText);
			DrawString(highScoreText, BPoint((bounds.Width() - textWidth) / 2, y));
			y += 28;
		}

		font.SetSize(16);
		font.SetFace(0);
		SetFont(&font);
		SetHighColor(kDarkTextColor);

		// Score - label left, value right
		DrawString("Score:", BPoint(leftMargin, y));
		BString scoreValue;
		scoreValue.SetToFormat("%d", fScore);
		float scoreWidth = StringWidth(scoreValue.String());
		DrawString(scoreValue.String(), BPoint(rightMargin - scoreWidth, y));
		y += 22;

		// Dungeons - label left, value right
		DrawString("Dungeons:", BPoint(leftMargin, y));
		BString dungeonValue;
		dungeonValue.SetToFormat("%d", fDungeonsBeaten);
		float dungeonWidth = StringWidth(dungeonValue.String());
		DrawString(dungeonValue.String(), BPoint(rightMargin - dungeonWidth, y));

		// Prompt (only if high score)
		if (fIsHighScore) {
			y += 30;
			font.SetSize(14);
			SetFont(&font);
			const char* prompt = "Enter your name:";
			float promptWidth = StringWidth(prompt);
			DrawString(prompt, BPoint((bounds.Width() - promptWidth) / 2, y));
		}
	}

private:
	int fScore;
	int fDungeonsBeaten;
	bool fIsHighScore;
};


// Plank button for game over modal
class PlankButtonGameOver : public BView {
public:
	PlankButtonGameOver(BRect frame, const char* label, BMessage* message)
		: BView(frame, "plankBtn", B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual ~PlankButtonGameOver() {
		delete fMessage;
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();
		float radius = 8;

		// Draw solid rounded background
		SetHighColor(100, 70, 50);
		FillRoundRect(bounds, radius, radius);

		// Draw plank background
		BBitmap* plankBg = ResourceLoader::Instance()->GetUIImage("plank1");
		if (plankBg != NULL) {
			BRect insetBounds = bounds.InsetByCopy(2, 2);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(plankBg, plankBg->Bounds(), insetBounds);
			SetDrawingMode(B_OP_COPY);
		}

		// Draw border
		SetHighColor(120, 90, 60);
		StrokeRoundRect(bounds, radius, radius);

		// Draw text
		BFont font;
		font.SetSize(18);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float textWidth = StringWidth(fLabel.String());
		float textX = (bounds.Width() - textWidth) / 2;
		float textY = bounds.Height() / 2 + 6;

		// Shadow
		SetHighColor(0, 0, 0, 180);
		DrawString(fLabel.String(), BPoint(textX + 2, textY + 2));

		// Text
		SetHighColor(kTextColor);
		DrawString(fLabel.String(), BPoint(textX, textY));
	}

	virtual void MouseDown(BPoint where) {
		Window()->PostMessage(new BMessage(fMessage->what));
	}

private:
	BString fLabel;
	BMessage* fMessage;
};


NameEntryWindow::NameEntryWindow(BWindow* parent, int score, int dungeonsBeaten,
	bool isHighScore)
	:
	BWindow(BRect(0, 0, 250, isHighScore ? 288 : 175), "Game Over",
		B_MODAL_WINDOW_LOOK, B_MODAL_SUBSET_WINDOW_FEEL,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fParent(parent),
	fNameInput(NULL),
	fScore(score),
	fDungeonsBeaten(dungeonsBeaten),
	fIsHighScore(isHighScore)
{
	AddToSubset(parent);

	// Center on parent
	BRect parentFrame = parent->Frame();
	BRect frame = Frame();
	float x = parentFrame.left + (parentFrame.Width() - frame.Width()) / 2;
	float y = parentFrame.top + (parentFrame.Height() - frame.Height()) / 2;
	MoveTo(x, y);

	float titleBarHeight = 40;
	float windowWidth = Bounds().Width();

	// Create title bar
	BRect titleRect(0, 0, windowWidth, titleBarHeight);
	GameOverTitleBar* titleBar = new GameOverTitleBar(titleRect);
	AddChild(titleBar);

	// Create content view
	BRect contentRect(0, titleBarHeight, windowWidth, Bounds().Height());
	GameOverContentView* contentView = new GameOverContentView(contentRect,
		score, dungeonsBeaten, isHighScore);

	float buttonWidth = 140;
	float buttonHeight = 36;
	float buttonX = (windowWidth - buttonWidth) / 2;

	if (isHighScore) {
		// Determine default name - use last used name or fallback to "Player"
		const char* defaultName = HighScoreManager::Instance()->GetLastUsedName();
		if (defaultName == NULL)
			defaultName = "Player";

		// Add text input
		float inputWidth = 180;
		float inputX = (windowWidth - inputWidth) / 2;
		fNameInput = new BTextControl(BRect(inputX, 123, inputX + inputWidth, 148),
			"nameInput", NULL, defaultName, NULL);
		fNameInput->SetDivider(0);
		contentView->AddChild(fNameInput);

		// Add buttons (lower position for high score layout)
		PlankButtonGameOver* mainMenuBtn = new PlankButtonGameOver(
			BRect(buttonX, 160, buttonX + buttonWidth, 160 + buttonHeight),
			"Main Menu", new BMessage(kMsgMainMenuWithSave));
		contentView->AddChild(mainMenuBtn);

		PlankButtonGameOver* newGameBtn = new PlankButtonGameOver(
			BRect(buttonX, 205, buttonX + buttonWidth, 205 + buttonHeight),
			"New Game", new BMessage(kMsgNewGameWithSave));
		contentView->AddChild(newGameBtn);
	} else {
		// No name input - just buttons (compact layout)
		PlankButtonGameOver* mainMenuBtn = new PlankButtonGameOver(
			BRect(buttonX, 55, buttonX + buttonWidth, 55 + buttonHeight),
			"Main Menu", new BMessage(kMsgMainMenuWithSave));
		contentView->AddChild(mainMenuBtn);

		PlankButtonGameOver* newGameBtn = new PlankButtonGameOver(
			BRect(buttonX, 100, buttonX + buttonWidth, 100 + buttonHeight),
			"New Game", new BMessage(kMsgNewGameWithSave));
		contentView->AddChild(newGameBtn);
	}

	AddChild(contentView);

	// Focus the text input if it exists
	if (fNameInput != NULL)
		fNameInput->MakeFocus(true);
}


NameEntryWindow::~NameEntryWindow()
{
}


void
NameEntryWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgMainMenuWithSave:
		{
			// Save the score only if it's a high score
			if (fIsHighScore && fNameInput != NULL) {
				const char* name = fNameInput->Text();
				if (name == NULL || name[0] == '\0')
					name = "Player";
				HighScoreManager::Instance()->AddScore(name, fScore, fDungeonsBeaten);
			}

			// Go to main menu
			fParent->PostMessage(new BMessage(kMsgMainMenu));
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case kMsgNewGameWithSave:
		{
			// Save the score only if it's a high score
			if (fIsHighScore && fNameInput != NULL) {
				const char* name = fNameInput->Text();
				if (name == NULL || name[0] == '\0')
					name = "Player";
				HighScoreManager::Instance()->AddScore(name, fScore, fDungeonsBeaten);
			}

			// Start new game
			fParent->PostMessage(new BMessage(kMsgNewGame));
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
