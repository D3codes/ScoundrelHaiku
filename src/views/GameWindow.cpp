#include "GameWindow.h"
#include "MainMenuView.h"
#include "GameBoardView.h"
#include "modals/CardActionWindow.h"
#include "modals/PauseWindow.h"
#include "modals/DungeonBeatWindow.h"
#include "modals/HowToPlayWindow.h"
#include "modals/HighScoresWindow.h"
#include "modals/NameEntryWindow.h"
#include "helpers/HighScoreManager.h"
#include "helpers/SaveManager.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Application.h>

GameWindow::GameWindow()
	:
	BWindow(BRect(100, 100, 100 + kWindowWidth, 100 + kWindowHeight),
		"Scoundrel", B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_QUIT_ON_WINDOW_CLOSE),
	fMainMenuView(NULL),
	fGameBoardView(NULL),
	fShowingMenu(false),
	fPendingScore(0),
	fPendingDungeons(0),
	fHowToPlayWindow(NULL),
	fHighScoresWindow(NULL)
{
	fGame.SetObserver(this);

	// Create views
	BRect bounds = Bounds();
	fMainMenuView = new MainMenuView(bounds);
	fGameBoardView = new GameBoardView(bounds);

	// Add both views, hide game board initially
	AddChild(fMainMenuView);
	AddChild(fGameBoardView);
	fGameBoardView->Hide();

	// Check for saved game
	if (SaveManager::Instance()->HasSavedGame()) {
		SaveManager::Instance()->LoadGame(&fGame);
		fMainMenuView->SetHasSavedGame(true);
	}

	fShowingMenu = true;
}


GameWindow::~GameWindow()
{
}


bool
GameWindow::QuitRequested()
{
	// Save game if playing
	if (fGame.State() == kGameStatePlaying) {
		SaveManager::Instance()->SaveGame(&fGame);
	}

	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
GameWindow::ShowMainMenu()
{
	if (fShowingMenu)
		return;

	fGameBoardView->Hide();
	fMainMenuView->Show();
	fMainMenuView->SetHasSavedGame(SaveManager::Instance()->HasSavedGame());
	fShowingMenu = true;
}


void
GameWindow::ShowGameBoard()
{
	if (!fShowingMenu)
		return;

	fMainMenuView->Hide();
	fGameBoardView->Show();
	fGameBoardView->SetGame(&fGame);
	fGameBoardView->Invalidate();
	fShowingMenu = false;
}


void
GameWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgNewGame:
			SaveManager::Instance()->DeleteSavedGame();
			fGameBoardView->RandomizeBackground();
			fGameBoardView->ClearRoom();  // Clear any existing cards
			ShowGameBoard();  // Show empty board first
			fGame.NewGame();  // Then start game (triggers deal animation)
			break;

		case kMsgResumeGame:
			if (SaveManager::Instance()->HasSavedGame()) {
				SaveManager::Instance()->LoadGame(&fGame);
				fGame.Resume();
				ShowGameBoard();
			}
			break;

		case kMsgHowToPlay:
			ShowHowToPlay();
			break;

		case kMsgMainMenu:
			// Don't save if game is over - delete save instead
			if (fGame.State() == kGameStateGameOver) {
				SaveManager::Instance()->DeleteSavedGame();
			} else {
				SaveManager::Instance()->SaveGame(&fGame);
			}
			ShowMainMenu();
			break;

		case kMsgPause:
			fGame.Pause();
			ShowPauseModal();
			break;

		case kMsgContinue:
			fGame.Resume();
			break;

		case kMsgFlee:
			fGame.Flee();
			break;

		case kMsgNextDungeon:
			fGameBoardView->RandomizeBackground();
			fGame.NextDungeon();
			break;

		case kMsgCardSelected:
		{
			int32 index;
			if (message->FindInt32("index", &index) == B_OK) {
				ShowCardActionModal(index);
			}
			break;
		}

		case kMsgCardAction:
			HandleCardAction(message);
			break;

		case kMsgHighScores:
			ShowHighScores();
			break;

		default:
			BWindow::MessageReceived(message);
			break;
	}
}


