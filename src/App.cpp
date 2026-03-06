#include "App.h"
#include "views/GameWindow.h"
#include "helpers/ResourceLoader.h"
#include "helpers/SoundPlayer.h"
#include "helpers/MusicPlayer.h"
#include "helpers/SaveManager.h"
#include "utils/Constants.h"

ScoundrelApp::ScoundrelApp()
	:
	BApplication(APP_SIGNATURE),
	fWindow(NULL)
{
	// Initialize singletons
	ResourceLoader::Instance();
	SoundPlayer::Instance();
	SaveManager::Instance();
}


ScoundrelApp::~ScoundrelApp()
{
	// Destroy singletons
	MusicPlayer::Destroy();
	SaveManager::Destroy();
	SoundPlayer::Destroy();
	ResourceLoader::Destroy();
}


void
ScoundrelApp::ReadyToRun()
{
	fWindow = new GameWindow();
	fWindow->Show();

	// Start background music
	MusicPlayer::Instance()->Start();
}


bool
ScoundrelApp::QuitRequested()
{
	return true;
}
