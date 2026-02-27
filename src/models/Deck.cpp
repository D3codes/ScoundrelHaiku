#include "Deck.h"
#include "utils/Constants.h"

#include <stdlib.h>
#include <time.h>

Deck::Deck()
	:
	fCards(20, true) // owns items
{
	Reset();
}


Deck::Deck(const BMessage* archive)
	:
	fCards(20, true)
{
	BMessage cardArchive;
	int32 index = 0;
	while (archive->FindMessage("card", index, &cardArchive) == B_OK) {
		Card* card = Card::Instantiate(&cardArchive);
		if (card != NULL)
			fCards.AddItem(card);
		index++;
	}
}


Deck::~Deck()
{
	// BObjectList will delete all cards
}


void
Deck::Reset()
{
	fCards.MakeEmpty();

	// Create deck: for each strength 2-14
	for (int strength = kMinCardStrength; strength <= kMaxMonsterStrength; strength++) {
		// 2 monster cards per strength
		fCards.AddItem(new Card(kSuitMonster, strength));
		fCards.AddItem(new Card(kSuitMonster, strength));

		// Weapons and potions only go up to 10
		if (strength <= kMaxWeaponStrength) {
			fCards.AddItem(new Card(kSuitWeapon, strength));
			fCards.AddItem(new Card(kSuitHealthPotion, strength));
		}
	}

	Shuffle();
}


void
Deck::Shuffle()
{
	// Seed random number generator
	static bool seeded = false;
	if (!seeded) {
		srand(time(NULL));
		seeded = true;
	}

	// Fisher-Yates shuffle using manual swap
	int count = fCards.CountItems();
	for (int i = count - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		if (i != j) {
			// Manual swap: remove both items and re-add in swapped positions
			Card* cardI = fCards.RemoveItemAt(i);
			Card* cardJ = fCards.RemoveItemAt(j);
			fCards.AddItem(cardI, j);
			fCards.AddItem(cardJ, i);
		}
	}
}


Card*
Deck::DrawCard()
{
	if (fCards.IsEmpty())
		return NULL;

	return fCards.RemoveItemAt(0);
}


void
Deck::AppendCards(BObjectList<Card>* cards)
{
	if (cards == NULL)
		return;

	while (cards->CountItems() > 0) {
		Card* card = cards->RemoveItemAt(0);
		if (card != NULL)
			fCards.AddItem(card);
	}

	Shuffle();
}


status_t
Deck::Archive(BMessage* archive) const
{
	status_t status = B_OK;

	for (int i = 0; i < fCards.CountItems() && status == B_OK; i++) {
		Card* card = fCards.ItemAt(i);
		if (card != NULL) {
			BMessage cardArchive;
			status = card->Archive(&cardArchive);
			if (status == B_OK)
				status = archive->AddMessage("card", &cardArchive);
		}
	}

	return status;
}
