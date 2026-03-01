#ifndef HIGH_SCORE_MANAGER_H
#define HIGH_SCORE_MANAGER_H

#include <String.h>
#include <ObjectList.h>

struct HighScoreEntry {
	BString		name;
	int			score;
	int			dungeonsBeaten;
};

class HighScoreManager {
public:
	static HighScoreManager*	Instance();

	void						AddScore(const char* name, int score,
									int dungeonsBeaten);
	BObjectList<HighScoreEntry>* GetScores();
	int							GetScoreCount();
	HighScoreEntry*				GetScore(int index);

	void						Load();
	void						Save();

private:
								HighScoreManager();
								~HighScoreManager();

	static HighScoreManager*	sInstance;
	BObjectList<HighScoreEntry>	fScores;
};

#endif // HIGH_SCORE_MANAGER_H
