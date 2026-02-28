#include "Game.h"
#include "helpers/SoundPlayer.h"
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
	NotifyRoomDealt();  // New game deals cards - trigger animation
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
	NotifyRoomDealt();  // New dungeon deals cards - trigger animation
	NotifyScoreUpdated();
}


void
Game::Pause()
{
	if (fState == kGameStatePlaying) {
		SoundPlayer::Instance()->PlaySound(SFX_PAGE);
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

	fRoom.SetHasInteracted(true);
	SoundPlayer::Instance()->PlayRandomEquip();
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

	fRoom.SetHasInteracted(true);

	// Check for perfect health bonus
	if (fPlayer.Health() == kMaxHealth) {
		// Already at full health - gain bonus points instead
		fBonusPoints = card->Strength();
		SoundPlayer::Instance()->PlaySound(SFX_SPARKLE);
	} else if (!fRoom.UsedHealthPotion()) {
		// Can only use one potion per room
		fPlayer.UseHealthPotion(card->Strength());
		fRoom.SetUsedHealthPotion(true);
		SoundPlayer::Instance()->PlaySound(SFX_SPARKLE);
	} else {
		// Potion already used - play glass breaking sound
		SoundPlayer::Instance()->PlaySound(SFX_GLASS_BREAK);
	}

	NotifyPlayerUpdated();
	EndAction(cardIndex);
}


void
Game::AttackMonster(int cardIndex, bool attackUnarmed)
{
	Card* card = fRoom.GetCard(cardIndex);
	if (card == NULL || card->Suit() != kSuitMonster)
		return;

	fRoom.SetHasInteracted(true);
	int monsterStrength = card->Strength();
	bool withWeapon = !attackUnarmed && fPlayer.CanAttackWithWeapon(monsterStrength);

	// Play attack sound
	if (withWeapon) {
		SoundPlayer::Instance()->PlayRandomSword();
	} else {
		SoundPlayer::Instance()->PlayRandomPunch();
	}

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

	// Notify UI to start flee animation (before room changes)
	NotifyFleeStarted();

	SoundPlayer::Instance()->PlaySound(SFX_SHUFFLE);

	fRoom.Flee(&fDeck);

	// Deal new room
	fRoom.NextRoom(&fDeck, true); // Can't flee next room

	NotifyRoomDealt();  // Flee deals new room - trigger animation
}


void
Game::EndAction(int cardIndex)
{
	fRoom.RemoveCard(cardIndex);

	bool dealtNewRoom = false;

	// Check if room is cleared and deck is empty
	if (fRoom.CardCount() == 1 && fDeck.IsEmpty()) {
		// Last card in room with empty deck
		// Don't deal new cards yet
	} else if (fRoom.CardCount() == 1) {
		// One card left - deal new room
		bool fledLastRoom = fRoom.PlayerFled();
		fRoom.NextRoom(&fDeck, fledLastRoom);
		dealtNewRoom = true;
	}

	// Check if dungeon is complete (room empty and deck empty)
	if (fRoom.IsEmpty() && fDeck.IsEmpty()) {
		HandleDungeonCompletion();
		return;
	}

	// Reset bonus points if we didn't complete dungeon
	if (fRoom.CardCount() > 0)
		fBonusPoints = 0;

	// Only animate if new cards were dealt
	if (dealtNewRoom)
		NotifyRoomDealt();
	else
		NotifyRoomUpdated();
}


void
Game::HandleDungeonCompletion()
{
	// Add remaining health to score
	fScore += fPlayer.Health();

	// Add bonus points from perfect health potion usage
	fScore += fBonusPoints;

	SoundPlayer::Instance()->PlaySound(SFX_FANFARE);

	NotifyScoreUpdated();

	fState = kGameStateDungeonBeat;
	NotifyStateChanged();
}


void
Game::EndGame()
{
	SoundPlayer::Instance()->PlaySound(SFX_GAMEOVER);
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
Game::NotifyRoomDealt()
{
	if (fObserver != NULL)
		fObserver->OnRoomDealt();
}


void
Game::NotifyFleeStarted()
{
	if (fObserver != NULL)
		fObserver->OnFleeStarted();
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
