#include "TopBarView.h"
#include "models/Game.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <String.h>
#include <Window.h>

#include <string.h>


// Custom stone-style button
class StoneButton : public BView {
public:
	StoneButton(BRect frame, const char* name, const char* label, BMessage* message)
		: BView(frame, name, B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message),
		  fEnabled(true)
	{
		// Transparent so parent's stone slab shows through corners
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual ~StoneButton() {
		delete fMessage;
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();
		float radius = 10;

		// Draw stone button texture with rounded corners
		BBitmap* stoneBg = ResourceLoader::Instance()->GetUIImage("stoneButton");
		if (stoneBg != NULL) {
			// Draw solid rounded background first (darker stone color)
			SetHighColor(60, 60, 70);
			FillRoundRect(bounds, radius, radius);

			// Draw the bitmap inset so rounded corners show
			BRect insetBounds = bounds.InsetByCopy(2, 2);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(stoneBg, stoneBg->Bounds(), insetBounds);
			SetDrawingMode(B_OP_COPY);

			// Draw rounded border
			SetHighColor(80, 80, 90);
			StrokeRoundRect(bounds, radius, radius);
		} else {
			// Fallback: simple rounded button
			SetHighColor(70, 70, 80);
			FillRoundRect(bounds, radius, radius);
			SetHighColor(90, 90, 100);
			StrokeRoundRect(bounds, radius, radius);
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
			// Disabled: black text
			SetHighColor(0, 0, 0);
		}
		DrawString(fLabel.String(), BPoint(textX, textY));

		// Draw slash over button when disabled
		if (!fEnabled) {
			SetPenSize(4);

			// Shadow
			SetHighColor(0, 0, 0, 180);
			StrokeLine(BPoint(2, bounds.Height() + 2),
				BPoint(bounds.Width() + 2, 2));

			// White slash
			SetHighColor(255, 255, 255);
			StrokeLine(BPoint(0, bounds.Height()),
				BPoint(bounds.Width(), 0));

			SetPenSize(1);
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
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_DRAW_ON_CHILDREN),
	fGame(NULL),
	fVisualDeckCount(0),
	fShowSlash(false)
{
	// Solid color fallback - Draw() will paint over it
	SetViewColor(70, 70, 90);

	float buttonSize = 50;
	float padding = 10;

	// Create pause button (stone style)
	fPauseButton = new StoneButton(
		BRect(padding, 15, padding + buttonSize, 15 + buttonSize),
		"pauseBtn", "||", new BMessage(kMsgPause));
	AddChild(fPauseButton);

	// Create flee button (stone style)
	float fleeX = frame.Width() - padding - buttonSize;
	float fleeY = 15;
	fFleeButton = new StoneButton(
		BRect(fleeX, fleeY, fleeX + buttonSize, fleeY + buttonSize),
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

	// Calculate positions for icon boxes - centered between buttons
	float iconBoxSize = 50;
	float buttonSize = 50;
	float padding = 10;
	float spacing = 8;
	float scoreBoxWidth = 80;

	// Total width of all three boxes
	float totalBoxesWidth = iconBoxSize + spacing + scoreBoxWidth + spacing + iconBoxSize;

	// Center the boxes between pause and flee buttons
	float leftEdge = padding + buttonSize + spacing;
	float rightEdge = bounds.Width() - padding - buttonSize - spacing;
	float availableWidth = rightEdge - leftEdge;
	float startX = leftEdge + (availableWidth - totalBoxesWidth) / 2;
	float boxY = 15;

	// Draw deck icon box (use visual count for animation sync)
	BRect deckBoxRect(startX, boxY, startX + iconBoxSize, boxY + iconBoxSize);
	DrawIconBox(deckBoxRect, "deck", fVisualDeckCount);

	// Draw score box (wider) - centered
	float scoreX = deckBoxRect.right + spacing;
	BRect scoreBoxRect(scoreX, boxY, scoreX + scoreBoxWidth, boxY + iconBoxSize);
	DrawScoreBox(scoreBoxRect);

	// Draw dungeon icon box (shows dungeons beaten, starts at 0)
	float dungeonX = scoreBoxRect.right + spacing;
	BRect dungeonBoxRect(dungeonX, boxY, dungeonX + iconBoxSize, boxY + iconBoxSize);
	DrawIconBox(dungeonBoxRect, "dungeonGlyph",
		fGame != NULL ? fGame->DungeonDepth() : 0);

	// Draw slash over flee button if needed
	if (fShowSlash)
		DrawFleeSlash();
}


void
TopBarView::DrawIconBox(BRect boxRect, const char* iconName, int value)
{
	float radius = 8;

	// Draw semi-transparent dark background (matching main menu style)
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(0, 0, 0, 160);
	FillRoundRect(boxRect, radius, radius);

	// Draw subtle border
	SetHighColor(80, 80, 80, 200);
	StrokeRoundRect(boxRect, radius, radius);
	SetDrawingMode(B_OP_COPY);

	// Draw icon - deck icon is larger
	float iconSize = 32;
	if (strcmp(iconName, "deck") == 0)
		iconSize = 36;

	float iconX = boxRect.left + (boxRect.Width() - iconSize) / 2;
	float iconY = boxRect.top + 4;

	BBitmap* icon = ResourceLoader::Instance()->GetGlyph(iconName);
	if (icon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(icon, BRect(0, 0, icon->Bounds().Width(), icon->Bounds().Height()),
			BRect(iconX, iconY, iconX + iconSize, iconY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}

	// Draw value text below icon
	BFont font;
	font.SetSize(14);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);
	SetHighColor(kTextColor);

	BString valueStr;
	valueStr.SetToFormat("%d", value);
	float textWidth = StringWidth(valueStr.String());
	float textX = boxRect.left + (boxRect.Width() - textWidth) / 2;
	float textY = boxRect.bottom - 6;
	DrawString(valueStr.String(), BPoint(textX, textY));
}


void
TopBarView::DrawScoreBox(BRect boxRect)
{
	float radius = 8;

	// Draw semi-transparent dark background (matching main menu style)
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(0, 0, 0, 160);
	FillRoundRect(boxRect, radius, radius);

	// Draw subtle border
	SetHighColor(80, 80, 80, 200);
	StrokeRoundRect(boxRect, radius, radius);
	SetDrawingMode(B_OP_COPY);

	// Draw "Score" label
	BFont font;
	font.SetSize(14);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);
	SetHighColor(kTextColor);

	const char* label = "Score";
	float labelWidth = StringWidth(label);
	float labelX = boxRect.left + (boxRect.Width() - labelWidth) / 2;
	DrawString(label, BPoint(labelX, boxRect.top + 20));

	// Draw score value
	font.SetSize(14);
	SetFont(&font);
	SetHighColor(kTextColor);

	int score = fGame != NULL ? fGame->Score() : 0;
	BString scoreStr;
	scoreStr.SetToFormat("%d", score);
	float textWidth = StringWidth(scoreStr.String());
	float textX = boxRect.left + (boxRect.Width() - textWidth) / 2;
	DrawString(scoreStr.String(), BPoint(textX, boxRect.bottom - 6));
}


void
TopBarView::DrawFleeSlash()
{
	// Get flee button frame and expand it for the slash
	BRect slashRect = fFleeButton->Frame();
	slashRect.InsetBy(-10, -10);

	float inset = 6;
	SetPenSize(4);

	// Shadow
	SetHighColor(0, 0, 0, 180);
	StrokeLine(BPoint(slashRect.left + inset + 2, slashRect.bottom - inset + 2),
		BPoint(slashRect.right - inset + 2, slashRect.top + inset + 2));

	// White slash
	SetHighColor(255, 255, 255);
	StrokeLine(BPoint(slashRect.left + inset, slashRect.bottom - inset),
		BPoint(slashRect.right - inset, slashRect.top + inset));

	SetPenSize(1);
}


void
TopBarView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgDeckCountChanged:
		{
			int32 delta;
			if (message->FindInt32("delta", &delta) == B_OK) {
				AdjustVisualDeckCount(delta);
			}
			break;
		}
		default:
			BView::MessageReceived(message);
			break;
	}
}


void
TopBarView::SetGame(Game* game)
{
	fGame = game;
	SyncVisualDeckCount();
	Refresh();
}


void
TopBarView::Refresh()
{
	if (fGame == NULL)
		return;

	// Update flee button state
	bool canFlee = fGame->GetRoom()->CanFlee();
	StoneButton* fleeBtn = dynamic_cast<StoneButton*>(fFleeButton);
	if (fleeBtn != NULL)
		fleeBtn->SetEnabled(canFlee);

	// Update slash visibility
	fShowSlash = !canFlee;

	Invalidate();
}


void
TopBarView::SetVisualDeckCount(int count)
{
	fVisualDeckCount = count;
	Invalidate();
}


void
TopBarView::AdjustVisualDeckCount(int delta)
{
	fVisualDeckCount += delta;
	if (fVisualDeckCount < 0)
		fVisualDeckCount = 0;
	Invalidate();
}


void
TopBarView::SyncVisualDeckCount()
{
	// Sync visual count with actual deck count
	if (fGame != NULL)
		fVisualDeckCount = fGame->GetDeck()->CardsRemaining();
	else
		fVisualDeckCount = 0;
}
