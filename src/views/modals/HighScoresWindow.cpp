#include "HighScoresWindow.h"
#include "helpers/HighScoreManager.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Screen.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
#include <View.h>


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

		// Draw separator line right after header
		SetHighColor(150, 140, 120);
		StrokeLine(BPoint(padding, y + 5), BPoint(bounds.Width() - padding, y + 5));

		y += lineHeight;

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
		B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fParent(parent)
{
	// Center on screen
	BRect screenFrame = BScreen().Frame();
	BRect frame = Frame();
	float x = (screenFrame.Width() - frame.Width()) / 2;
	float y = (screenFrame.Height() - frame.Height()) / 2;
	MoveTo(x, y);

	// Create content view (no custom title bar - use system title bar)
	BRect contentRect = Bounds();
	fContentView = new HighScoresContentView(contentRect);
	AddChild(fContentView);
}


HighScoresWindow::~HighScoresWindow()
{
}


bool
HighScoresWindow::QuitRequested()
{
	if (fParent != NULL)
		fParent->PostMessage(kMsgHighScoresClosed);
	return true;
}


void
HighScoresWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgHighScoresUpdated:
			if (fContentView != NULL)
				fContentView->Invalidate();
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
