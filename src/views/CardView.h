#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include <View.h>
#include <Point.h>
#include <Rect.h>

class Card;

class CardView : public BView {
public:
						CardView(BRect frame, int32 index);
	virtual				~CardView();

	virtual void		Draw(BRect updateRect);
	virtual void		MouseDown(BPoint where);

	void				SetCard(Card* card);
	void				ClearCard();
	void				SetBackgroundIndex(int index);
	int32				Index() const { return fIndex; }
	Card*				GetCard() const { return fCard; }

	void				SetAnimating(bool animating) { fIsAnimating = animating; }
	bool				IsAnimating() const { return fIsAnimating; }

private:
	void				DrawEmptySlot();
	void				DrawCard();

	int32				fIndex;
	Card*				fCard;
	int					fBackgroundIndex;
	bool				fIsAnimating;
};

#endif // CARD_VIEW_H
