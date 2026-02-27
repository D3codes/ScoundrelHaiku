#include "Player.h"
#include "utils/Constants.h"

Player::Player()
	:
	fHealth(kStartingHealth),
	fWeapon(0),
	fLastAttacked(0)
{
}


Player::Player(const BMessage* archive)
	:
	fHealth(kStartingHealth),
	fWeapon(0),
	fLastAttacked(0)
{
	int32 value;
	if (archive->FindInt32("health", &value) == B_OK)
		fHealth = value;
	if (archive->FindInt32("weapon", &value) == B_OK)
		fWeapon = value;
	if (archive->FindInt32("lastAttacked", &value) == B_OK)
		fLastAttacked = value;
}


Player::~Player()
{
}


void
Player::Reset()
{
	fHealth = kStartingHealth;
	fWeapon = 0;
	fLastAttacked = 0;
}


void
Player::EquipWeapon(int weaponStrength)
{
	fWeapon = weaponStrength;
	// Fresh weapon can attack any monster (lastAttacked = 15 means > 14)
	fLastAttacked = kMaxMonsterStrength + 1;
}


void
Player::UseHealthPotion(int potionStrength)
{
	fHealth += potionStrength;
	if (fHealth > kMaxHealth)
		fHealth = kMaxHealth;
}


bool
Player::CanAttackWithWeapon(int monsterStrength) const
{
	if (fWeapon <= 0)
		return false;

	// Can attack if lastAttacked is not set (fresh weapon) or
	// lastAttacked is greater than the monster's strength
	if (fLastAttacked == 0)
		return true;

	return fLastAttacked > monsterStrength;
}


void
Player::Attack(bool withWeapon, int monsterStrength)
{
	if (withWeapon && fWeapon > 0) {
		// Attack with weapon
		fLastAttacked = monsterStrength;

		// Calculate damage: only take damage if monster stronger than weapon
		if (monsterStrength > fWeapon) {
			fHealth -= (monsterStrength - fWeapon);
		}

		// Check if weapon breaks (attacked weakest possible monster)
		if (monsterStrength == kMinCardStrength) {
			fWeapon = 0;
			fLastAttacked = 0;
		}
	} else {
		// Unarmed attack - take full damage
		fHealth -= monsterStrength;
	}

	// Clamp health to 0
	if (fHealth < 0)
		fHealth = 0;
}


int
Player::StrongestMonsterCanAttack() const
{
	if (fWeapon <= 0 || fLastAttacked == 0)
		return 0;

	// Can attack monsters weaker than lastAttacked
	return fLastAttacked - 1;
}


status_t
Player::Archive(BMessage* archive) const
{
	status_t status = archive->AddInt32("health", fHealth);
	if (status == B_OK)
		status = archive->AddInt32("weapon", fWeapon);
	if (status == B_OK)
		status = archive->AddInt32("lastAttacked", fLastAttacked);
	return status;
}
