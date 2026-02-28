#ifndef ROOM_VIEW_H
#define ROOM_VIEW_H

#include <View.h>
#include <MessageRunner.h>
#include <Point.h>

class CardView;
class Room;
class Card;
class BBitmap;

// Animation state for a single card
struct CardAnimation {
	Card*		card;
	BPoint		startPos;
	BPoint		endPos;
	float		startScale;
	float		endScale;
	float		progress;		// 0.0 to 1.0
	bool		active;
};

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
	void				DrawAnimatingCard(CardAnimation& anim);
	float				EaseOutCubic(float t);
	BPoint				GetCardCenterPosition(int index);

	CardView*			fCardViews[4];
	Room*				fRoom;
	int					fBackgroundIndex;

	// Animation state
	bool				fIsDealing;
	int					fNextCardToDeal;
	CardAnimation		fAnimations[4];
	BMessageRunner*		fAnimationRunner;
	BMessageRunner*		fDealRunner;
	BPoint				fDeckPosition;		// In window coordinates
};

#endif // ROOM_VIEW_H
