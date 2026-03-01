#include "CardView.h"
#include "models/Card.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Picture.h>
#include <String.h>
#include <Window.h>

CardView::CardView(BRect frame, int32 index)
	:
	BView(frame, "cardView", B_FOLLOW_NONE, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fIndex(index),
	fCard(NULL),
	fBackgroundIndex(1),
	fIsAnimating(false)
{
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
}


void
CardView::SetCard(Card* card)
{
	fCard = card;
	fIsAnimating = false;
	Invalidate();
}


void
CardView::ClearCard()
{
	fCard = NULL;
	// Don't reset fIsAnimating - let SetAnimating() control it
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

	SetHighColor(kBackgroundColor);
	FillRect(bounds);

	if (Parent() == NULL || Parent()->Parent() == NULL)
		return;

	BView* roomView = Parent();
	BView* gameBoard = roomView->Parent();

	BString bgName;
	bgName.SetToFormat("dungeon%d", fBackgroundIndex);
	BBitmap* background = ResourceLoader::Instance()->GetBackground(bgName.String());

	if (background != NULL) {
		BRect gameBoardBounds = gameBoard->Bounds();
		BRect srcRect = background->Bounds();

		float scaleX = srcRect.Width() / gameBoardBounds.Width();
		float scaleY = srcRect.Height() / gameBoardBounds.Height();

		BPoint roomPos = roomView->Frame().LeftTop();
		BPoint cardPos = Frame().LeftTop();
		float totalX = roomPos.x + cardPos.x;
		float totalY = roomPos.y + cardPos.y;

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

	SetHighColor(0, 0, 0, 100);
	BRect shadowRect = bounds;
	shadowRect.OffsetBy(4, 4);
	FillRoundRect(shadowRect, radius, radius);

	SetHighColor(kCardBackgroundColor);
	FillRoundRect(bounds, radius, radius);

	BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
	if (paperBg != NULL) {
		BRect insetBounds = bounds.InsetByCopy(2, 2);
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(paperBg, paperBg->Bounds(), insetBounds);
		SetDrawingMode(B_OP_COPY);
	}

	SetHighColor(180, 170, 150);
	StrokeRoundRect(bounds, radius, radius);

	BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
		fCard->GetImageName().String());

	float bottomAreaHeight = 38;
	float imageAreaHeight = bounds.Height() - bottomAreaHeight;

	if (cardImage != NULL) {
		BRect imageRect = cardImage->Bounds();
		float padding = 8;
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

		float imageRadius = 10;

		// Create a clipping picture for rounded corners
		BPicture clipPicture;
		BeginPicture(&clipPicture);
		FillRoundRect(destRect, imageRadius, imageRadius);
		EndPicture();

		// Clip to rounded rect and draw image
		ClipToPicture(&clipPicture, B_ORIGIN, false);
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(cardImage, imageRect, destRect);
		SetDrawingMode(B_OP_COPY);

		// Remove clipping
		ClipToPicture(NULL);

		// Stroke the rounded border
		SetHighColor(160, 150, 130);
		StrokeRoundRect(destRect, imageRadius, imageRadius);
	} else {
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

	float iconSize = 32;
	BBitmap* suitIcon = ResourceLoader::Instance()->GetGlyph(
		fCard->GetIconName().String());

	BFont font;
	font.SetSize(28);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	BString strengthStr;
	strengthStr.SetToFormat("%d", fCard->Strength());
	float textWidth = StringWidth(strengthStr.String());

	float spacing = 6;
	float totalWidth = iconSize + spacing + textWidth;
	float startX = (bounds.Width() - totalWidth) / 2;
	float bottomY = bounds.Height() - 4;

	if (suitIcon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(suitIcon, BRect(0, 0, suitIcon->Bounds().Width(),
			suitIcon->Bounds().Height()),
			BRect(startX, bottomY - iconSize, startX + iconSize, bottomY));
		SetDrawingMode(B_OP_COPY);
	}

	SetHighColor(0, 0, 0);
	DrawString(strengthStr.String(),
		BPoint(startX + iconSize + spacing, bottomY - 5));
}


void
CardView::MouseDown(BPoint where)
{
	if (fCard == NULL || fIsAnimating)
		return;

	BMessage msg(kMsgCardSelected);
	msg.AddInt32("index", fIndex);
	Window()->PostMessage(&msg);
}
