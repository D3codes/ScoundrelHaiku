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
	void				Refresh();

private:
	CardView*			fCardViews[4];
	Room*				fRoom;
};

#endif // ROOM_VIEW_H
