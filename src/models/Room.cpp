#include "Room.h"
#include "Deck.h"
#include "utils/Constants.h"

Room::Room()
	:
	fCanFlee(true),
	fUsedHealthPotion(false),
	fPlayerFled(false)
{
	for (int i = 0; i < kRoomSize; i++) {
		fCards[i] = NULL;
		fDestinations[i] = kDestinationDeck;
	}
}


Room::Room(const BMessage* archive)
	:
	fCanFlee(true),
	fUsedHealthPotion(false),
	fPlayerFled(false)
{
	for (int i = 0; i < kRoomSize; i++) {
		fCards[i] = NULL;
		fDestinations[i] = kDestinationDeck;
	}

	// Load cards
	for (int i = 0; i < kRoomSize; i++) {
		BMessage cardArchive;
		BString fieldName;
		fieldName.SetToFormat("card%d", i);
		if (archive->FindMessage(fieldName.String(), &cardArchive) == B_OK) {
			fCards[i] = Card::Instantiate(&cardArchive);
		}
	}

	bool value;
	if (archive->FindBool("canFlee", &value) == B_OK)
		fCanFlee = value;
	if (archive->FindBool("usedHealthPotion", &value) == B_OK)
		fUsedHealthPotion = value;
	if (archive->FindBool("playerFled", &value) == B_OK)
		fPlayerFled = value;

	SetDestinations();
}


Room::~Room()
{
	for (int i = 0; i < kRoomSize; i++) {
		delete fCards[i];
		fCards[i] = NULL;
	}
}


void
Room::Reset(Deck* deck)
{
	for (int i = 0; i < kRoomSize; i++) {
		delete fCards[i];
		fCards[i] = NULL;
		fDestinations[i] = kDestinationDeck;
	}

	fCanFlee = true;
	fUsedHealthPotion = false;
	fPlayerFled = false;

	DealCards(deck);
}


void
Room::NextRoom(Deck* deck, bool fledLastRoom)
{
	// Can't flee if just fled
	fCanFlee = !fledLastRoom;
	fUsedHealthPotion = false;
	fPlayerFled = false;  // Reset for new room

	DealCards(deck);
}


void
Room::DealCards(Deck* deck)
{
	if (deck == NULL)
		return;

	// Fill empty slots
	for (int i = 0; i < kRoomSize; i++) {
		if (fCards[i] == NULL) {
			fCards[i] = deck->DrawCard();
		}
	}

	SetDestinations();
}


void
Room::SetDestinations()
{
	for (int i = 0; i < kRoomSize; i++) {
		if (fCards[i] == NULL) {
			fDestinations[i] = kDestinationDeck;
			continue;
		}

		switch (fCards[i]->Suit()) {
			case kSuitHealthPotion:
				fDestinations[i] = kDestinationHealth;
				break;
			case kSuitWeapon:
			case kSuitMonster:
				fDestinations[i] = kDestinationWeapon;
				break;
		}
	}
}


void
Room::Flee(Deck* deck)
{
	if (deck == NULL)
		return;

	// Collect all cards and return to deck
	BObjectList<Card> cardsToReturn(kRoomSize, false);
	for (int i = 0; i < kRoomSize; i++) {
		if (fCards[i] != NULL) {
			cardsToReturn.AddItem(fCards[i]);
			fCards[i] = NULL;
		}
	}

	deck->AppendCards(&cardsToReturn);

	fPlayerFled = true;
}


Card*
Room::GetCard(int index) const
{
	if (index < 0 || index >= kRoomSize)
		return NULL;
	return fCards[index];
}


void
Room::RemoveCard(int index)
{
	if (index < 0 || index >= kRoomSize)
		return;

	delete fCards[index];
	fCards[index] = NULL;
}


int
Room::CardCount() const
{
	int count = 0;
	for (int i = 0; i < kRoomSize; i++) {
		if (fCards[i] != NULL)
			count++;
	}
	return count;
}


bool
Room::IsEmpty() const
{
	return CardCount() == 0;
}


CardDestination
Room::GetDestination(int index) const
{
	if (index < 0 || index >= kRoomSize)
		return kDestinationDeck;
	return fDestinations[index];
}


status_t
Room::Archive(BMessage* archive) const
{
	status_t status = B_OK;

	for (int i = 0; i < kRoomSize && status == B_OK; i++) {
		if (fCards[i] != NULL) {
			BMessage cardArchive;
			status = fCards[i]->Archive(&cardArchive);
			if (status == B_OK) {
				BString fieldName;
				fieldName.SetToFormat("card%d", i);
				status = archive->AddMessage(fieldName.String(), &cardArchive);
			}
		}
	}

	if (status == B_OK)
		status = archive->AddBool("canFlee", fCanFlee);
	if (status == B_OK)
		status = archive->AddBool("usedHealthPotion", fUsedHealthPotion);
	if (status == B_OK)
		status = archive->AddBool("playerFled", fPlayerFled);

	return status;
}
