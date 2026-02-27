#ifndef ROOM_VIEW_H
#define ROOM_VIEW_H

#include <View.h>

class CardView;
class Room;

class RoomView : public BView {
public:
						RoomView(BRect frame);
	virtual				~RoomView();

	virtual void		Draw(BRect updateRect);
	virtual void		AttachedToWindow();

	void				SetRoom(Room* room);
	void				SetBackgroundIndex(int index);
	void				Refresh();

private:
	CardView*			fCardViews[4];
	Room*				fRoom;
	int					fBackgroundIndex;
};

#endif // ROOM_VIEW_H
