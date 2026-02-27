#ifndef CARD_ACTION_WINDOW_H
#define CARD_ACTION_WINDOW_H

#include <View.h>
#include <Window.h>

class Card;
class Player;

class CardActionWindow : public BWindow {
public:
						CardActionWindow(BWindow* parent, Card* card,
							int32 cardIndex, bool canUseWeapon,
							Player* player);
	virtual				~CardActionWindow();

	virtual void		MessageReceived(BMessage* message);

private:
	BWindow*			fParent;
	Card*				fCard;
	int32				fCardIndex;
	BView*				fFirstButton;
	BView*				fSecondButton;
	BView*				fCancelButton;
};

#endif // CARD_ACTION_WINDOW_H
