#include "CardView.h"
#include "models/Card.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <String.h>
#include <Window.h>

CardView::CardView(BRect frame, int32 index)
	:
	BView(frame, "cardView", B_FOLLOW_NONE, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fIndex(index),
	fCard(NULL),
	fBackgroundIndex(1),
	fIsAnimating(false),
	fAnimationProgress(0.0f),
	fStartPosition(0, 0),
	fEndPosition(frame.LeftTop()),
	fStartScale(0.3f),
	fEndScale(1.0f),
	fDeckPosition(0, 0)
{
	// Transparent to allow dungeon background to show around cards
	SetViewColor(B_TRANSPARENT_COLOR);
}


void
CardView::SetBackgroundIndex(int index)
{
	fBackgroundIndex = index;
	if (fCard == NULL)
		Invalidate();
}


CardView::~CardView()
{
	// Card is owned by Room, don't delete
}


void
CardView::SetCard(Card* card)
{
	fCard = card;
	fIsAnimating = false;
	Invalidate();
}


void
CardView::SetCardWithAnimation(Card* card, BPoint startPos, float startScale)
{
	fCard = card;
	fIsAnimating = true;
	fAnimationProgress = 0.0f;
	fStartPosition = startPos;
	fEndPosition = Frame().LeftTop();
	fStartScale = startScale;
	fEndScale = 1.0f;
	Invalidate();
}


void
CardView::UpdateAnimation()
{
	if (!fIsAnimating)
		return;

	// Advance animation with easing (ease-out cubic)
	fAnimationProgress += 0.08f;
	if (fAnimationProgress >= 1.0f) {
		fAnimationProgress = 1.0f;
		fIsAnimating = false;
	}

	Invalidate();
}


void
CardView::ClearCard()
{
	fCard = NULL;
	Invalidate();
}


void
CardView::Draw(BRect updateRect)
{
	if (fCard == NULL) {
		DrawEmptySlot();
	} else if (fIsAnimating) {
		DrawAnimatingCard();
	} else {
		DrawCard();
	}
}


void
CardView::DrawEmptySlot()
{
	// Draw the dungeon background portion for this card's area
	// This is needed because parent views are clipped and don't draw under children
	BRect bounds = Bounds();

	// Always fill with background color first to prevent artifacts
	SetHighColor(kBackgroundColor);
	FillRect(bounds);

	if (Parent() == NULL || Parent()->Parent() == NULL)
		return;

	// Get background index from parent RoomView
	BView* roomView = Parent();
	BView* gameBoard = roomView->Parent();

	// Get the background bitmap
	BString bgName;
	bgName.SetToFormat("dungeon%d", fBackgroundIndex);
	BBitmap* background = ResourceLoader::Instance()->GetBackground(bgName.String());

	if (background != NULL) {
		// Calculate our position relative to the game board
		BRect gameBoardBounds = gameBoard->Bounds();
		BRect srcRect = background->Bounds();

		// Scale factors
		float scaleX = srcRect.Width() / gameBoardBounds.Width();
		float scaleY = srcRect.Height() / gameBoardBounds.Height();

		// Our position in the game board
		BPoint roomPos = roomView->Frame().LeftTop();
		BPoint cardPos = Frame().LeftTop();
		float totalX = roomPos.x + cardPos.x;
		float totalY = roomPos.y + cardPos.y;

		// Calculate source rect for this card's area
		BRect ourSrcRect(
			totalX * scaleX,
			totalY * scaleY,
			(totalX + bounds.Width()) * scaleX,
			(totalY + bounds.Height()) * scaleY
		);

		SetDrawingMode(B_OP_COPY);
		DrawBitmap(background, ourSrcRect, bounds);
	}
}


void
CardView::DrawCard()
{
	BRect bounds = Bounds();
	float radius = 12;

	// Draw shadow first
	SetHighColor(0, 0, 0, 100);
	BRect shadowRect = bounds;
	shadowRect.OffsetBy(4, 4);
	FillRoundRect(shadowRect, radius, radius);

	// Always draw solid card background first
	SetHighColor(kCardBackgroundColor);
	FillRoundRect(bounds, radius, radius);

	// Draw paper texture on top if available (inset to show rounded corners)
	BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
	if (paperBg != NULL) {
		BRect insetBounds = bounds.InsetByCopy(2, 2);
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(paperBg, paperBg->Bounds(), insetBounds);
		SetDrawingMode(B_OP_COPY);
	}

	// Draw rounded border
	SetHighColor(180, 170, 150);
	StrokeRoundRect(bounds, radius, radius);

	// Try to load card image
	BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
		fCard->GetImageName().String());

	float bottomAreaHeight = 45;
	float imageAreaHeight = bounds.Height() - bottomAreaHeight;

	if (cardImage != NULL) {
		// Draw card image in upper portion with rounded corners
		BRect imageRect = cardImage->Bounds();
		float padding = 10;
		float availWidth = bounds.Width() - padding * 2;
		float availHeight = imageAreaHeight - padding * 2;

		float scale = availWidth / imageRect.Width();
		if (availHeight / imageRect.Height() < scale)
			scale = availHeight / imageRect.Height();

		float imageWidth = imageRect.Width() * scale;
		float imageHeight = imageRect.Height() * scale;
		float imageX = (bounds.Width() - imageWidth) / 2;
		float imageY = padding;

		BRect destRect(imageX, imageY, imageX + imageWidth, imageY + imageHeight);

		// Draw rounded background for image area
		float imageRadius = 8;
		SetHighColor(kCardBackgroundColor);
		FillRoundRect(destRect, imageRadius, imageRadius);

		// Draw image inset to show rounded corners
		BRect insetDestRect = destRect.InsetByCopy(2, 2);
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(cardImage, imageRect, insetDestRect);
		SetDrawingMode(B_OP_COPY);

		// Draw rounded border around image
		SetHighColor(160, 150, 130);
		StrokeRoundRect(destRect, imageRadius, imageRadius);
	} else {
		// Draw placeholder icon
		BBitmap* icon = ResourceLoader::Instance()->GetGlyph(
			fCard->GetIconName().String());

		if (icon != NULL) {
			BRect iconRect = icon->Bounds();
			float iconX = (bounds.Width() - iconRect.Width()) / 2;
			float iconY = (imageAreaHeight - iconRect.Height()) / 2;
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(icon, BPoint(iconX, iconY));
			SetDrawingMode(B_OP_COPY);
		}
	}

	// Draw icon + strength number at bottom (iOS style)
	float iconSize = 24;
	BBitmap* suitIcon = ResourceLoader::Instance()->GetGlyph(
		fCard->GetIconName().String());

	BFont font;
	font.SetSize(24);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	BString strengthStr;
	strengthStr.SetToFormat("%d", fCard->Strength());
	float textWidth = StringWidth(strengthStr.String());

	// Calculate total width of icon + spacing + text
	float spacing = 8;
	float totalWidth = iconSize + spacing + textWidth;
	float startX = (bounds.Width() - totalWidth) / 2;
	float bottomY = bounds.Height() - 12;

	// Draw icon
	if (suitIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(suitIcon, BRect(0, 0, suitIcon->Bounds().Width(),
			suitIcon->Bounds().Height()),
			BRect(startX, bottomY - iconSize, startX + iconSize, bottomY));
		SetDrawingMode(B_OP_COPY);
	}

	// Draw strength number
	SetHighColor(0, 0, 0);
	DrawString(strengthStr.String(),
		BPoint(startX + iconSize + spacing, bottomY - 3));
}


