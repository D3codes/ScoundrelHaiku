#include "RoomView.h"
#include "CardView.h"
#include "models/Room.h"
#include "models/Card.h"
#include "helpers/ResourceLoader.h"
#include "helpers/SoundPlayer.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <String.h>
#include <Window.h>

static const bigtime_t kAnimationInterval = 16667;  // ~60fps (microseconds)
static const bigtime_t kDealDelay = 350000;         // 350ms between cards
static const float kAnimationDuration = 0.5f;       // 500ms per card animation
static const float kAnimationStep = kAnimationInterval / 1000000.0f / kAnimationDuration;

RoomView::RoomView(BRect frame)
	:
	BView(frame, "roomView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE),
	fRoom(NULL),
	fBackgroundIndex(1),
	fIsDealing(false),
	fIsFleeing(false),
	fFleeAnimationPending(false),
	fNextCardToDeal(0),
	fAnimationRunner(NULL),
	fDealRunner(NULL),
	fDeckPosition(0, 0)
{
	SetViewColor(B_TRANSPARENT_COLOR);

	// Initialize animation states
	for (int i = 0; i < 4; i++) {
		fAnimations[i].card = NULL;
		fAnimations[i].active = false;
		fAnimations[i].progress = 0.0f;

		fFleeAnimations[i].card = NULL;
		fFleeAnimations[i].active = false;
		fFleeAnimations[i].progress = 0.0f;
	}

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


float
RoomView::EaseOutCubic(float t)
{
	float t1 = t - 1.0f;
	return 1.0f + t1 * t1 * t1;
}


BPoint
RoomView::GetCardCenterPosition(int index)
{
	if (index < 0 || index >= 4)
		return BPoint(0, 0);

	BRect cardFrame = fCardViews[index]->Frame();
	return BPoint(
		cardFrame.left + cardFrame.Width() / 2,
		cardFrame.top + cardFrame.Height() / 2
	);
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

	// Draw flee animations (cards going back to deck)
	for (int i = 0; i < 4; i++) {
		if (fFleeAnimations[i].active) {
			DrawAnimatingCard(fFleeAnimations[i]);
		}
	}

	// Draw deal animations (cards coming from deck)
	for (int i = 0; i < 4; i++) {
		if (fAnimations[i].active) {
			DrawAnimatingCard(fAnimations[i]);
		}
	}
}


void
RoomView::DrawAnimatingCard(CardAnimation& anim)
{
	if (anim.card == NULL)
		return;

	float t = EaseOutCubic(anim.progress);

	// Interpolate position
	float x = anim.startPos.x + (anim.endPos.x - anim.startPos.x) * t;
	float y = anim.startPos.y + (anim.endPos.y - anim.startPos.y) * t;

	// Interpolate scale
	float scale = anim.startScale + (anim.endScale - anim.startScale) * t;

	// Calculate card dimensions
	float cardWidth = kCardWidth * scale;
	float cardHeight = kCardHeight * scale;

	// Calculate card rect (centered on position)
	BRect cardRect(
		x - cardWidth / 2,
		y - cardHeight / 2,
		x + cardWidth / 2,
		y + cardHeight / 2
	);

	float radius = 12 * scale;

	// Draw card shadow
	SetHighColor(0, 0, 0, 100);
	BRect shadowRect = cardRect;
	shadowRect.OffsetBy(4 * scale, 4 * scale);
	FillRoundRect(shadowRect, radius, radius);

	// Draw card background
	SetHighColor(kCardBackgroundColor);
	FillRoundRect(cardRect, radius, radius);

	// Draw paper texture
	BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
	if (paperBg != NULL) {
		BRect insetBounds = cardRect.InsetByCopy(2 * scale, 2 * scale);
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(paperBg, paperBg->Bounds(), insetBounds);
		SetDrawingMode(B_OP_COPY);
	}

	// Draw card border
	SetHighColor(180, 170, 150);
	StrokeRoundRect(cardRect, radius, radius);

	// Draw card image
	BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
		anim.card->GetImageName().String());

	float bottomAreaHeight = 45 * scale;
	float imageAreaHeight = cardRect.Height() - bottomAreaHeight;

	if (cardImage != NULL) {
		BRect imageRect = cardImage->Bounds();
		float padding = 10 * scale;
		float availWidth = cardRect.Width() - padding * 2;
		float availHeight = imageAreaHeight - padding * 2;

		float imgScale = availWidth / imageRect.Width();
		if (availHeight / imageRect.Height() < imgScale)
			imgScale = availHeight / imageRect.Height();

		float imageWidth = imageRect.Width() * imgScale;
		float imageHeight = imageRect.Height() * imgScale;
		float imageX = cardRect.left + (cardRect.Width() - imageWidth) / 2;
		float imageY = cardRect.top + padding;

		BRect destRect(imageX, imageY, imageX + imageWidth, imageY + imageHeight);

		float imageRadius = 8 * scale;
		SetHighColor(kCardBackgroundColor);
		FillRoundRect(destRect, imageRadius, imageRadius);

		BRect insetDestRect = destRect.InsetByCopy(2 * scale, 2 * scale);
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(cardImage, imageRect, insetDestRect);
		SetDrawingMode(B_OP_COPY);

		SetHighColor(160, 150, 130);
		StrokeRoundRect(destRect, imageRadius, imageRadius);
	} else {
		// Fallback: draw icon
		BBitmap* icon = ResourceLoader::Instance()->GetGlyph(
			anim.card->GetIconName().String());
		if (icon != NULL) {
			BRect iconRect = icon->Bounds();
			float iconX = cardRect.left + (cardRect.Width() - iconRect.Width() * scale) / 2;
			float iconY = cardRect.top + (imageAreaHeight - iconRect.Height() * scale) / 2;
			BRect destIcon(iconX, iconY,
				iconX + iconRect.Width() * scale,
				iconY + iconRect.Height() * scale);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(icon, iconRect, destIcon);
			SetDrawingMode(B_OP_COPY);
		}
	}

	// Draw suit icon and strength at bottom
	float iconSize = 24 * scale;
	BBitmap* suitIcon = ResourceLoader::Instance()->GetGlyph(
		anim.card->GetIconName().String());

	BFont font;
	font.SetSize(24 * scale);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	BString strengthStr;
	strengthStr.SetToFormat("%d", anim.card->Strength());
	float textWidth = StringWidth(strengthStr.String());

	float spacing = 8 * scale;
	float totalWidth = iconSize + spacing + textWidth;
	float startX = cardRect.left + (cardRect.Width() - totalWidth) / 2;
	float bottomY = cardRect.bottom - 12 * scale;

	if (suitIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		BRect srcIcon = suitIcon->Bounds();
		BRect destIcon(startX, bottomY - iconSize, startX + iconSize, bottomY);
		DrawBitmap(suitIcon, srcIcon, destIcon);
		SetDrawingMode(B_OP_COPY);
	}

	SetHighColor(0, 0, 0);
	DrawString(strengthStr.String(),
		BPoint(startX + iconSize + spacing, bottomY - 3 * scale));
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
	// Don't override card views if animation is in progress
	if (fIsDealing || fIsFleeing)
		return;

	if (fRoom == NULL) {
		for (int i = 0; i < 4; i++) {
			fCardViews[i]->ClearCard();
			if (fCardViews[i]->IsHidden())
				fCardViews[i]->Show();
		}
		return;
	}

	for (int i = 0; i < 4; i++) {
		Card* card = fRoom->GetCard(i);
		if (card != NULL) {
			fCardViews[i]->SetCard(card);
			if (fCardViews[i]->IsHidden())
				fCardViews[i]->Show();
		} else {
			fCardViews[i]->ClearCard();
			// Keep empty slots visible to show background
			if (fCardViews[i]->IsHidden())
				fCardViews[i]->Show();
		}
	}

	Invalidate();
}


void
RoomView::ClearAllCards()
{
	// Clear all card views and hide them (for starting fresh before animation)
	for (int i = 0; i < 4; i++) {
		fCardViews[i]->ClearCard();
		if (!fCardViews[i]->IsHidden())
			fCardViews[i]->Hide();
	}
	Invalidate();
}


void
RoomView::PrepareFleeAnimation()
{
	// Capture current cards for flee animation
	// Calculate deck position in our coordinate system
	BPoint deckPosInRoom = fDeckPosition;
	if (Parent() != NULL) {
		BPoint ourPos = Frame().LeftTop();
		deckPosInRoom.x -= ourPos.x;
		deckPosInRoom.y -= ourPos.y;
	}

	// Set up flee animations for all visible cards
	for (int i = 0; i < 4; i++) {
		Card* viewCard = fCardViews[i]->GetCard();

		fFleeAnimations[i].active = false;
		fFleeAnimations[i].progress = 0.0f;

		if (viewCard != NULL) {
			// Card exists - set up animation to move back to deck
			fFleeAnimations[i].card = viewCard;
			fFleeAnimations[i].startPos = GetCardCenterPosition(i);
			fFleeAnimations[i].endPos = deckPosInRoom;
			fFleeAnimations[i].startScale = 1.0f;  // Start full size
			fFleeAnimations[i].endScale = 0.3f;    // End small (deck size)
		} else {
			fFleeAnimations[i].card = NULL;
		}
	}

	fFleeAnimationPending = true;
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

	// Calculate deck position in our coordinate system
	BPoint deckPosInRoom = fDeckPosition;
	if (Parent() != NULL) {
		BPoint ourPos = Frame().LeftTop();
		deckPosInRoom.x -= ourPos.x;
		deckPosInRoom.y -= ourPos.y;
	}

	// If flee animation is pending, start it first
	if (fFleeAnimationPending) {
		fFleeAnimationPending = false;
		fIsFleeing = true;

		// Hide all CardViews and start flee animations
		bool anyFleeing = false;
		for (int i = 0; i < 4; i++) {
			if (fFleeAnimations[i].card != NULL) {
				fFleeAnimations[i].active = true;
				fFleeAnimations[i].progress = 0.0f;

				// Hide CardView - we'll draw the animated card
				fCardViews[i]->ClearCard();
				if (!fCardViews[i]->IsHidden())
					fCardViews[i]->Hide();

				anyFleeing = true;
			}
		}

		// Set up deal animations for later (after flee completes)
		fNextCardToDeal = -1;
		for (int i = 0; i < 4; i++) {
			Card* roomCard = fRoom->GetCard(i);

			fAnimations[i].active = false;
			fAnimations[i].progress = 0.0f;

			if (roomCard != NULL) {
				// All cards are new after a flee
				fAnimations[i].card = roomCard;
				fAnimations[i].startPos = deckPosInRoom;
				fAnimations[i].endPos = GetCardCenterPosition(i);
				fAnimations[i].startScale = 0.3f;
				fAnimations[i].endScale = 1.0f;

				if (fNextCardToDeal < 0)
					fNextCardToDeal = i;
			} else {
				fAnimations[i].card = NULL;
			}
		}

		if (anyFleeing) {
			// Start animation timer
			BMessage animMsg(kMsgAnimationTick);
			fAnimationRunner = new BMessageRunner(BMessenger(this), &animMsg,
				kAnimationInterval, -1);
		}

		Invalidate();
		return;
	}

	// Normal deal animation (no flee)
	fNextCardToDeal = -1;
	for (int i = 0; i < 4; i++) {
		Card* roomCard = fRoom->GetCard(i);
		Card* viewCard = fCardViews[i]->GetCard();

		bool isCarriedOver = (roomCard != NULL && roomCard == viewCard);
		bool isNewCard = (roomCard != NULL && roomCard != viewCard);

		fAnimations[i].active = false;
		fAnimations[i].progress = 0.0f;

		if (isCarriedOver) {
			fAnimations[i].card = NULL;
			if (fCardViews[i]->IsHidden())
				fCardViews[i]->Show();
		} else if (isNewCard) {
			fAnimations[i].card = roomCard;
			fAnimations[i].startPos = deckPosInRoom;
			fAnimations[i].endPos = GetCardCenterPosition(i);
			fAnimations[i].startScale = 0.3f;
			fAnimations[i].endScale = 1.0f;

			fCardViews[i]->ClearCard();
			if (!fCardViews[i]->IsHidden())
				fCardViews[i]->Hide();

			if (fNextCardToDeal < 0)
				fNextCardToDeal = i;
		} else {
			fAnimations[i].card = NULL;
			fCardViews[i]->ClearCard();
			if (!fCardViews[i]->IsHidden())
				fCardViews[i]->Hide();
		}
	}

	if (fNextCardToDeal >= 0) {
		fIsDealing = true;
		DealNextCard();
	}

	Invalidate();
}


void
RoomView::StartDealAnimation()
{
	// Called after flee animation completes to start dealing new cards
	fIsFleeing = false;
	fIsDealing = true;

	if (fNextCardToDeal >= 0) {
		DealNextCard();
	} else {
		fIsDealing = false;
	}
}


void
RoomView::DealNextCard()
{
	if (fNextCardToDeal < 0 || fNextCardToDeal >= 4 || fRoom == NULL) {
		fIsDealing = false;
		return;
	}

	// Find the next card to deal (skip NULL entries)
	while (fNextCardToDeal < 4 && fAnimations[fNextCardToDeal].card == NULL) {
		fNextCardToDeal++;
	}

	if (fNextCardToDeal >= 4) {
		fIsDealing = false;
		return;
	}

	// Play deal sound
	SoundPlayer::Instance()->PlaySound(SFX_DEAL_CARD);

	// Start this card's animation
	fAnimations[fNextCardToDeal].active = true;
	fAnimations[fNextCardToDeal].progress = 0.0f;

	// Start animation timer if not already running
	if (fAnimationRunner == NULL) {
		BMessage animMsg(kMsgAnimationTick);
		fAnimationRunner = new BMessageRunner(BMessenger(this), &animMsg,
			kAnimationInterval, -1);
	}

	fNextCardToDeal++;

	// Schedule next card deal if there are more new cards to animate
	bool moreCards = false;
	for (int i = fNextCardToDeal; i < 4; i++) {
		if (fAnimations[i].card != NULL) {
			moreCards = true;
			break;
		}
	}

	if (moreCards) {
		delete fDealRunner;
		BMessage dealMsg(kMsgDealNextCard);
		fDealRunner = new BMessageRunner(BMessenger(this), &dealMsg, kDealDelay, 1);
	}

	Invalidate();
}


void
RoomView::UpdateAnimations()
{
	bool anyAnimating = false;

	// Update flee animations (all cards animate simultaneously)
	if (fIsFleeing) {
		bool anyFleeing = false;
		for (int i = 0; i < 4; i++) {
			if (fFleeAnimations[i].active) {
				fFleeAnimations[i].progress += kAnimationStep;

				if (fFleeAnimations[i].progress >= 1.0f) {
					fFleeAnimations[i].progress = 1.0f;
					fFleeAnimations[i].active = false;
				} else {
					anyFleeing = true;
				}
			}
		}

		if (!anyFleeing) {
			// Flee animation complete - start deal animation
			StartDealAnimation();
		}

		anyAnimating = anyFleeing;
	}

	// Update deal animations
	for (int i = 0; i < 4; i++) {
		if (fAnimations[i].active) {
			fAnimations[i].progress += kAnimationStep;

			if (fAnimations[i].progress >= 1.0f) {
				fAnimations[i].progress = 1.0f;
				fAnimations[i].active = false;

				// Show the CardView with the card
				fCardViews[i]->SetCard(fAnimations[i].card);
				if (fCardViews[i]->IsHidden())
					fCardViews[i]->Show();
			} else {
				anyAnimating = true;
			}
		}
	}

	Invalidate();

	// Stop animation timer if no cards are animating
	if (!anyAnimating && !fIsFleeing) {
		delete fAnimationRunner;
		fAnimationRunner = NULL;

		// Check if we're done dealing
		bool moreToDeal = false;
		for (int i = fNextCardToDeal; i < 4; i++) {
			if (fAnimations[i].card != NULL && !fAnimations[i].active) {
				moreToDeal = true;
				break;
			}
		}
		if (!moreToDeal) {
			fIsDealing = false;
		}
	}
}
