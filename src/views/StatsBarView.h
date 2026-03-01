#ifndef STATS_BAR_VIEW_H
#define STATS_BAR_VIEW_H

#include <InterfaceDefs.h>
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
	void				DrawIconBox(BRect boxRect, const char* iconName, int value);
	void				DrawProgressBar(BRect barRect, rgb_color fillColor,
							float fillRatio);
	void				UpdateTooltips();

	Player*				fPlayer;
	BView*				fSwordTooltipView;
};

#endif // STATS_BAR_VIEW_H
