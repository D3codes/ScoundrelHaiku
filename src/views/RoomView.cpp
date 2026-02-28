#include "RoomView.h"
#include "CardView.h"
#include "models/Room.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"

#include <Bitmap.h>
#include <String.h>
#include <Window.h>

RoomView::RoomView(BRect frame)
	:
	BView(frame, "roomView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE),
	fRoom(NULL),
	fBackgroundIndex(1)
{
	// Use solid color as fallback
	SetViewColor(kBackgroundColor);

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
	// Child views are deleted by BView
}


void
RoomView::AttachedToWindow()
{
	BView::AttachedToWindow();
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
