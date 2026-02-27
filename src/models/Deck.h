#ifndef DECK_H
#define DECK_H

#include <Message.h>
#include <ObjectList.h>

#include "Card.h"

class Deck {
public:
						Deck();
						Deck(const BMessage* archive);
						~Deck();

	void				Reset();
	void				Shuffle();

	Card*				DrawCard();
	void				AppendCards(BObjectList<Card>* cards);

	int					CardsRemaining() const { return fCards.CountItems(); }
	bool				IsEmpty() const { return fCards.IsEmpty(); }

	status_t			Archive(BMessage* archive) const;

private:
	BObjectList<Card>	fCards;
};

#endif // DECK_H
