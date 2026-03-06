#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <Looper.h>
#include <ObjectList.h>
#include <Path.h>
#include <String.h>

class BFileGameSound;

class MusicPlayer : public BLooper {
public:
	static MusicPlayer*	Instance();
	static void			Destroy();

	virtual void		MessageReceived(BMessage* message);

	void				Start();
	void				Stop();
	void				SetMuted(bool muted);
	bool				IsMuted() const { return fMuted; }

	void				LoadSettings();
	void				SaveSettings();

private:
						MusicPlayer();
	virtual				~MusicPlayer();

	void				PlayCurrentTrack();
	void				NextTrack();
	void				ShufflePlaylist();

	static MusicPlayer*	sInstance;

	BObjectList<BString> fTracks;
	int32				fCurrentTrack;
	BFileGameSound*		fCurrentSound;
	BPath				fMusicPath;
	bool				fMuted;
	bool				fPlaying;
};

#endif // MUSIC_PLAYER_H
