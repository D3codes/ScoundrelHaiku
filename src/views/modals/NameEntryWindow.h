#ifndef NAME_ENTRY_WINDOW_H
#define NAME_ENTRY_WINDOW_H

#include <Window.h>

class BTextControl;

class NameEntryWindow : public BWindow {
public:
						NameEntryWindow(BWindow* parent, int score,
							int dungeonsBeaten);
	virtual				~NameEntryWindow();

	virtual void		MessageReceived(BMessage* message);

private:
	BWindow*			fParent;
	BTextControl*		fNameInput;
	int					fScore;
	int					fDungeonsBeaten;
};

#endif // NAME_ENTRY_WINDOW_H
