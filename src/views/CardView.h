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
	void				SetCardWithAnimation(Card* card, BPoint startPos, float startScale);
	void				ClearCard();
	void				SetBackgroundIndex(int index);
	int32				Index() const { return fIndex; }

	// Animation
	void				UpdateAnimation();
	bool				IsAnimating() const { return fIsAnimating; }
	void				SetDeckPosition(BPoint deckPos) { fDeckPosition = deckPos; }

private:
	void				DrawEmptySlot();
	void				DrawCard();
	void				DrawAnimatingCard();

	int32				fIndex;
	Card*				fCard;
	int					fBackgroundIndex;

	// Animation state
	bool				fIsAnimating;
	float				fAnimationProgress;  // 0.0 to 1.0
	BPoint				fStartPosition;      // Position relative to parent
	BPoint				fEndPosition;        // Final position (our frame origin)
	float				fStartScale;         // Starting scale (small)
	float				fEndScale;           // Final scale (1.0)
	BPoint				fDeckPosition;       // Where deck icon is located
};

#endif // CARD_VIEW_H
