#include "CardActionWindow.h"
#include "models/Card.h"
#include "models/Player.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <StringView.h>
#include <View.h>

class CardActionView : public BView {
public:
	CardActionView(BRect frame, Card* card, Player* player, bool canUseWeapon)
		: BView(frame, "cardActionView", B_FOLLOW_ALL, B_WILL_DRAW),
		  fCard(card),
		  fPlayer(player),
		  fCanUseWeapon(canUseWeapon)
	{
		// Use a dark solid color as base
		SetViewColor(40, 40, 50);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Draw dark gradient background
		for (float y = bounds.top; y <= bounds.bottom; y++) {
			float ratio = (y - bounds.top) / bounds.Height();
			int gray = 35 + (int)(ratio * 15);
			SetHighColor(gray, gray, gray + 10);
			StrokeLine(BPoint(bounds.left, y), BPoint(bounds.right, y));
		}

		// Draw card area with paper background
		float cardRadius = 12;
		BRect cardRect(bounds.Width() / 2 - 80, 30,
			bounds.Width() / 2 + 80, 230);

		// Draw card background
		SetHighColor(kCardBackgroundColor);
		FillRoundRect(cardRect, cardRadius, cardRadius);

		// Draw paper texture on card (inset for rounded corners)
		BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
		if (paperBg != NULL) {
			BRect insetCardRect = cardRect.InsetByCopy(2, 2);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(paperBg, paperBg->Bounds(), insetCardRect);
			SetDrawingMode(B_OP_COPY);
		}

		// Draw card border
		SetHighColor(180, 170, 150);
		SetPenSize(2);
		StrokeRoundRect(cardRect, cardRadius, cardRadius);
		SetPenSize(1);

		// Draw card image with rounded corners
		BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
			fCard->GetImageName().String());

		if (cardImage != NULL) {
			BRect imageRect = cardImage->Bounds();
			float scale = 120.0f / imageRect.Width();
			if (140.0f / imageRect.Height() < scale)
				scale = 140.0f / imageRect.Height();

			float imageWidth = imageRect.Width() * scale;
			float imageHeight = imageRect.Height() * scale;
			float imageX = cardRect.left + (cardRect.Width() - imageWidth) / 2;
			float imageY = cardRect.top + 15;

			BRect destRect(imageX, imageY, imageX + imageWidth, imageY + imageHeight);
			float imageRadius = 8;

			// Draw rounded background for image
			SetHighColor(kCardBackgroundColor);
			FillRoundRect(destRect, imageRadius, imageRadius);

			// Draw image inset for rounded corners
			BRect insetDestRect = destRect.InsetByCopy(2, 2);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(cardImage, imageRect, insetDestRect);
			SetDrawingMode(B_OP_COPY);

			// Draw rounded border around image
			SetHighColor(160, 150, 130);
			StrokeRoundRect(destRect, imageRadius, imageRadius);
		}

		// Draw icon and strength at bottom of card
		float iconSize = 25;
		BBitmap* icon = ResourceLoader::Instance()->GetGlyph(
			fCard->GetIconName().String());

		BString strengthStr;
		strengthStr.SetToFormat("%d", fCard->Strength());

		BFont font;
		font.SetSize(24);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float textWidth = StringWidth(strengthStr.String());
		float contentWidth = iconSize + 8 + textWidth;
		float startX = cardRect.left + (cardRect.Width() - contentWidth) / 2;
		float bottomY = cardRect.bottom - 15;

		if (icon != NULL) {
			BRect iconRect = icon->Bounds();
			float iconScale = iconSize / iconRect.Width();
			BRect destRect(startX, bottomY - iconSize,
				startX + iconSize, bottomY);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(icon, iconRect, destRect);
			SetDrawingMode(B_OP_COPY);
			startX += iconSize + 8;
		}

		SetHighColor(kDarkTextColor);
		DrawString(strengthStr.String(), BPoint(startX, bottomY - 5));
	}

private:
	Card* fCard;
	Player* fPlayer;
	bool fCanUseWeapon;
};


// Custom button view that draws plank background
class PlankButton : public BView {
public:
	PlankButton(BRect frame, const char* name, const char* label,
		BMessage* message, uint32 action)
		: BView(frame, name, B_FOLLOW_NONE, B_WILL_DRAW),
		  fLabel(label),
		  fMessage(message),
		  fAction(action),
		  fEnabled(true),
		  fDamagePreview(-1)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual ~PlankButton() {
		delete fMessage;
	}

	void SetEnabled(bool enabled) {
		fEnabled = enabled;
		Invalidate();
	}