void
GameWindow::ShowCardActionModal(int32 cardIndex)
{
	Card* card = fGame.GetRoom()->GetCard(cardIndex);
	if (card == NULL)
		return;

	bool canUseWeapon = false;
	if (card->Suit() == kSuitMonster) {
		canUseWeapon = fGame.GetPlayer()->CanAttackWithWeapon(card->Strength());
	}

	CardActionWindow* modal = new CardActionWindow(this, card, cardIndex,
		canUseWeapon, fGame.GetPlayer());
	modal->Show();
}


void
GameWindow::ShowPauseModal()
{
	PauseWindow* modal = new PauseWindow(this);
	modal->Show();
}


void
GameWindow::ShowDungeonBeatModal()
{
	DungeonBeatWindow* modal = new DungeonBeatWindow(this, fGame.Score(),
		fGame.DungeonDepth());
	modal->Show();
}


void
GameWindow::HandleCardAction(BMessage* message)
{
	int32 index;
	int32 action;

	if (message->FindInt32("index", &index) != B_OK)
		return;
	if (message->FindInt32("action", &action) != B_OK)
		return;

	switch (action) {
		case kActionDrink:
			fGame.UseHealthPotion(index);
			break;
		case kActionEquip:
			fGame.EquipWeapon(index);
			break;
		case kActionAttackUnarmed:
			fGame.AttackMonster(index, true);
			break;
		case kActionAttackWeapon:
			fGame.AttackMonster(index, false);
			break;
	}

	// Explicitly refresh the game board after any card action
	if (fGameBoardView != NULL) {
		fGameBoardView->Refresh();
	}
}


// Helper to check if a window pointer is still valid
static bool
IsWindowValid(BWindow* window)
{
	if (window == NULL)
		return false;

	// Check if window is still in application's window list
	int32 count = be_app->CountWindows();
	for (int32 i = 0; i < count; i++) {
		if (be_app->WindowAt(i) == window)
			return true;
	}
	return false;
}


void
GameWindow::ShowHowToPlay()
{
	if (IsWindowValid(fHowToPlayWindow)) {
		// Window already exists, bring to front
		fHowToPlayWindow->Activate();
		return;
	}

	fHowToPlayWindow = new HowToPlayWindow(this);
	fHowToPlayWindow->Show();
}


void
GameWindow::ShowHighScores()
{
	if (IsWindowValid(fHighScoresWindow)) {
		// Window already exists, bring to front
		fHighScoresWindow->Activate();
		return;
	}

	fHighScoresWindow = new HighScoresWindow(this);
	fHighScoresWindow->Show();
}


void
GameWindow::ShowNameEntry()
{
	bool isHighScore = HighScoreManager::Instance()->IsHighScore(fPendingScore);
	NameEntryWindow* modal = new NameEntryWindow(this, fPendingScore,
		fPendingDungeons, isHighScore);
	modal->Show();

	// Automatically open high scores window when achieving a high score
	if (isHighScore)
		ShowHighScores();
}


// GameObserver implementation

void
GameWindow::OnGameStateChanged(GameState newState)
{
	switch (newState) {
		case kGameStateGameOver:
			// Store score for game over modal
			fPendingScore = fGame.Score();
			fPendingDungeons = fGame.DungeonDepth();
			SaveManager::Instance()->DeleteSavedGame();
			ShowNameEntry();
			break;
		case kGameStateDungeonBeat:
			ShowDungeonBeatModal();
			break;
		default:
			break;
	}
}


void
GameWindow::OnRoomUpdated()
{
	// Called when a card is removed (no animation needed)
	if (fGameBoardView != NULL)
		fGameBoardView->Refresh();
}


void
GameWindow::OnRoomDealt()
{
	// Called when new cards are dealt (trigger animation)
	if (fGameBoardView != NULL)
		fGameBoardView->RefreshWithAnimation();
}


void
GameWindow::OnFleeStarted()
{
	// Called before flee - capture current cards for flee animation
	if (fGameBoardView != NULL)
		fGameBoardView->PrepareFleeAnimation();
}


void
GameWindow::OnPlayerUpdated()
{
	if (fGameBoardView != NULL)
		fGameBoardView->Refresh();
}


void
GameWindow::OnScoreUpdated()
{
	if (fGameBoardView != NULL)
		fGameBoardView->Refresh();
}
