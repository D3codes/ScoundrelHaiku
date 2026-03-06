#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <Window.h>

class BButton;
class BSlider;

class SettingsWindow : public BWindow {
public:
						SettingsWindow(BWindow* parent);
	virtual				~SettingsWindow();

	virtual void		MessageReceived(BMessage* message);
	virtual bool		QuitRequested();

private:
	BWindow*			fParent;
	BSlider*			fSfxVolumeSlider;
	BSlider*			fMusicVolumeSlider;
	BButton*			fResetButton;
};

#endif // SETTINGS_WINDOW_H
