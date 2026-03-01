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
	virtual void		MouseMoved(BPoint where, uint32 transit,
							const BMessage* dragMessage);

	void				SetPlayer(Player* player);
	void				Refresh();

private:
	void				DrawIconBox(BRect boxRect, const char* iconName, int value);
	void				DrawProgressBar(BRect barRect, rgb_color fillColor,
							float fillRatio);
	BRect				GetSwordBoxRect();
	void				UpdateTooltipForPoint(BPoint point);

	Player*				fPlayer;
};

#endif // STATS_BAR_VIEW_H
