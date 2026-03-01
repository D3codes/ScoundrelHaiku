#include "HighScoresWindow.h"
#include "helpers/HighScoreManager.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"

#include <Bitmap.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
#include <View.h>


// Title bar with close button
class HighScoresTitleBar : public BView {
public:
	HighScoresTitleBar(BRect frame)
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

		const char* title = "High Scores";
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
		float buttonSize = 30;
		float buttonMargin = 5;
		BRect closeRect(buttonMargin, (Bounds().Height() - buttonSize) / 2,
			buttonMargin + buttonSize, (Bounds().Height() + buttonSize) / 2);

		if (closeRect.Contains(where)) {
			Window()->PostMessage(B_QUIT_REQUESTED);
		}
	}
};


// Content view showing scores list
class HighScoresContentView : public BView {
public:
	HighScoresContentView(BRect frame)
		: BView(frame, "scoresContent", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(222, 210, 190);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Fill background
		SetHighColor(222, 210, 190);
		FillRect(bounds);

		HighScoreManager* mgr = HighScoreManager::Instance();
		int count = mgr->GetScoreCount();

		BFont font;
		font.SetSize(14);
		SetFont(&font);

		float y = 25;
		float lineHeight = 22;
		float padding = 15;

		if (count == 0) {
			SetHighColor(kDarkTextColor);
			const char* noScores = "No high scores yet!";
			float textWidth = StringWidth(noScores);
			DrawString(noScores, BPoint((bounds.Width() - textWidth) / 2, y));
			return;
		}

		// Draw header
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);
		SetHighColor(kDarkTextColor);

		DrawString("#", BPoint(padding, y));
		DrawString("Name", BPoint(padding + 30, y));
		DrawString("Score", BPoint(bounds.Width() - padding - 80, y));
		DrawString("Dng", BPoint(bounds.Width() - padding - 25, y));

		y += lineHeight;

		// Draw separator line
		SetHighColor(150, 140, 120);
		StrokeLine(BPoint(padding, y - 8), BPoint(bounds.Width() - padding, y - 8));

		// Draw scores
		font.SetFace(0);
		SetFont(&font);

		for (int i = 0; i < count && i < 15; i++) {
			HighScoreEntry* entry = mgr->GetScore(i);

			SetHighColor(kDarkTextColor);

			// Rank
			BString rankStr;
			rankStr.SetToFormat("%d.", i + 1);
			DrawString(rankStr.String(), BPoint(padding, y));

			// Name (truncate if too long)
			BString name = entry->name;
			float maxNameWidth = bounds.Width() - padding - 30 - 110;
			while (StringWidth(name.String()) > maxNameWidth && name.Length() > 0) {
				name.Truncate(name.Length() - 1);
			}
			DrawString(name.String(), BPoint(padding + 30, y));

			// Score
			BString scoreStr;
			scoreStr.SetToFormat("%d", entry->score);
			float scoreWidth = StringWidth(scoreStr.String());
			DrawString(scoreStr.String(), BPoint(bounds.Width() - padding - 50 - scoreWidth, y));

			// Dungeons
			BString dungeonStr;
			dungeonStr.SetToFormat("%d", entry->dungeonsBeaten);
			float dungeonWidth = StringWidth(dungeonStr.String());
			DrawString(dungeonStr.String(), BPoint(bounds.Width() - padding - dungeonWidth, y));

			y += lineHeight;
		}
	}
};


HighScoresWindow::HighScoresWindow(BWindow* parent)
	:
	BWindow(BRect(0, 0, 280, 400), "High Scores",
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

	// Create title bar
	BRect titleRect(0, 0, Bounds().Width(), titleBarHeight);
	HighScoresTitleBar* titleBar = new HighScoresTitleBar(titleRect);
	AddChild(titleBar);

	// Create content view
	BRect contentRect(0, titleBarHeight, Bounds().Width(), Bounds().Height());
	HighScoresContentView* contentView = new HighScoresContentView(contentRect);
	AddChild(contentView);
}


HighScoresWindow::~HighScoresWindow()
{
}


void
HighScoresWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
