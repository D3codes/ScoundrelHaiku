#include "HowToPlayWindow.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <ScrollView.h>
#include <String.h>
#include <View.h>

class HowToPlayContentView : public BView {
public:
	HowToPlayContentView(BRect frame)
		: BView(frame, "howToContent", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		// Use parchment-like color for clean scrolling
		SetViewColor(222, 210, 190);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Draw all the text content
		float y = 50;
		float leftMargin = 20;
		float rightMargin = bounds.Width() - 20;
		float textWidth = rightMargin - leftMargin;

		// Title
		BFont titleFont;
		titleFont.SetSize(32);
		titleFont.SetFace(B_BOLD_FACE);
		SetFont(&titleFont);
		SetHighColor(kDarkTextColor);

		const char* title = "How to Play";
		float titleWidth = StringWidth(title);
		DrawString(title, BPoint((bounds.Width() - titleWidth) / 2, y));
		y += 50;

		// Section header font
		BFont sectionFont;
		sectionFont.SetSize(20);
		sectionFont.SetFace(B_BOLD_FACE);

		// Body font
		BFont bodyFont;
		bodyFont.SetSize(14);

		// Objective section
		SetFont(&sectionFont);
		SetHighColor(kDarkTextColor);
		DrawString("Objective", BPoint(leftMargin, y));
		y += 25;

		SetFont(&bodyFont);
		y = DrawWrappedText(
			"Scoundrel Solitaire is a rogue-like card game. The deck is your "
			"dungeon, and rooms are made up of four cards at a time. The goal "
			"is to make it through as many dungeons as possible without your "
			"life reaching 0.",
			leftMargin, y, textWidth);
		y += 30;

		// Gameplay section
		SetFont(&sectionFont);
		DrawString("Gameplay", BPoint(leftMargin, y));
		y += 25;

		SetFont(&bodyFont);
		y = DrawWrappedText(
			"Each room begins with four cards pulled from the top of the deck.",
			leftMargin, y, textWidth);
		y += 5;
		y = DrawWrappedText(
			"You may flee the room if you wish. If you do so, the cards in the "
			"room will be placed on the bottom of the deck. You cannot flee two "
			"rooms in a row.",
			leftMargin, y, textWidth);
		y += 5;
		y = DrawWrappedText(
			"If you choose to face a room, you must interact with three of the "
			"four cards in the room to advance. The fourth card will be carried "
			"over to the next room.",
			leftMargin, y, textWidth);
		y += 5;
		y = DrawWrappedText(
			"If you make it through a dungeon, all the cards are shuffled back "
			"into the deck and you continue into the next dungeon with your "
			"current life and weapon.",
			leftMargin, y, textWidth);
		y += 5;
		y = DrawWrappedText(
			"The game ends when your life reaches 0.",
			leftMargin, y, textWidth);
		y += 30;

		// Types of Cards section
		SetFont(&sectionFont);
		DrawString("Types of Cards", BPoint(leftMargin, y));
		y += 30;

		// Health Potion
		DrawCardTypeIcon("heart1", leftMargin, y - 15);
		SetFont(&sectionFont);
		sectionFont.SetSize(18);
		SetFont(&sectionFont);
		DrawString("Health Potion", BPoint(leftMargin + 30, y));
		y += 20;

		SetFont(&bodyFont);
		DrawString("Strength: 2-10  |  Count: 9", BPoint(leftMargin, y));
		y += 20;
		y = DrawWrappedText(
			"Increase life equal to the Health Potion's strength, up to the max of 20.",
			leftMargin, y, textWidth);
		y += 5;
		y = DrawWrappedText(
			"Only works once per room! Drinking a second potion will expend it "
			"without increasing your life.",
			leftMargin, y, textWidth);
		y += 25;

		// Weapon
		sectionFont.SetSize(18);
		SetFont(&sectionFont);
		DrawCardTypeIcon("sword1", leftMargin, y - 15);
		DrawString("Weapon", BPoint(leftMargin + 30, y));
		y += 20;

		SetFont(&bodyFont);
		DrawString("Strength: 2-10  |  Count: 9", BPoint(leftMargin, y));
		y += 20;
		y = DrawWrappedText(
			"Equip an undamaged weapon and a shield with the strength of the "
			"Weapon card. Any currently equipped weapons and shields are expended.",
			leftMargin, y, textWidth);
		y += 25;

		// Monster
		sectionFont.SetSize(18);
		SetFont(&sectionFont);
		DrawCardTypeIcon("dragon1", leftMargin, y - 15);
		DrawString("Monster", BPoint(leftMargin + 30, y));
		y += 20;

		SetFont(&bodyFont);
		DrawString("Strength: 2-14  |  Count: 26", BPoint(leftMargin, y));
		y += 20;
		y = DrawWrappedText(
			"Attack unarmed, losing life equal to the strength of the Monster card.",
			leftMargin, y, textWidth);
		y += 5;
		DrawString("OR", BPoint(leftMargin + 20, y));
		y += 18;
		y = DrawWrappedText(
			"Attack with equipped weapon, losing life equal to the strength of "
			"the Monster card minus the strength of your shield.",
			leftMargin, y, textWidth);
		y += 5;
		y = DrawWrappedText(
			"The equipped weapon will take damage and will only be able to be "
			"used against monsters weaker than the strength of the attacked "
			"Monster card.",
			leftMargin, y, textWidth);
		y += 30;

		// Scoring section
		SetFont(&sectionFont);
		sectionFont.SetSize(20);
		SetFont(&sectionFont);
		DrawString("Scoring", BPoint(leftMargin, y));
		y += 25;

		SetFont(&bodyFont);
		y = DrawWrappedText(
			"At the end of a dungeon, your score is the sum of the strength of "
			"all the Monster cards you defeated plus your remaining life.",
			leftMargin, y, textWidth);
		y += 5;
		y = DrawWrappedText(
			"If your life is 20 and your last card was a Health Potion, a bonus "
			"is added to your score equal to the strength of that card.",
			leftMargin, y, textWidth);
		y += 30;

		// UI section
		SetFont(&sectionFont);
		DrawString("UI Elements", BPoint(leftMargin, y));
		y += 25;

		SetFont(&bodyFont);
		DrawUIElement("||", "Pause the game", leftMargin, y);
		y += 50;
		DrawUIElementWithIcon("deck", "Cards remaining in the deck", leftMargin, y);
		y += 50;
		DrawUIElementWithIcon("dungeonGlyph", "Dungeons beaten", leftMargin, y);
		y += 50;
		DrawUIElement("Run", "Flee the current room", leftMargin, y);
		y += 50;
		DrawUIElementWithIcon("heart1", "Your life", leftMargin, y);
		y += 50;
		DrawUIElementWithIcon("shield1", "Shield strength (reduces monster damage)", leftMargin, y);
		y += 50;
		DrawUIElementWithIcon("sword1", "Weapon strength (max monster you can attack)", leftMargin, y);
		y += 300; // Large padding at bottom - whitespace behind close button when fully scrolled
	}

private:
	float DrawWrappedText(const char* text, float x, float y, float maxWidth) {
		BString str(text);
		BFont font;
		GetFont(&font);

		float lineHeight = 18;
		float currentY = y;
		int32 start = 0;
		int32 length = str.Length();

		while (start < length) {
			// Find how many characters fit on this line
			int32 end = start;
			float width = 0;

			while (end < length && str[end] != '\n') {
				char c = str[end];
				float charWidth = font.StringWidth(&c, 1);
				if (width + charWidth > maxWidth && end > start) {
					// Back up to last space
					int32 lastSpace = end;
					while (lastSpace > start && str[lastSpace] != ' ')
						lastSpace--;
					if (lastSpace > start)
						end = lastSpace;
					break;
				}
				width += charWidth;
				end++;
			}

			// Draw this line
			BString line;
			str.CopyInto(line, start, end - start);
			DrawString(line.String(), BPoint(x, currentY));
			currentY += lineHeight;

			// Skip whitespace at start of next line
			start = end;
			while (start < length && (str[start] == ' ' || str[start] == '\n'))
				start++;
		}

		return currentY;
	}

