#include "CardView.h"
#include "models/Card.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Window.h>

CardView::CardView(BRect frame, int32 index)
	:
	BView(frame, "cardView", B_FOLLOW_NONE, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fIndex(index),
	fCard(NULL)
{
	// Transparent to allow dungeon background to show around cards
	SetViewColor(B_TRANSPARENT_COLOR);
}


CardView::~CardView()
{
	// Card is owned by Room, don't delete
}


void
CardView::SetCard(Card* card)
{
	fCard = card;
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
	} else {
		DrawCard();
	}
}


void
CardView::DrawEmptySlot()
{
	BRect bounds = Bounds();

	// Fill entire area with solid dark color first (no transparency)
	SetDrawingMode(B_OP_COPY);
	SetHighColor(35, 35, 45);
	FillRect(bounds);

	// Draw dark rounded rectangle for empty slot
	SetHighColor(45, 45, 55);
	FillRoundRect(bounds, 8, 8);

	// Draw border
	SetHighColor(60, 60, 70);
	StrokeRoundRect(bounds, 8, 8);
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
CardView::MouseDown(BPoint where)
{
	if (fCard == NULL)
		return;

	// Send message to parent
	BMessage msg(kMsgCardSelected);
	msg.AddInt32("index", fIndex);
	Window()->PostMessage(&msg);
}
