#include "GameWindow.h"
#include "MainMenuView.h"
#include "GameBoardView.h"
#include "modals/CardActionWindow.h"
#include "modals/PauseWindow.h"
#include "modals/GameOverWindow.h"
#include "modals/DungeonBeatWindow.h"
#include "helpers/SaveManager.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Alert.h>
#include <Application.h>

GameWindow::GameWindow()
	:
	BWindow(BRect(100, 100, 100 + kWindowWidth, 100 + kWindowHeight),
		"Scoundrel", B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_QUIT_ON_WINDOW_CLOSE),
	fMainMenuView(NULL),
	fGameBoardView(NULL),
	fShowingMenu(false)
{
	fGame.SetObserver(this);

	// Create views
	BRect bounds = Bounds();
	fMainMenuView = new MainMenuView(bounds);
	fGameBoardView = new GameBoardView(bounds);

	// Check for saved game
	if (SaveManager::Instance()->HasSavedGame()) {
		SaveManager::Instance()->LoadGame(&fGame);
		fMainMenuView->SetHasSavedGame(true);
	}

	// Show main menu initially
	ShowMainMenu();
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

	if (fGameBoardView != NULL && fGameBoardView->Parent() != NULL) {
		fGameBoardView->RemoveSelf();
	}

	if (fMainMenuView->Parent() == NULL) {
		AddChild(fMainMenuView);
	}
	fMainMenuView->SetHasSavedGame(SaveManager::Instance()->HasSavedGame());
	fMainMenuView->Invalidate();
	fShowingMenu = true;
}


void
GameWindow::ShowGameBoard()
{
	if (!fShowingMenu)
		return;

	if (fMainMenuView != NULL && fMainMenuView->Parent() != NULL) {
		fMainMenuView->RemoveSelf();
	}

	if (fGameBoardView->Parent() == NULL) {
		AddChild(fGameBoardView);
	}
	fGameBoardView->SetGame(&fGame);
	fGameBoardView->Invalidate();
	fShowingMenu = false;
}


void
GameWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgNewGame:
			fGame.NewGame();
			SaveManager::Instance()->DeleteSavedGame();
			ShowGameBoard();
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
			SaveManager::Instance()->SaveGame(&fGame);
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
		canUseWeapon);
	modal->Show();
}


void
GameWindow::ShowPauseModal()
{
	PauseWindow* modal = new PauseWindow(this);
	modal->Show();
}


void
GameWindow::ShowGameOverModal()
{
	GameOverWindow* modal = new GameOverWindow(this, fGame.Score(),
		fGame.DungeonDepth());
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
}


void
GameWindow::ShowHowToPlay()
{
	BString helpText;
	helpText = "SCOUNDREL - How to Play\n\n";
	helpText << "You are a scoundrel exploring dungeons filled with monsters.\n\n";
	helpText << "CARDS:\n";
	helpText << "- Monsters (red): Attack to defeat. Takes damage if unarmed.\n";
	helpText << "- Weapons (blue): Equip to reduce damage from monsters.\n";
	helpText << "- Potions (green): Drink to restore health.\n\n";
	helpText << "COMBAT:\n";
	helpText << "- Unarmed attack: Take full monster damage.\n";
	helpText << "- With weapon: Damage = monster - weapon (min 0).\n";
	helpText << "- Weapon can only attack weaker monsters than last attacked.\n";
	helpText << "- Weapon breaks after attacking strength-2 monster.\n\n";
	helpText << "RULES:\n";
	helpText << "- One health potion per room.\n";
	helpText << "- Flee returns cards to deck (can't flee next room).\n";
	helpText << "- Clear all cards to beat the dungeon.\n";
	helpText << "- Score = monsters defeated + health remaining.";

	BAlert* alert = new BAlert("How to Play", helpText.String(), "OK");
	alert->Go();
}


// GameObserver implementation

void
GameWindow::OnGameStateChanged(GameState newState)
{
	switch (newState) {
		case kGameStateGameOver:
			ShowGameOverModal();
			SaveManager::Instance()->DeleteSavedGame();
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
	if (fGameBoardView != NULL)
		fGameBoardView->Refresh();
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
