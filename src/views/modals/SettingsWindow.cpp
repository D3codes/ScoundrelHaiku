#include "SettingsWindow.h"
#include "helpers/HighScoreManager.h"
#include "helpers/MusicPlayer.h"
#include "helpers/SoundPlayer.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Alert.h>
#include <Box.h>
#include <Button.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <Slider.h>
#include <SpaceLayoutItem.h>
#include <StringView.h>
#include <View.h>

static const uint32 kMsgSfxVolumeChanged = 'SVOL';
static const uint32 kMsgMusicVolumeChanged = 'MVOL';
static const uint32 kMsgResetScores = 'RSTS';


SettingsWindow::SettingsWindow(BWindow* parent)
	:
	BWindow(BRect(0, 0, 350, 280), "Settings",
		B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fParent(parent)
{
	// Audio section
	fSfxVolumeSlider = new BSlider("sfxVolume", "Sound Effects",
		new BMessage(kMsgSfxVolumeChanged), 0, 100, B_HORIZONTAL);
	fSfxVolumeSlider->SetValue((int32)(SoundPlayer::Instance()->Volume() * 100));
	fSfxVolumeSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fSfxVolumeSlider->SetHashMarkCount(11);
	fSfxVolumeSlider->SetLimitLabels("0%", "100%");
	fSfxVolumeSlider->SetModificationMessage(new BMessage(kMsgSfxVolumeChanged));

	fMusicVolumeSlider = new BSlider("musicVolume", "Background Music",
		new BMessage(kMsgMusicVolumeChanged), 0, 100, B_HORIZONTAL);
	fMusicVolumeSlider->SetValue((int32)(MusicPlayer::Instance()->Volume() * 100));
	fMusicVolumeSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fMusicVolumeSlider->SetHashMarkCount(11);
	fMusicVolumeSlider->SetLimitLabels("0%", "100%");
	fMusicVolumeSlider->SetModificationMessage(new BMessage(kMsgMusicVolumeChanged));

	BBox* audioBox = new BBox("audioBox");
	audioBox->SetLabel("Audio");
	BLayoutBuilder::Group<>(audioBox, B_VERTICAL, B_USE_SMALL_SPACING)
		.SetInsets(B_USE_SMALL_INSETS)
		.AddStrut(B_USE_SMALL_SPACING)
		.Add(fSfxVolumeSlider)
		.Add(fMusicVolumeSlider)
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


bool
SettingsWindow::QuitRequested()
{
	fParent->PostMessage(kMsgSettingsClosed);
	return true;
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgSfxVolumeChanged:
		{
			float volume = (float)fSfxVolumeSlider->Value() / 100.0;
			SoundPlayer::Instance()->SetVolume(volume);
			break;
		}

		case kMsgMusicVolumeChanged:
		{
			float volume = (float)fMusicVolumeSlider->Value() / 100.0;
			MusicPlayer::Instance()->SetVolume(volume);
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
