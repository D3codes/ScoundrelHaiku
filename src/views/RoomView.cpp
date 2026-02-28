#include "RoomView.h"
#include "CardView.h"
#include "models/Room.h"
#include "helpers/ResourceLoader.h"
#include "helpers/SoundPlayer.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <String.h>
#include <Window.h>

static const bigtime_t kAnimationInterval = 16667;  // ~60fps (microseconds)
static const bigtime_t kDealDelay = 300000;         // 300ms between cards

RoomView::RoomView(BRect frame)
	:
	BView(frame, "roomView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE),
	fRoom(NULL),
	fBackgroundIndex(1),
	fIsDealing(false),
	fNextCardToDeal(0),
	fAnimationRunner(NULL),
	fDealRunner(NULL),
	fDeckPosition(0, 0)
{
	// Use solid color as fallback
	SetViewColor(kBackgroundColor);

	for (int i = 0; i < 4; i++)
		fCardsToAnimate[i] = false;

	// Calculate card positions for 2x2 grid
	float cardWidth = kCardWidth;
	float cardHeight = kCardHeight;
	float spacing = kCardSpacing;

	float totalWidth = cardWidth * 2 + spacing;
	float totalHeight = cardHeight * 2 + spacing;
	float startX = (frame.Width() - totalWidth) / 2;
	float startY = (frame.Height() - totalHeight) / 2;

	// Create card views in 2x2 grid
	for (int i = 0; i < 4; i++) {
		int row = i / 2;
		int col = i % 2;
		float x = startX + col * (cardWidth + spacing);
		float y = startY + row * (cardHeight + spacing);

		BRect cardFrame(x, y, x + cardWidth, y + cardHeight);
		fCardViews[i] = new CardView(cardFrame, i);
		AddChild(fCardViews[i]);
	}
}


RoomView::~RoomView()
{
	delete fAnimationRunner;
	delete fDealRunner;
}


void
RoomView::AttachedToWindow()
{
	BView::AttachedToWindow();
}


void
RoomView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgAnimationTick:
			UpdateAnimations();
			break;

		case kMsgDealNextCard:
			DealNextCard();
			break;

		default:
			BView::MessageReceived(message);
			break;
	}
}


void
RoomView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();

	// Fill with solid background first
	SetHighColor(kBackgroundColor);
	FillRect(bounds);

	// Draw dungeon background
	BString bgName;
	bgName.SetToFormat("dungeon%d", fBackgroundIndex);
	BBitmap* background = ResourceLoader::Instance()->GetBackground(bgName.String());

	if (background != NULL && Parent() != NULL) {
		// Calculate source rect based on our position in parent
		BRect parentBounds = Parent()->Bounds();
		BRect srcRect = background->Bounds();

		// Scale to parent size
		float scaleX = srcRect.Width() / parentBounds.Width();
		float scaleY = srcRect.Height() / parentBounds.Height();

		// Our position in parent
		BPoint pos = Frame().LeftTop();

		// Calculate the portion of the background that corresponds to our area
		BRect ourSrcRect(
			pos.x * scaleX,
			pos.y * scaleY,
			(pos.x + bounds.Width()) * scaleX,
			(pos.y + bounds.Height()) * scaleY
		);

		SetDrawingMode(B_OP_COPY);
		DrawBitmap(background, ourSrcRect, bounds);
	}
}


void
RoomView::SetBackgroundIndex(int index)
{
	fBackgroundIndex = index;
	// Update all card views with the new background index
	for (int i = 0; i < 4; i++) {
		fCardViews[i]->SetBackgroundIndex(index);
	}
	Invalidate();
}


void
RoomView::SetDeckPosition(BPoint deckPos)
{
	fDeckPosition = deckPos;
	for (int i = 0; i < 4; i++) {
		fCardViews[i]->SetDeckPosition(deckPos);
	}
}


void
RoomView::SetRoom(Room* room)
{
	fRoom = room;
	Refresh();
}