	void SetDamagePreview(int damage) {
		fDamagePreview = damage;
		Invalidate();
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();
		float radius = 8;

		// Draw solid rounded background first (shows in corners)
		SetHighColor(100, 70, 50);
		FillRoundRect(bounds, radius, radius);

		// Draw plank background inset to show rounded corners
		BBitmap* plankBg = ResourceLoader::Instance()->GetUIImage("woodButton");
		if (plankBg != NULL) {
			BRect insetBounds = bounds.InsetByCopy(2, 2);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(plankBg, plankBg->Bounds(), insetBounds);
			SetDrawingMode(B_OP_COPY);
		}

		// Draw rounded border
		SetHighColor(120, 90, 60);
		StrokeRoundRect(bounds, radius, radius);

		if (!fEnabled) {
			SetHighColor(0, 0, 0, 100);
			FillRoundRect(bounds, radius, radius);
		}

		// Draw button text
		BFont font;
		font.SetSize(20);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		SetHighColor(fEnabled ? kTextColor : (rgb_color){100, 100, 100, 255});

		float textWidth = StringWidth(fLabel.String());
		float totalWidth = textWidth;

		// Add space for damage preview if showing
		float heartSize = 30;
		if (fDamagePreview >= 0) {
			totalWidth += heartSize + 30; // heart + damage text
		}

		float startX = (bounds.Width() - totalWidth) / 2;
		float textY = bounds.Height() / 2 + 7;

		DrawString(fLabel.String(), BPoint(startX, textY));

		// Draw damage preview with heart icon
		if (fDamagePreview >= 0) {
			float heartX = startX + textWidth + 10;

			BBitmap* heartIcon = ResourceLoader::Instance()->GetGlyph("heart1");
			if (heartIcon != NULL) {
				BRect iconRect = heartIcon->Bounds();
				float scale = heartSize / iconRect.Width();
				BRect destRect(heartX, (bounds.Height() - heartSize) / 2,
					heartX + heartSize, (bounds.Height() + heartSize) / 2);
				SetDrawingMode(B_OP_ALPHA);
				DrawBitmap(heartIcon, iconRect, destRect);
				SetDrawingMode(B_OP_COPY);
			}

			// Draw damage number on heart
			BString damageStr;
			damageStr.SetToFormat("-%d", fDamagePreview);
			font.SetSize(14);
			SetFont(&font);
			SetHighColor(kTextColor);
			float damageWidth = StringWidth(damageStr.String());
			DrawString(damageStr.String(),
				BPoint(heartX + (heartSize - damageWidth) / 2,
					bounds.Height() / 2 + 5));
		}
	}

	virtual void MouseDown(BPoint where) {
		if (!fEnabled)
			return;

		Window()->PostMessage(fMessage);
	}

private:
	BString fLabel;
	BMessage* fMessage;
	uint32 fAction;
	bool fEnabled;
	int fDamagePreview;
};


CardActionWindow::CardActionWindow(BWindow* parent, Card* card,
	int32 cardIndex, bool canUseWeapon, Player* player)
	:
	BWindow(BRect(0, 0, 280, 440), "Card Action",
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
	CardActionView* mainView = new CardActionView(bounds, card, player, canUseWeapon);

	float buttonWidth = 200;
	float buttonHeight = 40;
	float buttonSpacing = 20;
	float centerX = bounds.Width() / 2;
	float buttonY = 250;

	// Create first action button
	BMessage* firstMsg = new BMessage(kActionDrink);
	int firstDamage = -1;

	switch (card->Suit()) {
		case kSuitHealthPotion:
			firstMsg->what = kActionDrink;
			break;
		case kSuitWeapon:
			firstMsg->what = kActionEquip;
			break;
		case kSuitMonster:
			firstMsg->what = kActionAttackUnarmed;
			firstDamage = card->Strength(); // Full damage when unarmed
			break;
	}

	fFirstButton = new PlankButton(
		BRect(centerX - buttonWidth / 2, buttonY,
			centerX + buttonWidth / 2, buttonY + buttonHeight),
		"firstBtn", card->GetFirstButtonText().String(),
		firstMsg, firstMsg->what);

	if (firstDamage >= 0) {
		((PlankButton*)fFirstButton)->SetDamagePreview(firstDamage);
	}

	mainView->AddChild(fFirstButton);
	buttonY += buttonHeight + buttonSpacing;

	// Create second action button (only for monsters with weapon)
	fSecondButton = NULL;
	if (card->Suit() == kSuitMonster && canUseWeapon && player != NULL) {
		BMessage* secondMsg = new BMessage(kActionAttackWeapon);
		int weaponDamage = card->Strength() - player->Weapon();
		if (weaponDamage < 0) weaponDamage = 0;

		fSecondButton = new PlankButton(
			BRect(centerX - buttonWidth / 2, buttonY,
				centerX + buttonWidth / 2, buttonY + buttonHeight),
			"secondBtn", card->GetSecondButtonText().String(),
			secondMsg, kActionAttackWeapon);

		((PlankButton*)fSecondButton)->SetDamagePreview(weaponDamage);
		mainView->AddChild(fSecondButton);
		buttonY += buttonHeight + buttonSpacing;
	}

	// Cancel button
	fCancelButton = new PlankButton(
		BRect(centerX - buttonWidth / 2, buttonY,
			centerX + buttonWidth / 2, buttonY + buttonHeight),
		"cancelBtn", "Cancel", new BMessage(kActionCancel), kActionCancel);

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
