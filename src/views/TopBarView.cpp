#include "TopBarView.h"
#include "models/Game.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Window.h>


// Custom stone-style button
class StoneButton : public BView {
public:
	StoneButton(BRect frame, const char* name, const char* label, BMessage* message)
		: BView(frame, name, B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message),
		  fEnabled(true)
	{
		SetViewColor(80, 80, 90);
	}

	virtual ~StoneButton() {
		delete fMessage;
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Always draw solid background first
		SetHighColor(80, 80, 90);
		FillRoundRect(bounds, 8, 8);

		BBitmap* stoneBg = ResourceLoader::Instance()->GetUIImage("stoneButton");
		if (stoneBg != NULL) {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(stoneBg, stoneBg->Bounds(), bounds);
			SetDrawingMode(B_OP_COPY);
		}

		// Draw label
		BFont font;
		font.SetSize(16);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float textWidth = StringWidth(fLabel.String());
		float textX = (bounds.Width() - textWidth) / 2;
		float textY = bounds.Height() / 2 + 5;

		if (fEnabled) {
			// Shadow
			SetHighColor(0, 0, 0, 150);
			DrawString(fLabel.String(), BPoint(textX + 1, textY + 1));
			// Text
			SetHighColor(kTextColor);
		} else {
			SetHighColor(60, 60, 60);
		}
		DrawString(fLabel.String(), BPoint(textX, textY));

		// Draw disabled overlay
		if (!fEnabled) {
			SetHighColor(128, 128, 128, 100);
			FillRoundRect(bounds, 8, 8);
		}
	}

	virtual void MouseDown(BPoint where) {
		if (fEnabled)
			Window()->PostMessage(fMessage);
	}

	void SetEnabled(bool enabled) {
		if (fEnabled != enabled) {
			fEnabled = enabled;
			Invalidate();
		}
	}

	bool IsEnabled() const { return fEnabled; }

private:
	BString fLabel;
	BMessage* fMessage;
	bool fEnabled;
};


TopBarView::TopBarView(BRect frame)
	:
	BView(frame, "topBarView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fGame(NULL)
{
	// Solid color fallback - Draw() will paint over it
	SetViewColor(70, 70, 90);

	float buttonSize = 50;
	float iconBoxSize = 50;
	float padding = 10;

	// Create pause button (stone style)
	fPauseButton = new StoneButton(
		BRect(padding, 15, padding + buttonSize, 15 + buttonSize),
		"pauseBtn", "||", new BMessage(kMsgPause));
	AddChild(fPauseButton);

	// Create flee button (stone style)
	fFleeButton = new StoneButton(
		BRect(frame.Width() - padding - buttonSize, 15,
			frame.Width() - padding, 15 + buttonSize),
		"fleeBtn", "Run", new BMessage(kMsgFlee));
	AddChild(fFleeButton);
}


TopBarView::~TopBarView()
{
}


void
TopBarView::AttachedToWindow()
{
	BView::AttachedToWindow();
}


void
TopBarView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();

	// Draw stone slab background
	BBitmap* stoneBg = ResourceLoader::Instance()->GetUIImage("stoneSlab2");
	if (stoneBg != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(stoneBg, stoneBg->Bounds(), bounds);
		SetDrawingMode(B_OP_COPY);
	} else {
		// Fallback gradient
		rgb_color topColor = {70, 70, 90, 255};
		rgb_color bottomColor = {90, 90, 110, 255};

		for (float y = bounds.top; y <= bounds.bottom; y++) {
			float ratio = (y - bounds.top) / (bounds.bottom - bounds.top);
			rgb_color color;
			color.red = topColor.red + (bottomColor.red - topColor.red) * ratio;
			color.green = topColor.green + (bottomColor.green - topColor.green) * ratio;
			color.blue = topColor.blue + (bottomColor.blue - topColor.blue) * ratio;
			color.alpha = 255;
			SetHighColor(color);
			StrokeLine(BPoint(bounds.left, y), BPoint(bounds.right, y));
		}
	}

	// Draw shadow at bottom
	SetHighColor(0, 0, 0, 100);
	StrokeLine(BPoint(bounds.left, bounds.bottom),
		BPoint(bounds.right, bounds.bottom));
	StrokeLine(BPoint(bounds.left, bounds.bottom - 1),
		BPoint(bounds.right, bounds.bottom - 1));

	// Calculate positions for icon boxes
	float iconBoxSize = 50;
	float buttonSize = 50;
	float padding = 10;
	float spacing = 8;

	float startX = padding + buttonSize + spacing;
	float boxY = 15;

	// Draw deck icon box
	BRect deckBoxRect(startX, boxY, startX + iconBoxSize, boxY + iconBoxSize);
	DrawIconBox(deckBoxRect, "deck",
		fGame != NULL ? fGame->GetDeck()->CardsRemaining() : 0);

	// Draw score box (wider)
	float scoreBoxWidth = 80;
	float scoreX = deckBoxRect.right + spacing;
	BRect scoreBoxRect(scoreX, boxY, scoreX + scoreBoxWidth, boxY + iconBoxSize);
	DrawScoreBox(scoreBoxRect);

	// Draw dungeon icon box
	float dungeonX = scoreBoxRect.right + spacing;
	BRect dungeonBoxRect(dungeonX, boxY, dungeonX + iconBoxSize, boxY + iconBoxSize);
	DrawIconBox(dungeonBoxRect, "dungeonGlyph",
		fGame != NULL ? fGame->DungeonDepth() + 1 : 1);
}


