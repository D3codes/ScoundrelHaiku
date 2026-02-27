#include "RoomView.h"
#include "CardView.h"
#include "models/Room.h"
#include "utils/Constants.h"

RoomView::RoomView(BRect frame)
	:
	BView(frame, "roomView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS),
	fRoom(NULL)
{
	SetViewColor(B_TRANSPARENT_COLOR);

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
	// Background drawn by view color
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
}