	void DrawCardTypeIcon(const char* iconName, float x, float y) {
		BBitmap* icon = ResourceLoader::Instance()->GetGlyph(iconName);
		if (icon != NULL) {
			BRect iconRect = icon->Bounds();
			float iconSize = 20;
			BRect destRect(x, y, x + iconSize, y + iconSize);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(icon, iconRect, destRect);
			SetDrawingMode(B_OP_COPY);
		}
	}

	void DrawUIElement(const char* symbol, const char* description, float x, float y) {
		// Draw stone button background
		BRect boxRect(x, y - 15, x + 40, y + 25);
		SetHighColor(80, 80, 100);
		FillRoundRect(boxRect, 5, 5);
		SetHighColor(100, 100, 120);
		StrokeRoundRect(boxRect, 5, 5);

		// Draw symbol
		BFont font;
		font.SetSize(16);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);
		SetHighColor(kTextColor);
		float symbolWidth = StringWidth(symbol);
		DrawString(symbol, BPoint(x + (40 - symbolWidth) / 2, y + 8));

		// Draw description
		font.SetSize(14);
		font.SetFace(0);
		SetFont(&font);
		SetHighColor(kDarkTextColor);
		DrawString(description, BPoint(x + 50, y + 8));
	}

	void DrawUIElementWithIcon(const char* iconName, const char* description, float x, float y) {
		// Draw glass-like box background
		BRect boxRect(x, y - 15, x + 40, y + 25);
		SetHighColor(200, 200, 220, 180);
		FillRoundRect(boxRect, 5, 5);
		SetHighColor(150, 150, 170);
		StrokeRoundRect(boxRect, 5, 5);

		// Draw icon
		BBitmap* icon = ResourceLoader::Instance()->GetGlyph(iconName);
		if (icon != NULL) {
			BRect iconRect = icon->Bounds();
			float iconSize = 25;
			BRect destRect(x + 7, y - 10, x + 7 + iconSize, y - 10 + iconSize);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(icon, iconRect, destRect);
			SetDrawingMode(B_OP_COPY);
		}

		// Draw description
		BFont font;
		font.SetSize(14);
		SetFont(&font);
		SetHighColor(kDarkTextColor);
		DrawString(description, BPoint(x + 50, y + 8));
	}
};