void
CardView::DrawAnimatingCard()
{
	// First draw the empty slot background
	DrawEmptySlot();

	if (fCard == NULL)
		return;

	BRect bounds = Bounds();

	// Apply ease-out cubic easing
	float t = fAnimationProgress;
	float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

	// Interpolate scale
	float scale = fStartScale + (fEndScale - fStartScale) * eased;

	// Calculate scaled card size
	float cardWidth = bounds.Width() * scale;
	float cardHeight = bounds.Height() * scale;

	// Interpolate position (convert from parent coordinates to our local coordinates)
	float startX = fStartPosition.x - Frame().left;
	float startY = fStartPosition.y - Frame().top;
	float endX = (bounds.Width() - cardWidth) / 2;
	float endY = (bounds.Height() - cardHeight) / 2;

	float currentX = startX + (endX - startX) * eased;
	float currentY = startY + (endY - startY) * eased;

	// Create the animated card rect
	BRect cardRect(currentX, currentY, currentX + cardWidth, currentY + cardHeight);
	float radius = 12 * scale;

	// Draw shadow
	SetHighColor(0, 0, 0, (uint8)(100 * eased));
	BRect shadowRect = cardRect;
	shadowRect.OffsetBy(4 * scale, 4 * scale);
	FillRoundRect(shadowRect, radius, radius);

	// Draw card background
	SetHighColor(kCardBackgroundColor);
	FillRoundRect(cardRect, radius, radius);

	// Draw paper texture
	BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
	if (paperBg != NULL) {
		BRect insetRect = cardRect.InsetByCopy(2 * scale, 2 * scale);
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(paperBg, paperBg->Bounds(), insetRect);
		SetDrawingMode(B_OP_COPY);
	}

	// Draw border
	SetHighColor(180, 170, 150);
	StrokeRoundRect(cardRect, radius, radius);

	// Draw card image
	BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
		fCard->GetImageName().String());

	float bottomAreaHeight = 45 * scale;
	float imageAreaHeight = cardHeight - bottomAreaHeight;

	if (cardImage != NULL) {
		BRect imageRect = cardImage->Bounds();
		float padding = 10 * scale;
		float availWidth = cardWidth - padding * 2;
		float availHeight = imageAreaHeight - padding * 2;

		float imgScale = availWidth / imageRect.Width();
		if (availHeight / imageRect.Height() < imgScale)
			imgScale = availHeight / imageRect.Height();

		float imageWidth = imageRect.Width() * imgScale;
		float imageHeight = imageRect.Height() * imgScale;
		float imageX = cardRect.left + (cardWidth - imageWidth) / 2;
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
	}

	// Draw icon and strength at bottom
	float iconSize = 24 * scale;
	BBitmap* suitIcon = ResourceLoader::Instance()->GetGlyph(
		fCard->GetIconName().String());

	BFont font;
	font.SetSize(24 * scale);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	BString strengthStr;
	strengthStr.SetToFormat("%d", fCard->Strength());
	float textWidth = StringWidth(strengthStr.String());

	float spacing = 8 * scale;
	float totalWidth = iconSize + spacing + textWidth;
	float startIconX = cardRect.left + (cardWidth - totalWidth) / 2;
	float bottomY = cardRect.bottom - 12 * scale;

	if (suitIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(suitIcon, suitIcon->Bounds(),
			BRect(startIconX, bottomY - iconSize, startIconX + iconSize, bottomY));
		SetDrawingMode(B_OP_COPY);
	}

	SetHighColor(0, 0, 0);
	DrawString(strengthStr.String(),
		BPoint(startIconX + iconSize + spacing, bottomY - 3 * scale));
}


void
CardView::MouseDown(BPoint where)
{
	// Don't allow selection during animation
	if (fCard == NULL || fIsAnimating)
		return;

	// Send message to parent
	BMessage msg(kMsgCardSelected);
	msg.AddInt32("index", fIndex);
	Window()->PostMessage(&msg);
}
