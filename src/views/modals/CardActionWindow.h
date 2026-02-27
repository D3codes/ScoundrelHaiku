#ifndef CARD_ACTION_WINDOW_H
#define CARD_ACTION_WINDOW_H

#include <Button.h>
#include <Window.h>

class Card;

class CardActionWindow : public BWindow {
public:
						CardActionWindow(BWindow* parent, Card* card,
							int32 cardIndex, bool canUseWeapon);
	virtual				~CardActionWindow();

	virtual void		MessageReceived(BMessage* message);

private:
	BWindow*			fParent;
	Card*				fCard;
	int32				fCardIndex;
	BButton*			fFirstButton;
	BButton*			fSecondButton;
	BButton*			fCancelButton;
};

#endif // CARD_ACTION_WINDOW_H