class PlankButtonHowTo : public BView {
public:
	PlankButtonHowTo(BRect frame, const char* label, BMessage* message)
		: BView(frame, "plankBtn", B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual ~PlankButtonHowTo() {
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




HowToPlayWindow::HowToPlayWindow(BWindow* parent)
	:
	BWindow(BRect(0, 0, 400, 550), "How to Play",
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

	BRect bounds = Bounds();

	float buttonWidth = 120;
	float buttonHeight = 40;
	float buttonAreaHeight = 55;

	// Scroll view leaves room for button at bottom
	float scrollHeight = bounds.Height() - buttonAreaHeight;
	float contentWidth = bounds.Width() - B_V_SCROLL_BAR_WIDTH - 1;
	float contentHeight = 1700; // Full scrollable height with padding at bottom

	// Create content view at FULL scrollable size
	BRect contentRect(0, 0, contentWidth, contentHeight);
	HowToPlayContentView* contentView = new HowToPlayContentView(contentRect);

	// Create scroll view - it adopts the content's full size frame
	BScrollView* scrollView = new BScrollView("scrollView", contentView,
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, 0, false, true, B_NO_BORDER);

	// Resize scroll view to the visible area size
	scrollView->ResizeTo(bounds.Width(), scrollHeight);

	AddChild(scrollView);

	// Create a background view for button area that matches parchment
	BView* buttonBgView = new BView(
		BRect(0, scrollHeight, bounds.Width(), bounds.Height()),
		"buttonBg", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM, 0);
	buttonBgView->SetViewColor(222, 210, 190);
	AddChild(buttonBgView);

	// Add close button to the button background view
	float buttonX = (bounds.Width() - buttonWidth) / 2;
	float buttonY = (buttonAreaHeight - buttonHeight) / 2;

	PlankButtonHowTo* closeBtn = new PlankButtonHowTo(
		BRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight),
		"Close", new BMessage(B_QUIT_REQUESTED));
	buttonBgView->AddChild(closeBtn);
}


HowToPlayWindow::~HowToPlayWindow()
{
}


void
HowToPlayWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_QUIT_REQUESTED:
			PostMessage(B_QUIT_REQUESTED);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
