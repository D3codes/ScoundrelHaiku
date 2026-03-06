#include "SettingsWindow.h"
#include "helpers/HighScoreManager.h"
#include "helpers/SoundPlayer.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Alert.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <SpaceLayoutItem.h>
#include <StringView.h>
#include <View.h>

static const uint32 kMsgMuteChanged = 'MUTC';
static const uint32 kMsgResetScores = 'RSTS';


SettingsWindow::SettingsWindow(BWindow* parent)
	:
	BWindow(BRect(0, 0, 300, 200), "Settings",
		B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fParent(parent)
{
	// Audio section
	fMuteCheckBox = new BCheckBox("muteCheck", "Mute sound effects",
		new BMessage(kMsgMuteChanged));
	fMuteCheckBox->SetValue(SoundPlayer::Instance()->IsMuted() ? B_CONTROL_ON : B_CONTROL_OFF);

	BBox* audioBox = new BBox("audioBox");
	audioBox->SetLabel("Audio");
	BLayoutBuilder::Group<>(audioBox, B_VERTICAL, B_USE_SMALL_SPACING)
		.SetInsets(B_USE_SMALL_INSETS)
		.AddStrut(B_USE_SMALL_SPACING)
		.Add(fMuteCheckBox)
		.AddGlue()
		.End();

	// High Scores section
	fResetButton = new BButton("resetBtn", "Reset High Scores",
		new BMessage(kMsgResetScores));

	BBox* scoresBox = new BBox("scoresBox");
	scoresBox->SetLabel("High Scores");
	BLayoutBuilder::Group<>(scoresBox, B_VERTICAL, B_USE_SMALL_SPACING)
		.SetInsets(B_USE_SMALL_INSETS)
		.AddStrut(B_USE_SMALL_SPACING)
		.Add(fResetButton)
		.AddGlue()
		.End();

	// Main layout
	BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_DEFAULT_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.Add(audioBox)
		.Add(scoresBox)
		.AddGlue()
		.End();

	// Center on parent
	BRect parentFrame = parent->Frame();
	BRect frame = Frame();
	float x = parentFrame.left + (parentFrame.Width() - frame.Width()) / 2;
	float y = parentFrame.top + (parentFrame.Height() - frame.Height()) / 2;
	MoveTo(x, y);
}


SettingsWindow::~SettingsWindow()
{
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgMuteChanged:
		{
			bool muted = (fMuteCheckBox->Value() == B_CONTROL_ON);
			SoundPlayer::Instance()->SetMuted(muted);
			break;
		}

		case kMsgResetScores:
		{
			BAlert* alert = new BAlert("Confirm Reset",
				"Are you sure you want to reset all high scores? This cannot be undone.",
				"Cancel", "Reset", NULL,
				B_WIDTH_AS_USUAL, B_WARNING_ALERT);
			alert->SetShortcut(0, B_ESCAPE);

			int32 result = alert->Go();
			if (result == 1) {
				HighScoreManager::Instance()->Reset();
			}
			break;
		}

		default:
			BWindow::MessageReceived(message);
			break;
	}
}
