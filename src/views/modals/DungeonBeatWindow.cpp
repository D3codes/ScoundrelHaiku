#include "DungeonBeatWindow.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Button.h>
#include <StringView.h>
#include <View.h>

class DungeonBeatView : public BView {
public:
	DungeonBeatView(BRect frame, int score, int dungeonDepth)
		: BView(frame, "dungeonBeatView", B_FOLLOW_ALL, B_WILL_DRAW),
		  fScore(score),
		  fDungeonDepth(dungeonDepth)
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

		// Draw title "Dungeon" and "Cleared!"
		BFont titleFont;
		titleFont.SetSize(36);
		titleFont.SetFace(B_BOLD_FACE);
		SetFont(&titleFont);
		SetHighColor(kTextColor);

		const char* line1 = "Dungeon";
		const char* line2 = "Cleared!";
		float line1Width = StringWidth(line1);
		float line2Width = StringWidth(line2);

		DrawString(line1, BPoint(bounds.Width() / 2 - line1Width / 2,
			modalRect.top + 50));
		DrawString(line2, BPoint(bounds.Width() / 2 - line2Width / 2,
			modalRect.top + 90));

		// Draw score
		BFont bodyFont;
		bodyFont.SetSize(24);
		SetFont(&bodyFont);
		SetHighColor(kDarkTextColor);

		BString scoreLabel = "Score:";
		BString scoreValue;
		scoreValue.SetToFormat("%d", fScore);

		float labelX = modalRect.left + 50;
		float valueX = modalRect.right - 50 - StringWidth(scoreValue.String());
		float scoreY = modalRect.top + 150;

		DrawString(scoreLabel.String(), BPoint(labelX, scoreY));
		DrawString(scoreValue.String(), BPoint(valueX, scoreY));

		// Draw dungeons (shows next dungeon number)
		BString dungeonLabel = "Dungeons:";
		BString dungeonValue;
		dungeonValue.SetToFormat("%d", fDungeonDepth + 1);

		valueX = modalRect.right - 50 - StringWidth(dungeonValue.String());
		float dungeonY = scoreY + 40;

		DrawString(dungeonLabel.String(), BPoint(labelX, dungeonY));
		DrawString(dungeonValue.String(), BPoint(valueX, dungeonY));
	}

private:
	int fScore;
	int fDungeonDepth;
};


class PlankButtonDB : public BView {
public:
	PlankButtonDB(BRect frame, const char* label, BMessage* message)
		: BView(frame, "plankBtn", B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual ~PlankButtonDB() {
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


DungeonBeatWindow::DungeonBeatWindow(BWindow* parent, int score, int dungeonDepth)
	:
	BWindow(BRect(0, 0, 300, 400), "Dungeon Complete",
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
	DungeonBeatView* mainView = new DungeonBeatView(bounds, score, dungeonDepth);

	// Continue button
	float buttonWidth = 200;
	float buttonHeight = 40;
	float centerX = bounds.Width() / 2;

	PlankButtonDB* continueBtn = new PlankButtonDB(
		BRect(centerX - buttonWidth / 2, 320,
			centerX + buttonWidth / 2, 320 + buttonHeight),
		"Next Dungeon", new BMessage(kMsgNextDungeon));
	mainView->AddChild(continueBtn);

	AddChild(mainView);
}


DungeonBeatWindow::~DungeonBeatWindow()
{
}


void
DungeonBeatWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgNextDungeon:
			fParent->PostMessage(message);
			PostMessage(B_QUIT_REQUESTED);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
