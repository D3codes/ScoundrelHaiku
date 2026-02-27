#ifndef CARD_H
#define CARD_H

#include <Message.h>
#include <String.h>

enum CardSuit {
	kSuitWeapon,
	kSuitHealthPotion,
	kSuitMonster
};

class Card {
public:
						Card(CardSuit suit, int strength);
						Card(const BMessage* archive);
						Card(const Card& other);
						~Card();

	Card&				operator=(const Card& other);

	CardSuit			Suit() const { return fSuit; }
	int					Strength() const { return fStrength; }

	BString				GetImageName() const;
	BString				GetIconName() const;
	BString				GetFirstButtonText() const;
	BString				GetSecondButtonText() const;

	rgb_color			GetColor() const;

	status_t			Archive(BMessage* archive) const;
	static Card*		Instantiate(const BMessage* archive);

private:
	CardSuit			fSuit;
	int					fStrength;
};

#endif // CARD_H