void
TopBarView::DrawIconBox(BRect boxRect, const char* iconName, int value)
{
	// Draw glass-like background
	SetHighColor(60, 60, 70, 180);
	FillRoundRect(boxRect, 10, 10);

	// Draw subtle border
	SetHighColor(80, 80, 90);
	StrokeRoundRect(boxRect, 10, 10);

	// Draw shadow
	SetHighColor(0, 0, 0, 80);
	BRect shadowRect = boxRect;
	shadowRect.OffsetBy(2, 2);
	StrokeRoundRect(shadowRect, 10, 10);

	// Draw icon
	float iconSize = 30;
	float iconX = boxRect.left + (boxRect.Width() - iconSize) / 2;
	float iconY = boxRect.top + 5;

	BBitmap* icon = ResourceLoader::Instance()->GetGlyph(iconName);
	if (icon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(icon, BRect(0, 0, icon->Bounds().Width(), icon->Bounds().Height()),
			BRect(iconX, iconY, iconX + iconSize, iconY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}

	// Draw value text below icon
	BFont font;
	font.SetSize(18);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);
	SetHighColor(kTextColor);

	BString valueStr;
	valueStr.SetToFormat("%d", value);
	float textWidth = StringWidth(valueStr.String());
	float textX = boxRect.left + (boxRect.Width() - textWidth) / 2;
	float textY = boxRect.bottom - 5;
	DrawString(valueStr.String(), BPoint(textX, textY));
}


void
TopBarView::DrawScoreBox(BRect boxRect)
{
	// Draw glass-like background
	SetHighColor(60, 60, 70, 180);
	FillRoundRect(boxRect, 10, 10);

	// Draw subtle border
	SetHighColor(80, 80, 90);
	StrokeRoundRect(boxRect, 10, 10);

	// Draw shadow
	SetHighColor(0, 0, 0, 80);
	BRect shadowRect = boxRect;
	shadowRect.OffsetBy(2, 2);
	StrokeRoundRect(shadowRect, 10, 10);

	// Draw "Score" label
	BFont font;
	font.SetSize(14);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);
	SetHighColor(kTextColor);

	const char* label = "Score";
	float labelWidth = StringWidth(label);
	float labelX = boxRect.left + (boxRect.Width() - labelWidth) / 2;
	DrawString(label, BPoint(labelX, boxRect.top + 18));

	// Draw score value
	font.SetSize(18);
	SetFont(&font);

	int score = fGame != NULL ? fGame->Score() : 0;
	BString scoreStr;
	scoreStr.SetToFormat("%d", score);
	float textWidth = StringWidth(scoreStr.String());
	float textX = boxRect.left + (boxRect.Width() - textWidth) / 2;
	DrawString(scoreStr.String(), BPoint(textX, boxRect.bottom - 5));
}


void
TopBarView::SetGame(Game* game)
{
	fGame = game;
	Refresh();
}


void
TopBarView::Refresh()
{
	if (fGame == NULL)
		return;

	// Update flee button state
	StoneButton* fleeBtn = dynamic_cast<StoneButton*>(fFleeButton);
	if (fleeBtn != NULL)
		fleeBtn->SetEnabled(fGame->GetRoom()->CanFlee());

	Invalidate();
}
