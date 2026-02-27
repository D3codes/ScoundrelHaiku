#include "Card.h"
#include "utils/Constants.h"

Card::Card(CardSuit suit, int strength)
	:
	fSuit(suit),
	fStrength(strength)
{
	// Validate strength
	if (fStrength < kMinCardStrength)
		fStrength = kMinCardStrength;

	// Weapons and potions cap at 10, monsters can go to 14
	if (fSuit != kSuitMonster && fStrength > kMaxWeaponStrength)
		fStrength = kMaxWeaponStrength;
	else if (fStrength > kMaxMonsterStrength)
		fStrength = kMaxMonsterStrength;
}


Card::Card(const BMessage* archive)
	:
	fSuit(kSuitMonster),
	fStrength(2)
{
	int32 suit;
	if (archive->FindInt32("suit", &suit) == B_OK)
		fSuit = static_cast<CardSuit>(suit);

	int32 strength;
	if (archive->FindInt32("strength", &strength) == B_OK)
		fStrength = strength;
}


Card::Card(const Card& other)
	:
	fSuit(other.fSuit),
	fStrength(other.fStrength)
{
}


Card::~Card()
{
}


Card&
Card::operator=(const Card& other)
{
	if (this != &other) {
		fSuit = other.fSuit;
		fStrength = other.fStrength;
	}
	return *this;
}


BString
Card::GetImageName() const
{
	BString name;

	switch (fSuit) {
		case kSuitWeapon:
			name.SetToFormat("weapon%d", fStrength);
			break;
		case kSuitHealthPotion:
			// Health potions are grouped by strength ranges
			if (fStrength <= 3)
				name = "healthPotion2";
			else if (fStrength <= 6)
				name = "healthPotion5";
			else
				name = "healthPotion8";
			break;
		case kSuitMonster:
			name.SetToFormat("monster%d", fStrength);
			break;
	}

	return name;
}


BString
Card::GetIconName() const
{
	switch (fSuit) {
		case kSuitWeapon:
			return "sword1";
		case kSuitHealthPotion:
			return "heart1";
		case kSuitMonster:
			return "dragon1";
	}
	return "";
}


BString
Card::GetFirstButtonText() const
{
	switch (fSuit) {
		case kSuitWeapon:
			return "Equip";
		case kSuitHealthPotion:
			return "Drink";
		case kSuitMonster:
			return "Unarmed";
	}
	return "";
}


BString
Card::GetSecondButtonText() const
{
	if (fSuit == kSuitMonster)
		return "Weapon";
	return "";
}


rgb_color
Card::GetColor() const
{
	switch (fSuit) {
		case kSuitWeapon:
			return kWeaponColor;
		case kSuitHealthPotion:
			return kPotionColor;
		case kSuitMonster:
			return kMonsterColor;
	}
	return kDarkTextColor;
}


status_t
Card::Archive(BMessage* archive) const
{
	status_t status = archive->AddInt32("suit", static_cast<int32>(fSuit));
	if (status == B_OK)
		status = archive->AddInt32("strength", fStrength);
	return status;
}


Card*
Card::Instantiate(const BMessage* archive)
{
	return new Card(archive);
}