void
RoomView::Refresh()
{
	if (fRoom == NULL) {
		for (int i = 0; i < 4; i++) {
			fCardViews[i]->ClearCard();
		}
		return;
	}

	for (int i = 0; i < 4; i++) {
		Card* card = fRoom->GetCard(i);
		if (card != NULL) {
			fCardViews[i]->SetCard(card);
		} else {
			fCardViews[i]->ClearCard();
		}
	}

	Invalidate();
}


void
RoomView::RefreshWithAnimation()
{
	if (fRoom == NULL) {
		Refresh();
		return;
	}

	// Stop any existing animation
	delete fAnimationRunner;
	fAnimationRunner = NULL;
	delete fDealRunner;
	fDealRunner = NULL;

	// Figure out which cards need to be dealt (currently NULL in view but exist in room)
	fNextCardToDeal = -1;
	for (int i = 0; i < 4; i++) {
		Card* roomCard = fRoom->GetCard(i);
		// If room has a card but view doesn't show it yet
		fCardsToAnimate[i] = (roomCard != NULL);

		// Clear all cards first to show empty slots
		fCardViews[i]->ClearCard();
	}

	// Find first card to deal
	for (int i = 0; i < 4; i++) {
		if (fCardsToAnimate[i]) {
			fNextCardToDeal = i;
			break;
		}
	}

	if (fNextCardToDeal >= 0) {
		fIsDealing = true;
		// Start dealing after a short delay
		BMessage dealMsg(kMsgDealNextCard);
		fDealRunner = new BMessageRunner(BMessenger(this), &dealMsg, kDealDelay, 1);
	}

	Invalidate();
}


void
RoomView::DealNextCard()
{
	if (fNextCardToDeal < 0 || fNextCardToDeal >= 4 || fRoom == NULL) {
		fIsDealing = false;
		return;
	}

	// Find the next card to deal
	while (fNextCardToDeal < 4 && !fCardsToAnimate[fNextCardToDeal]) {
		fNextCardToDeal++;
	}

	if (fNextCardToDeal >= 4) {
		fIsDealing = false;
		return;
	}

	Card* card = fRoom->GetCard(fNextCardToDeal);
	if (card != NULL) {
		// Play deal sound
		SoundPlayer::Instance()->PlaySound(SFX_DEAL_CARD);

		// Calculate start position (deck position in our coordinate system)
		BPoint startPos = fDeckPosition;
		// Convert from window coordinates to our coordinates
		if (Parent() != NULL) {
			startPos.x -= Frame().left;
			startPos.y -= Frame().top;
		}

		// Start the card animation
		fCardViews[fNextCardToDeal]->SetCardWithAnimation(card, startPos, 0.2f);

		// Start animation timer if not already running
		if (fAnimationRunner == NULL) {
			BMessage animMsg(kMsgAnimationTick);
			fAnimationRunner = new BMessageRunner(BMessenger(this), &animMsg,
				kAnimationInterval, -1); // -1 = infinite
		}
	}

	fCardsToAnimate[fNextCardToDeal] = false;
	fNextCardToDeal++;

	// Schedule next card deal
	bool moreCards = false;
	for (int i = fNextCardToDeal; i < 4; i++) {
		if (fCardsToAnimate[i]) {
			moreCards = true;
			break;
		}
	}

	if (moreCards) {
		delete fDealRunner;
		BMessage dealMsg(kMsgDealNextCard);
		fDealRunner = new BMessageRunner(BMessenger(this), &dealMsg, kDealDelay, 1);
	}
}


void
RoomView::UpdateAnimations()
{
	bool anyAnimating = false;

	for (int i = 0; i < 4; i++) {
		if (fCardViews[i]->IsAnimating()) {
			fCardViews[i]->UpdateAnimation();
			anyAnimating = true;
		}
	}

	// Stop animation timer if no cards are animating
	if (!anyAnimating) {
		delete fAnimationRunner;
		fAnimationRunner = NULL;

		// Check if we're done dealing
		bool moreToDeal = false;
		for (int i = 0; i < 4; i++) {
			if (fCardsToAnimate[i]) {
				moreToDeal = true;
				break;
			}
		}
		if (!moreToDeal) {
			fIsDealing = false;
		}
	}
}
