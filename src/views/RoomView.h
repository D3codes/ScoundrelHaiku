#ifndef ROOM_VIEW_H
#define ROOM_VIEW_H

#include <View.h>
#include <MessageRunner.h>
#include <Point.h>

class CardView;
class Room;

class RoomView : public BView {
public:
						RoomView(BRect frame);
	virtual				~RoomView();

	virtual void		Draw(BRect updateRect);
	virtual void		AttachedToWindow();
	virtual void		MessageReceived(BMessage* message);

	void				SetRoom(Room* room);
	void				SetBackgroundIndex(int index);
	void				SetDeckPosition(BPoint deckPos);
	void				Refresh();
	void				RefreshWithAnimation();
	bool				IsDealing() const { return fIsDealing; }

private:
	void				DealNextCard();
	void				UpdateAnimations();

	CardView*			fCardViews[4];
	Room*				fRoom;
	int					fBackgroundIndex;

	// Animation state
	bool				fIsDealing;
	int					fNextCardToDeal;
	bool				fCardsToAnimate[4];
	BMessageRunner*		fAnimationRunner;
	BMessageRunner*		fDealRunner;
	BPoint				fDeckPosition;
};

#endif // ROOM_VIEW_H
