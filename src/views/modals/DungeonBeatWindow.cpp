#include "DungeonBeatWindow.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <String.h>
#include <View.h>


// Title bar
class DungeonBeatTitleBar : public BView {
public:
	DungeonBeatTitleBar(BRect frame)
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

		const char* title = "Dungeon Cleared!";
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
class DungeonBeatContentView : public BView {
public:
	DungeonBeatContentView(BRect frame, int score, int dungeonDepth)
		: BView(frame, "dungeonBeatContent", B_FOLLOW_ALL, B_WILL_DRAW),
		  fScore(score),
		  fDungeonDepth(dungeonDepth)
	{
		SetViewColor(222, 210, 190);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		SetHighColor(222, 210, 190);
		FillRect(bounds);

		float leftMargin = 30;
		float rightMargin = bounds.Width() - 30;

		BFont font;
		font.SetSize(16);
		SetFont(&font);
		SetHighColor(kDarkTextColor);

		// Score - label left, value right
		DrawString("Score:", BPoint(leftMargin, 30));
		BString scoreValue;
		scoreValue.SetToFormat("%d", fScore);
		float scoreWidth = StringWidth(scoreValue.String());
		DrawString(scoreValue.String(), BPoint(rightMargin - scoreWidth, 30));

		// Dungeons - label left, value right (shows completed count)
		DrawString("Dungeons:", BPoint(leftMargin, 52));
		BString dungeonValue;
		dungeonValue.SetToFormat("%d", fDungeonDepth + 1);
		float dungeonWidth = StringWidth(dungeonValue.String());
		DrawString(dungeonValue.String(), BPoint(rightMargin - dungeonWidth, 52));
	}

private:
	int fScore;
	int fDungeonDepth;
};


// Plank button
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


DungeonBeatWindow::DungeonBeatWindow(BWindow* parent, int score, int dungeonDepth)
	:
	BWindow(BRect(0, 0, 250, 175), "Dungeon Complete",
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
	float windowWidth = Bounds().Width();

	// Create title bar
	BRect titleRect(0, 0, windowWidth, titleBarHeight);
	DungeonBeatTitleBar* titleBar = new DungeonBeatTitleBar(titleRect);
	AddChild(titleBar);

	// Create content view
	BRect contentRect(0, titleBarHeight, windowWidth, Bounds().Height());
	DungeonBeatContentView* contentView = new DungeonBeatContentView(contentRect,
		score, dungeonDepth);

	// Add button
	float buttonWidth = 140;
	float buttonHeight = 36;
	float buttonX = (windowWidth - buttonWidth) / 2;

	PlankButtonDB* continueBtn = new PlankButtonDB(
		BRect(buttonX, 70, buttonX + buttonWidth, 70 + buttonHeight),
		"Next Dungeon", new BMessage(kMsgNextDungeon));
	contentView->AddChild(continueBtn);

	AddChild(contentView);
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
