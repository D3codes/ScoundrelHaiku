#include "SettingsWindow.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <CheckBox.h>
#include <String.h>
#include <View.h>


class SettingsContentView : public BView {
public:
	SettingsContentView(BRect frame)
		: BView(frame, "settingsContent", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(222, 210, 190);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		float y = 30;
		float leftMargin = 20;

		// Title font
		BFont titleFont;
		titleFont.SetSize(24);
		titleFont.SetFace(B_BOLD_FACE);

		// Section font
		BFont sectionFont;
		sectionFont.SetSize(18);
		sectionFont.SetFace(B_BOLD_FACE);

		// Body font
		BFont bodyFont;
		bodyFont.SetSize(14);

		// Draw title
		SetFont(&titleFont);
		SetHighColor(kDarkTextColor);
		DrawString("Settings", BPoint(leftMargin, y));
		y += 50;

		// Audio section
		SetFont(&sectionFont);
		DrawString("Audio", BPoint(leftMargin, y));
		y += 30;

		// Placeholder text
		SetFont(&bodyFont);
		SetHighColor(100, 100, 100);
		DrawString("Sound settings coming soon...", BPoint(leftMargin, y));
		y += 50;

		// Display section
		SetFont(&sectionFont);
		SetHighColor(kDarkTextColor);
		DrawString("Display", BPoint(leftMargin, y));
		y += 30;

		SetFont(&bodyFont);
		SetHighColor(100, 100, 100);
		DrawString("Display settings coming soon...", BPoint(leftMargin, y));
		y += 50;

		// Game section
		SetFont(&sectionFont);
		SetHighColor(kDarkTextColor);
		DrawString("Game", BPoint(leftMargin, y));
		y += 30;

		SetFont(&bodyFont);
		SetHighColor(100, 100, 100);
		DrawString("Game settings coming soon...", BPoint(leftMargin, y));
	}
};


SettingsWindow::SettingsWindow(BWindow* parent)
	:
	BWindow(BRect(0, 0, 350, 300), "Settings",
		B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fParent(parent)
{
	// Center on parent
	BRect parentFrame = parent->Frame();
	BRect frame = Frame();
	float x = parentFrame.left + (parentFrame.Width() - frame.Width()) / 2;
	float y = parentFrame.top + (parentFrame.Height() - frame.Height()) / 2;
	MoveTo(x, y);

	// Create content view
	BRect bounds = Bounds();
	SettingsContentView* contentView = new SettingsContentView(bounds);
	AddChild(contentView);
}


SettingsWindow::~SettingsWindow()
{
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
