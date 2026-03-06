#include "MusicPlayer.h"
#include "ResourceLoader.h"

#include <Directory.h>
#include <File.h>
#include <FileGameSound.h>
#include <FindDirectory.h>
#include <Message.h>
#include <MessageRunner.h>
#include <stdlib.h>
#include <time.h>

static const uint32 kMsgCheckTrack = 'CHKT';

MusicPlayer* MusicPlayer::sInstance = NULL;


MusicPlayer*
MusicPlayer::Instance()
{
	if (sInstance == NULL) {
		sInstance = new MusicPlayer();
		sInstance->Run();
	}
	return sInstance;
}


void
MusicPlayer::Destroy()
{
	if (sInstance != NULL) {
		sInstance->Stop();
		sInstance->Lock();
		sInstance->Quit();
		sInstance = NULL;
	}
}


MusicPlayer::MusicPlayer()
	:
	BLooper("MusicPlayer"),
	fTracks(10, true),
	fCurrentTrack(0),
	fCurrentSound(NULL),
	fMuted(false),
	fPlaying(false)
{
	// Seed random number generator
	srand(time(NULL));

	// Set up music path
	BPath dataPath = ResourceLoader::Instance()->GetDataPath();
	fMusicPath = dataPath;
	fMusicPath.Append("sounds/music");

	// Add tracks in the playlist order
	fTracks.AddItem(new BString("Knights in Shadows"));
	fTracks.AddItem(new BString("The Hollow Crown"));
	fTracks.AddItem(new BString("Wanderer's Requiem"));
	fTracks.AddItem(new BString("Oaths Upon the Wind"));
	fTracks.AddItem(new BString("Ballad of the Wayfarer"));
	fTracks.AddItem(new BString("Legends of the Silver Road"));
	fTracks.AddItem(new BString("Call of the Noble Heart"));
	fTracks.AddItem(new BString("Adventure Awaits"));

	LoadSettings();
}


MusicPlayer::~MusicPlayer()
{
	Stop();
}


void
MusicPlayer::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgCheckTrack:
			// Check if current track has finished
			if (fPlaying && fCurrentSound != NULL) {
				if (!fCurrentSound->IsPlaying()) {
					NextTrack();
				}
			}
			break;

		default:
			BLooper::MessageReceived(message);
			break;
	}
}


void
MusicPlayer::Start()
{
	if (fMuted || fPlaying)
		return;

	// Pick a random starting track
	fCurrentTrack = rand() % fTracks.CountItems();
	fPlaying = true;
	PlayCurrentTrack();

	// Start a message runner to check for track completion
	BMessage checkMsg(kMsgCheckTrack);
	BMessageRunner* runner = new BMessageRunner(BMessenger(this), &checkMsg,
		500000, -1);  // Check every 500ms
	(void)runner;  // Runner will be cleaned up when looper quits
}


void
MusicPlayer::Stop()
{
	fPlaying = false;
	if (fCurrentSound != NULL) {
		fCurrentSound->StopPlaying();
		delete fCurrentSound;
		fCurrentSound = NULL;
	}
}


void
MusicPlayer::SetMuted(bool muted)
{
	if (fMuted == muted)
		return;

	fMuted = muted;
	SaveSettings();

	if (muted) {
		Stop();
	} else {
		Start();
	}
}


void
MusicPlayer::PlayCurrentTrack()
{
	if (fMuted || !fPlaying)
		return;

	// Stop any currently playing sound
	if (fCurrentSound != NULL) {
		fCurrentSound->StopPlaying();
		delete fCurrentSound;
		fCurrentSound = NULL;
	}

	// Get the track name
	BString* trackName = fTracks.ItemAt(fCurrentTrack);
	if (trackName == NULL)
		return;

	// Build the path to the music file
	BPath path(fMusicPath);
	BString filename;
	filename.SetToFormat("%s.mp3", trackName->String());
	path.Append(filename.String());

	// Create and start the sound
	entry_ref ref;
	if (get_ref_for_path(path.Path(), &ref) != B_OK)
		return;

	fCurrentSound = new BFileGameSound(&ref, false);
	if (fCurrentSound->InitCheck() == B_OK) {
		fCurrentSound->SetGain(1.0);
		fCurrentSound->StartPlaying();
	} else {
		delete fCurrentSound;
		fCurrentSound = NULL;
	}
}


void
MusicPlayer::NextTrack()
{
	fCurrentTrack++;
	if (fCurrentTrack >= fTracks.CountItems())
		fCurrentTrack = 0;

	PlayCurrentTrack();
}


void
MusicPlayer::ShufflePlaylist()
{
	// Fisher-Yates shuffle
	int count = fTracks.CountItems();
	for (int i = count - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		if (i != j) {
			BString* temp = fTracks.RemoveItemAt(i);
			BString* other = fTracks.RemoveItemAt(j);
			fTracks.AddItem(temp, j);
			fTracks.AddItem(other, i);
		}
	}
}


void
MusicPlayer::LoadSettings()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	path.Append("Scoundrel");
	path.Append("settings");

	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	BMessage archive;
	if (archive.Unflatten(&file) != B_OK)
		return;

	bool muted;
	if (archive.FindBool("musicMuted", &muted) == B_OK)
		fMuted = muted;
}


void
MusicPlayer::SaveSettings()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	path.Append("Scoundrel");

	// Create directory if it doesn't exist
	BDirectory dir;
	dir.CreateDirectory(path.Path(), NULL);

	path.Append("settings");

	// Read existing settings first
	BMessage archive;
	BFile readFile(path.Path(), B_READ_ONLY);
	if (readFile.InitCheck() == B_OK) {
		archive.Unflatten(&readFile);
	}
	readFile.Unset();

	// Update music muted setting
	archive.RemoveName("musicMuted");
	archive.AddBool("musicMuted", fMuted);

	// Write back
	BFile writeFile(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (writeFile.InitCheck() == B_OK) {
		archive.Flatten(&writeFile);
	}
}
