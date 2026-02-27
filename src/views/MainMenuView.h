#ifndef MAIN_MENU_VIEW_H
#define MAIN_MENU_VIEW_H

#include <Button.h>
#include <StringView.h>
#include <View.h>

class MainMenuView : public BView {
public:
						MainMenuView(BRect frame);
	virtual				~MainMenuView();

	virtual void		Draw(BRect updateRect);
	virtual void		AttachedToWindow();

	void				SetHasSavedGame(bool hasSaved);

private:
	BStringView*		fTitleView;
	BButton*			fNewGameButton;
	BButton*			fResumeButton;
	BButton*			fHowToPlayButton;
};

#endif // MAIN_MENU_VIEW_H
