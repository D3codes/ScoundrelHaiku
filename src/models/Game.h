#ifndef GAME_H
#define GAME_H

#include <Handler.h>
#include <Message.h>
#include <ObjectList.h>

#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Room.h"
#include "GameState.h"

class GameObserver {
public:
	virtual				~GameObserver() {}
	virtual void		OnGameStateChanged(GameState newState) = 0;
	virtual void		OnRoomUpdated() = 0;
	virtual void		OnPlayerUpdated() = 0;
	virtual void		OnScoreUpdated() = 0;
};

class Game {
public:
						Game();
						Game(const BMessage* archive);
						~Game();

	void				SetObserver(GameObserver* observer);

	// Game state
	GameState			State() const { return fState; }
	int					Score() const { return fScore; }
	int					BonusPoints() const { return fBonusPoints; }
	int					DungeonDepth() const { return fDungeonDepth; }

	// Accessors
	Deck*				GetDeck() { return &fDeck; }
	Player*				GetPlayer() { return &fPlayer; }
	Room*				GetRoom() { return &fRoom; }

	// Game actions
	void				NewGame();
	void				NextDungeon();
	void				Pause();
	void				Resume();

	void				EquipWeapon(int cardIndex);
	void				UseHealthPotion(int cardIndex);
	void				AttackMonster(int cardIndex, bool attackUnarmed);
	void				Flee();

	// Persistence
	status_t			Archive(BMessage* archive) const;
	bool				HasSavedGame() const;

private:
	void				EndAction(int cardIndex);
	void				HandleDungeonCompletion();
	void				EndGame();
	void				NotifyStateChanged();
	void				NotifyRoomUpdated();
	void				NotifyPlayerUpdated();
	void				NotifyScoreUpdated();

	Deck				fDeck;
	Player				fPlayer;
	Room				fRoom;

	GameState			fState;
	int					fScore;
	int					fBonusPoints;
	int					fDungeonDepth;

	GameObserver*		fObserver;
};

#endif // GAME_H
