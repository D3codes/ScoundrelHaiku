#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <Window.h>

class SettingsWindow : public BWindow {
public:
						SettingsWindow(BWindow* parent);
	virtual				~SettingsWindow();

	virtual void		MessageReceived(BMessage* message);

private:
	BWindow*			fParent;
};

#endif // SETTINGS_WINDOW_H
