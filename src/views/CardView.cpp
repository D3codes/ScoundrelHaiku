#include "CardView.h"
#include "models/Card.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Window.h>

CardView::CardView(BRect frame, int32 index)
	:
	BView(frame, "cardView", B_FOLLOW_NONE, B_WILL_DRAW),
	fIndex(index),
	fCard(NULL)
{
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

	// Draw dark rounded rectangle for empty slot
	SetHighColor(60, 60, 70);
	FillRoundRect(bounds, 8, 8);

	// Draw border
	SetHighColor(80, 80, 90);
	StrokeRoundRect(bounds, 8, 8);
}


void
CardView::DrawCard()
{
	BRect bounds = Bounds();

	// Draw card background (paper color)
	SetHighColor(kCardBackgroundColor);
	FillRoundRect(bounds, 8, 8);

	// Draw card border with suit color
	rgb_color cardColor = fCard->GetColor();
	SetHighColor(cardColor);
	SetPenSize(2);
	StrokeRoundRect(bounds, 8, 8);
	SetPenSize(1);

	// Try to load card image
	BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
		fCard->GetImageName().String());

	if (cardImage != NULL) {
		// Draw card image centered
		BRect imageRect = cardImage->Bounds();
		float scale = (bounds.Width() - 20) / imageRect.Width();
		if ((bounds.Height() - 40) / imageRect.Height() < scale)
			scale = (bounds.Height() - 40) / imageRect.Height();

		float imageWidth = imageRect.Width() * scale;
		float imageHeight = imageRect.Height() * scale;
		float imageX = (bounds.Width() - imageWidth) / 2;
		float imageY = 20;

		BRect destRect(imageX, imageY, imageX + imageWidth, imageY + imageHeight);
		DrawBitmap(cardImage, imageRect, destRect);
	} else {
		// Draw placeholder icon
		BBitmap* icon = ResourceLoader::Instance()->GetGlyph(
			fCard->GetIconName().String());

		if (icon != NULL) {
			BRect iconRect = icon->Bounds();
			float iconX = (bounds.Width() - iconRect.Width()) / 2;
			float iconY = (bounds.Height() - iconRect.Height()) / 2 - 10;
			DrawBitmap(icon, BPoint(iconX, iconY));
		} else {
			// Draw text placeholder if no icon
			SetHighColor(cardColor);
			BFont font;
			font.SetSize(kHeadingFontSize);
			SetFont(&font);

			const char* suitName = "";
			switch (fCard->Suit()) {
				case kSuitWeapon:
					suitName = "Weapon";
					break;
				case kSuitHealthPotion:
					suitName = "Potion";
					break;
				case kSuitMonster:
					suitName = "Monster";
					break;
			}

			float textWidth = StringWidth(suitName);
			DrawString(suitName, BPoint((bounds.Width() - textWidth) / 2,
				bounds.Height() / 2));
		}
	}

	// Draw strength number at bottom
	SetHighColor(cardColor);
	BFont font;
	font.SetSize(kTitleFontSize);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	BString strengthStr;
	strengthStr.SetToFormat("%d", fCard->Strength());
	float textWidth = StringWidth(strengthStr.String());
	DrawString(strengthStr.String(),
		BPoint((bounds.Width() - textWidth) / 2, bounds.Height() - 12));
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
