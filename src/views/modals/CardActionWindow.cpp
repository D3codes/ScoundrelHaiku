#include "CardActionWindow.h"
#include "models/Card.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <StringView.h>
#include <View.h>

class CardActionView : public BView {
public:
	CardActionView(BRect frame, Card* card)
		: BView(frame, "cardActionView", B_FOLLOW_ALL, B_WILL_DRAW),
		  fCard(card)
	{
		SetViewColor(kBackgroundColor);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Draw card image area
		BRect cardRect(bounds.Width() / 2 - 60, 20,
			bounds.Width() / 2 + 60, 180);

		// Draw card background
		SetHighColor(kCardBackgroundColor);
		FillRoundRect(cardRect, 10, 10);

		// Draw border
		SetHighColor(fCard->GetColor());
		SetPenSize(3);
		StrokeRoundRect(cardRect, 10, 10);
		SetPenSize(1);

		// Try to load card image
		BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
			fCard->GetImageName().String());

		if (cardImage != NULL) {
			BRect imageRect = cardImage->Bounds();
			float scale = 100.0f / imageRect.Width();
			if (120.0f / imageRect.Height() < scale)
				scale = 120.0f / imageRect.Height();

			float imageWidth = imageRect.Width() * scale;
			float imageHeight = imageRect.Height() * scale;
			float imageX = cardRect.left + (cardRect.Width() - imageWidth) / 2;
			float imageY = cardRect.top + 15;

			BRect destRect(imageX, imageY, imageX + imageWidth, imageY + imageHeight);
			DrawBitmap(cardImage, imageRect, destRect);
		}

		// Draw strength
		SetHighColor(fCard->GetColor());
		BFont font;
		font.SetSize(kTitleFontSize);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		BString strengthStr;
		strengthStr.SetToFormat("%d", fCard->Strength());
		float textWidth = StringWidth(strengthStr.String());
		DrawString(strengthStr.String(),
			BPoint(cardRect.left + (cardRect.Width() - textWidth) / 2,
				cardRect.bottom - 10));

		// Draw card type label
		SetHighColor(kTextColor);
		font.SetSize(kHeadingFontSize);
		SetFont(&font);

		const char* typeName = "";
		switch (fCard->Suit()) {
			case kSuitWeapon:
				typeName = "Weapon";
				break;
			case kSuitHealthPotion:
				typeName = "Health Potion";
				break;
			case kSuitMonster:
				typeName = "Monster";
				break;
		}

		textWidth = StringWidth(typeName);
		DrawString(typeName, BPoint((bounds.Width() - textWidth) / 2, 210));
	}

private:
	Card* fCard;
};


CardActionWindow::CardActionWindow(BWindow* parent, Card* card,
	int32 cardIndex, bool canUseWeapon)
	:
	BWindow(BRect(0, 0, 250, 320), "Card Action",
		B_MODAL_WINDOW_LOOK, B_MODAL_SUBSET_WINDOW_FEEL,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fParent(parent),
	fCard(card),
	fCardIndex(cardIndex)
{
	AddToSubset(parent);

	// Center on parent
	BRect parentFrame = parent->Frame();
	BRect frame = Frame();
	float x = parentFrame.left + (parentFrame.Width() - frame.Width()) / 2;
	float y = parentFrame.top + (parentFrame.Height() - frame.Height()) / 2;
	MoveTo(x, y);

	// Create main view
	BRect bounds = Bounds();
	CardActionView* mainView = new CardActionView(bounds, card);

	// Create buttons based on card type
	float buttonWidth = 100;
	float buttonHeight = 30;
	float centerX = bounds.Width() / 2;
	float buttonY = 230;

	fFirstButton = new BButton(
		BRect(centerX - buttonWidth - 5, buttonY,
			centerX - 5, buttonY + buttonHeight),
		"firstBtn", card->GetFirstButtonText().String(),
		new BMessage(kActionDrink)); // Will be changed based on type

	switch (card->Suit()) {
		case kSuitHealthPotion:
			fFirstButton->SetMessage(new BMessage(kActionDrink));
			fSecondButton = NULL;
			// Center the drink button
			fFirstButton->MoveTo(centerX - buttonWidth / 2, buttonY);
			fFirstButton->ResizeTo(buttonWidth, buttonHeight);
			break;
		case kSuitWeapon:
			fFirstButton->SetMessage(new BMessage(kActionEquip));
			fSecondButton = NULL;
			// Center the equip button
			fFirstButton->MoveTo(centerX - buttonWidth / 2, buttonY);
			fFirstButton->ResizeTo(buttonWidth, buttonHeight);
			break;
		case kSuitMonster:
			fFirstButton->SetMessage(new BMessage(kActionAttackUnarmed));
			fSecondButton = new BButton(
				BRect(centerX + 5, buttonY,
					centerX + buttonWidth + 5, buttonY + buttonHeight),
				"secondBtn", "Weapon", new BMessage(kActionAttackWeapon));
			fSecondButton->SetEnabled(canUseWeapon);
			break;
	}

	// Cancel button
	fCancelButton = new BButton(
		BRect(centerX - buttonWidth / 2, buttonY + 40,
			centerX + buttonWidth / 2, buttonY + 40 + buttonHeight),
		"cancelBtn", "Cancel", new BMessage(kActionCancel));

	mainView->AddChild(fFirstButton);
	if (fSecondButton != NULL)
		mainView->AddChild(fSecondButton);
	mainView->AddChild(fCancelButton);

	AddChild(mainView);
}


CardActionWindow::~CardActionWindow()
{
}


void
CardActionWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kActionDrink:
		case kActionEquip:
		case kActionAttackUnarmed:
		case kActionAttackWeapon:
		{
			// Send action to parent
			BMessage actionMsg(kMsgCardAction);
			actionMsg.AddInt32("index", fCardIndex);
			actionMsg.AddInt32("action", message->what);
			fParent->PostMessage(&actionMsg);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case kActionCancel:
			PostMessage(B_QUIT_REQUESTED);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
