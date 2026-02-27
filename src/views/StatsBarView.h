#ifndef STATS_BAR_VIEW_H
#define STATS_BAR_VIEW_H

#include <View.h>

class Player;

class StatsBarView : public BView {
public:
						StatsBarView(BRect frame);
	virtual				~StatsBarView();

	virtual void		Draw(BRect updateRect);

	void				SetPlayer(Player* player);
	void				Refresh();

private:
	void				DrawHealthBar(BRect barRect);
	void				DrawWeaponBar(BRect barRect);

	Player*				fPlayer;
};

#endif // STATS_BAR_VIEW_H
