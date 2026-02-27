#ifndef APP_H
#define APP_H

#include <Application.h>

class GameWindow;

class ScoundrelApp : public BApplication {
public:
						ScoundrelApp();
	virtual				~ScoundrelApp();

	virtual void		ReadyToRun();
	virtual bool		QuitRequested();

private:
	GameWindow*			fWindow;
};

#endif // APP_H
