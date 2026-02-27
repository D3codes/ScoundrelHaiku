#ifndef PLAYER_H
#define PLAYER_H

#include <Message.h>

class Player {
public:
						Player();
						Player(const BMessage* archive);
						~Player();

	void				Reset();

	int					Health() const { return fHealth; }
	int					Weapon() const { return fWeapon; }
	bool				HasWeapon() const { return fWeapon > 0; }
	int					LastAttacked() const { return fLastAttacked; }

	void				EquipWeapon(int weaponStrength);
	void				UseHealthPotion(int potionStrength);
	bool				CanAttackWithWeapon(int monsterStrength) const;
	void				Attack(bool withWeapon, int monsterStrength);
	int					StrongestMonsterCanAttack() const;

	bool				IsAlive() const { return fHealth > 0; }

	status_t			Archive(BMessage* archive) const;

private:
	int					fHealth;
	int					fWeapon;		// 0 = no weapon
	int					fLastAttacked;	// 0 = not set (weapon fresh)
};

#endif // PLAYER_H
