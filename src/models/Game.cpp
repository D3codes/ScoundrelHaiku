#include "Game.h"
#include "utils/Constants.h"

Game::Game()
	:
	fState(kGameStateCreated),
	fScore(0),
	fBonusPoints(0),
	fDungeonDepth(0),
	fObserver(NULL)
{
}


Game::Game(const BMessage* archive)
	:
	fState(kGameStateCreated),
	fScore(0),
	fBonusPoints(0),
	fDungeonDepth(0),
	fObserver(NULL)
{
	int32 value;
	if (archive->FindInt32("state", &value) == B_OK)
		fState = static_cast<GameState>(value);
	if (archive->FindInt32("score", &value) == B_OK)
		fScore = value;
	if (archive->FindInt32("bonusPoints", &value) == B_OK)
		fBonusPoints = value;
	if (archive->FindInt32("dungeonDepth", &value) == B_OK)
		fDungeonDepth = value;

	// Load deck
	BMessage deckArchive;
	if (archive->FindMessage("deck", &deckArchive) == B_OK)
		fDeck = Deck(&deckArchive);

	// Load player
	BMessage playerArchive;
	if (archive->FindMessage("player", &playerArchive) == B_OK)
		fPlayer = Player(&playerArchive);

	// Load room
	BMessage roomArchive;
	if (archive->FindMessage("room", &roomArchive) == B_OK)
		fRoom = Room(&roomArchive);
}


Game::~Game()
{
}


void
Game::SetObserver(GameObserver* observer)
{
	fObserver = observer;
}


void
Game::NewGame()
{
	fScore = 0;
	fBonusPoints = 0;
	fDungeonDepth = 0;

	fDeck.Reset();
	fPlayer.Reset();
	fRoom.Reset(&fDeck);

	fState = kGameStatePlaying;
	NotifyStateChanged();
	NotifyRoomUpdated();
	NotifyPlayerUpdated();
	NotifyScoreUpdated();
}


void
Game::NextDungeon()
{
	fDungeonDepth++;
	fBonusPoints = 0;

	fDeck.Reset();
	fRoom.Reset(&fDeck);

	fState = kGameStatePlaying;
	NotifyStateChanged();
	NotifyRoomUpdated();
	NotifyScoreUpdated();
}


void
Game::Pause()
{
	if (fState == kGameStatePlaying) {
		fState = kGameStatePaused;
		NotifyStateChanged();
	}
}


void
Game::Resume()
{
	if (fState == kGameStatePaused) {
		fState = kGameStatePlaying;
		NotifyStateChanged();
	}
}


void
Game::EquipWeapon(int cardIndex)
{
	Card* card = fRoom.GetCard(cardIndex);
	if (card == NULL || card->Suit() != kSuitWeapon)
		return;

	fPlayer.EquipWeapon(card->Strength());
	NotifyPlayerUpdated();

	EndAction(cardIndex);
}


void
Game::UseHealthPotion(int cardIndex)
{
	Card* card = fRoom.GetCard(cardIndex);
	if (card == NULL || card->Suit() != kSuitHealthPotion)
		return;

	// Check for perfect health bonus
	if (fPlayer.Health() == kMaxHealth) {
		// Already at full health - gain bonus points instead
		fBonusPoints = card->Strength();
	} else if (!fRoom.UsedHealthPotion()) {
		// Can only use one potion per room
		fPlayer.UseHealthPotion(card->Strength());
		fRoom.SetUsedHealthPotion(true);
	}
	// If potion already used this room, card is just discarded

	NotifyPlayerUpdated();
	EndAction(cardIndex);
}


void
Game::AttackMonster(int cardIndex, bool attackUnarmed)
{
	Card* card = fRoom.GetCard(cardIndex);
	if (card == NULL || card->Suit() != kSuitMonster)
		return;

	int monsterStrength = card->Strength();
	bool withWeapon = !attackUnarmed && fPlayer.CanAttackWithWeapon(monsterStrength);

	fPlayer.Attack(withWeapon, monsterStrength);
	NotifyPlayerUpdated();

	// Check if player died
	if (!fPlayer.IsAlive()) {
		EndGame();
		return;
	}

	// Add monster strength to score
	fScore += monsterStrength;
	NotifyScoreUpdated();

	EndAction(cardIndex);
}


void
Game::Flee()
{
	if (!fRoom.CanFlee())
		return;

	bool fledLastRoom = fRoom.PlayerFled();
	fRoom.Flee(&fDeck);

	// Deal new room
	fRoom.NextRoom(&fDeck, true); // Can't flee next room

	NotifyRoomUpdated();
}


void
Game::EndAction(int cardIndex)
{
	fRoom.RemoveCard(cardIndex);

	// Check if room is cleared and deck is empty
	if (fRoom.CardCount() == 1 && fDeck.IsEmpty()) {
		// Last card in room with empty deck
		// Don't deal new cards yet
	} else if (fRoom.CardCount() == 1) {
		// One card left - deal new room
		bool fledLastRoom = fRoom.PlayerFled();
		fRoom.NextRoom(&fDeck, fledLastRoom);
	}

	// Check if dungeon is complete (room empty and deck empty)
	if (fRoom.IsEmpty() && fDeck.IsEmpty()) {
		HandleDungeonCompletion();
		return;
	}

	// Reset bonus points if we didn't complete dungeon
	if (fRoom.CardCount() > 0)
		fBonusPoints = 0;

	NotifyRoomUpdated();
}


void
Game::HandleDungeonCompletion()
{
	// Add remaining health to score
	fScore += fPlayer.Health();

	// Add bonus points from perfect health potion usage
	fScore += fBonusPoints;

	NotifyScoreUpdated();

	fState = kGameStateDungeonBeat;
	NotifyStateChanged();
}


void
Game::EndGame()
{
	fState = kGameStateGameOver;
	NotifyStateChanged();
}


void
Game::NotifyStateChanged()
{
	if (fObserver != NULL)
		fObserver->OnGameStateChanged(fState);
}


void
Game::NotifyRoomUpdated()
{
	if (fObserver != NULL)
		fObserver->OnRoomUpdated();
}


void
Game::NotifyPlayerUpdated()
{
	if (fObserver != NULL)
		fObserver->OnPlayerUpdated();
}


void
Game::NotifyScoreUpdated()
{
	if (fObserver != NULL)
		fObserver->OnScoreUpdated();
}


status_t
Game::Archive(BMessage* archive) const
{
	status_t status = archive->AddInt32("state", static_cast<int32>(fState));
	if (status == B_OK)
		status = archive->AddInt32("score", fScore);
	if (status == B_OK)
		status = archive->AddInt32("bonusPoints", fBonusPoints);
	if (status == B_OK)
		status = archive->AddInt32("dungeonDepth", fDungeonDepth);

	// Archive deck
	if (status == B_OK) {
		BMessage deckArchive;
		status = fDeck.Archive(&deckArchive);
		if (status == B_OK)
			status = archive->AddMessage("deck", &deckArchive);
	}

	// Archive player
	if (status == B_OK) {
		BMessage playerArchive;
		status = fPlayer.Archive(&playerArchive);
		if (status == B_OK)
			status = archive->AddMessage("player", &playerArchive);
	}

	// Archive room
	if (status == B_OK) {
		BMessage roomArchive;
		status = fRoom.Archive(&roomArchive);
		if (status == B_OK)
			status = archive->AddMessage("room", &roomArchive);
	}

	return status;
}


bool
Game::HasSavedGame() const
{
	return fState == kGameStatePlaying || fState == kGameStatePaused;
}
