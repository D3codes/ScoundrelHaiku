#ifndef ROOM_H
#define ROOM_H

#include <Message.h>
#include <ObjectList.h>

#include "Card.h"

class Deck;

enum CardDestination {
	kDestinationHealth,
	kDestinationWeapon,
	kDestinationDeck
};

class Room {
public:
						Room();
						Room(const BMessage* archive);
						~Room();

	void				Reset(Deck* deck);
	void				NextRoom(Deck* deck, bool fledLastRoom);
	void				Flee(Deck* deck);

	Card*				GetCard(int index) const;
	void				RemoveCard(int index);

	int					CardCount() const;
	bool				IsEmpty() const;

	bool				CanFlee() const { return fCanFlee; }
	bool				UsedHealthPotion() const { return fUsedHealthPotion; }
	void				SetUsedHealthPotion(bool used) { fUsedHealthPotion = used; }
	bool				PlayerFled() const { return fPlayerFled; }

	CardDestination		GetDestination(int index) const;

	status_t			Archive(BMessage* archive) const;

private:
	void				DealCards(Deck* deck);
	void				SetDestinations();

	Card*				fCards[4];
	CardDestination		fDestinations[4];
	bool				fCanFlee;
	bool				fUsedHealthPotion;
	bool				fPlayerFled;
};

#endif // ROOM_H
