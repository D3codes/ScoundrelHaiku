#include "CardActionWindow.h"
#include "models/Card.h"
#include "models/Player.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"
#include "utils/MessageCodes.h"

#include <Bitmap.h>
#include <Picture.h>
#include <StringView.h>
#include <View.h>


// Title bar with close button and title
class CardActionTitleBar : public BView {
public:
	CardActionTitleBar(BRect frame, const char* title)
		: BView(frame, "titleBar", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW),
		  fTitle(title)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Draw stone background
		BBitmap* stoneBg = ResourceLoader::Instance()->GetUIImage("stoneSlab2");
		if (stoneBg != NULL) {
			SetDrawingMode(B_OP_COPY);
			DrawBitmap(stoneBg, stoneBg->Bounds(), bounds);
		} else {
			SetHighColor(80, 80, 100);
			FillRect(bounds);
		}

		// Draw close button (X) on the left
		float buttonSize = 30;
		float buttonMargin = 5;
		BRect closeRect(buttonMargin, (bounds.Height() - buttonSize) / 2,
			buttonMargin + buttonSize, (bounds.Height() + buttonSize) / 2);

		SetHighColor(60, 60, 80);
		FillRoundRect(closeRect, 5, 5);
		SetHighColor(100, 100, 120);
		StrokeRoundRect(closeRect, 5, 5);

		// Draw X
		SetHighColor(kTextColor);
		SetPenSize(2);
		float inset = 8;
		StrokeLine(BPoint(closeRect.left + inset, closeRect.top + inset),
			BPoint(closeRect.right - inset, closeRect.bottom - inset));
		StrokeLine(BPoint(closeRect.right - inset, closeRect.top + inset),
			BPoint(closeRect.left + inset, closeRect.bottom - inset));
		SetPenSize(1);

		// Draw title centered
		BFont font;
		font.SetSize(20);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float titleWidth = StringWidth(fTitle.String());
		float titleX = (bounds.Width() - titleWidth) / 2;
		float titleY = bounds.Height() / 2 + 7;

		// Draw shadow
		SetHighColor(0, 0, 0, 180);
		DrawString(fTitle.String(), BPoint(titleX + 2, titleY + 2));

		// Draw title
		SetHighColor(kTextColor);
		DrawString(fTitle.String(), BPoint(titleX, titleY));
	}

	virtual void MouseDown(BPoint where) {
		// Check if close button was clicked
		float buttonSize = 30;
		float buttonMargin = 5;
		BRect closeRect(buttonMargin, (Bounds().Height() - buttonSize) / 2,
			buttonMargin + buttonSize, (Bounds().Height() + buttonSize) / 2);

		if (closeRect.Contains(where)) {
			Window()->PostMessage(new BMessage(kActionCancel));
		}
	}

private:
	BString fTitle;
};


class CardActionContentView : public BView {
public:
	CardActionContentView(BRect frame, Card* card)
		: BView(frame, "cardActionView", B_FOLLOW_ALL, B_WILL_DRAW),
		  fCard(card)
	{
		SetViewColor(222, 210, 190);
	}

	virtual void Draw(BRect updateRect) {
		BRect bounds = Bounds();

		// Fill background
		SetHighColor(222, 210, 190);
		FillRect(bounds);

		// Draw card area with paper background
		float cardRadius = 12;
		float cardWidth = 140;
		float cardHeight = 180;
		BRect cardRect((bounds.Width() - cardWidth) / 2, 15,
			(bounds.Width() + cardWidth) / 2, 15 + cardHeight);

		// Fill card background
		SetHighColor(kCardBackgroundColor);
		FillRoundRect(cardRect, cardRadius, cardRadius);

		// Create clipping for rounded card shape
		BPicture cardClipPicture;
		BeginPicture(&cardClipPicture);
		FillRoundRect(cardRect, cardRadius, cardRadius);
		EndPicture();

		// Draw paper texture clipped to rounded card
		BBitmap* paperBg = ResourceLoader::Instance()->GetUIImage("paper");
		if (paperBg != NULL) {
			ClipToPicture(&cardClipPicture, B_ORIGIN, false);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(paperBg, paperBg->Bounds(), cardRect);
			SetDrawingMode(B_OP_COPY);
			ClipToPicture(NULL);
		}

		// Stroke card border
		SetHighColor(180, 170, 150);
		SetPenSize(2);
		StrokeRoundRect(cardRect, cardRadius, cardRadius);
		SetPenSize(1);

		// Draw card image
		BBitmap* cardImage = ResourceLoader::Instance()->GetCardImage(
			fCard->GetImageName().String());

		if (cardImage != NULL) {
			BRect imageRect = cardImage->Bounds();
			float scale = 100.0f / imageRect.Width();
			if (105.0f / imageRect.Height() < scale)
				scale = 105.0f / imageRect.Height();

			float imageWidth = imageRect.Width() * scale;
			float imageHeight = imageRect.Height() * scale;
			float imageX = cardRect.left + (cardRect.Width() - imageWidth) / 2;
			float imageY = cardRect.top + 10;

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
		}

		// Draw icon and strength at bottom of card
		float iconSize = 28;
		BBitmap* icon = ResourceLoader::Instance()->GetGlyph(
			fCard->GetIconName().String());

		BString strengthStr;
		strengthStr.SetToFormat("%d", fCard->Strength());

		BFont font;
		font.SetSize(24);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float textWidth = StringWidth(strengthStr.String());
		float contentWidth = iconSize + 6 + textWidth;
		float startX = cardRect.left + (cardRect.Width() - contentWidth) / 2;
		float bottomY = cardRect.bottom - 6;

		if (icon != NULL) {
			BRect iconRect = icon->Bounds();
			BRect destIconRect(startX, bottomY - iconSize,
				startX + iconSize, bottomY);
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(icon, iconRect, destIconRect);
			SetDrawingMode(B_OP_COPY);
			startX += iconSize + 6;
		}

		SetHighColor(kDarkTextColor);
		DrawString(strengthStr.String(), BPoint(startX, bottomY - 5));
	}

private:
	Card* fCard;
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
		BBitmap* plankBg = ResourceLoader::Instance()->GetUIImage("plank1");
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
		font.SetSize(18);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		float textWidth = StringWidth(fLabel.String());
		float totalWidth = textWidth;

		// Add space for damage preview if showing
		float heartSize = 25;
		if (fDamagePreview >= 0) {
			totalWidth += heartSize + 25;
		}

		float startX = (bounds.Width() - totalWidth) / 2;
		float textY = bounds.Height() / 2 + 6;

		// Draw shadow
		if (fEnabled) {
			SetHighColor(0, 0, 0, 180);
			DrawString(fLabel.String(), BPoint(startX + 2, textY + 2));
		}

		// Draw text
		SetHighColor(fEnabled ? kTextColor : (rgb_color){100, 100, 100, 255});
		DrawString(fLabel.String(), BPoint(startX, textY));

		// Draw damage preview with heart icon
		if (fDamagePreview >= 0) {
			float heartX = startX + textWidth + 8;

			BBitmap* heartIcon = ResourceLoader::Instance()->GetGlyph("heart1");
			if (heartIcon != NULL) {
				BRect iconRect = heartIcon->Bounds();
				BRect destRect(heartX, (bounds.Height() - heartSize) / 2,
					heartX + heartSize, (bounds.Height() + heartSize) / 2);
				SetDrawingMode(B_OP_ALPHA);
				DrawBitmap(heartIcon, iconRect, destRect);
				SetDrawingMode(B_OP_COPY);
			}

			// Draw damage number on heart
			BString damageStr;
			damageStr.SetToFormat("-%d", fDamagePreview);
			font.SetSize(12);
			SetFont(&font);
			float damageWidth = StringWidth(damageStr.String());
			float damageX = heartX + (heartSize - damageWidth) / 2;
			float damageY = bounds.Height() / 2 + 4;

			// Draw shadow
			SetHighColor(0, 0, 0, 180);
			DrawString(damageStr.String(), BPoint(damageX + 1, damageY + 1));

			// Draw text
			SetHighColor(kTextColor);
			DrawString(damageStr.String(), BPoint(damageX, damageY));
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
	BWindow(BRect(0, 0, 220, 100), "Card Action",
		B_MODAL_WINDOW_LOOK, B_MODAL_SUBSET_WINDOW_FEEL,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fParent(parent),
	fCard(card),
	fCardIndex(cardIndex)
{
	AddToSubset(parent);

	// Calculate window size based on content
	float titleBarHeight = 40;
	float cardAreaHeight = 210;
	float buttonHeight = 36;
	float buttonSpacing = 10;
	float padding = 15;
	float windowWidth = 220;

	int numButtons = 1;
	if (card->Suit() == kSuitMonster && canUseWeapon)
		numButtons = 2;

	float buttonsHeight = numButtons * buttonHeight + (numButtons - 1) * buttonSpacing;
	float windowHeight = titleBarHeight + cardAreaHeight + buttonsHeight + padding;

	ResizeTo(windowWidth, windowHeight);

	// Center on parent
	BRect parentFrame = parent->Frame();
	BRect frame = Frame();
	float x = parentFrame.left + (parentFrame.Width() - frame.Width()) / 2;
	float y = parentFrame.top + (parentFrame.Height() - frame.Height()) / 2;
	MoveTo(x, y);

	// Determine title based on card type
	const char* title;
	switch (card->Suit()) {
		case kSuitMonster:
			title = "Attack";
			break;
		case kSuitHealthPotion:
			title = "Potion";
			break;
		case kSuitWeapon:
			title = "Weapon";
			break;
		default:
			title = "Card";
			break;
	}

	// Create title bar
	BRect titleRect(0, 0, windowWidth, titleBarHeight);
	CardActionTitleBar* titleBar = new CardActionTitleBar(titleRect, title);
	AddChild(titleBar);

	// Create content view (card display)
	BRect contentRect(0, titleBarHeight, windowWidth, titleBarHeight + cardAreaHeight);
	CardActionContentView* contentView = new CardActionContentView(contentRect, card);
	AddChild(contentView);

	// Create buttons container
	float buttonY = titleBarHeight + cardAreaHeight;
	float buttonWidth = windowWidth - 30;
	float centerX = windowWidth / 2;

	// Create background view for buttons
	BRect buttonAreaRect(0, buttonY, windowWidth, windowHeight);
	BView* buttonArea = new BView(buttonAreaRect, "buttonArea", B_FOLLOW_NONE, B_WILL_DRAW);
	buttonArea->SetViewColor(222, 210, 190);

	// Create action button(s)
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
			firstDamage = card->Strength();
			break;
	}

	float btnY = 0;
	fFirstButton = new PlankButton(
		BRect(centerX - buttonWidth / 2, btnY,
			centerX + buttonWidth / 2, btnY + buttonHeight),
		"firstBtn", card->GetFirstButtonText().String(),
		firstMsg, firstMsg->what);

	if (firstDamage >= 0) {
		((PlankButton*)fFirstButton)->SetDamagePreview(firstDamage);
	}

	buttonArea->AddChild(fFirstButton);
	btnY += buttonHeight + buttonSpacing;

	// Create second action button (only for monsters with weapon)
	fSecondButton = NULL;
	if (card->Suit() == kSuitMonster && canUseWeapon && player != NULL) {
		BMessage* secondMsg = new BMessage(kActionAttackWeapon);
		int weaponDamage = card->Strength() - player->Weapon();
		if (weaponDamage < 0) weaponDamage = 0;

		fSecondButton = new PlankButton(
			BRect(centerX - buttonWidth / 2, btnY,
				centerX + buttonWidth / 2, btnY + buttonHeight),
			"secondBtn", card->GetSecondButtonText().String(),
			secondMsg, kActionAttackWeapon);

		((PlankButton*)fSecondButton)->SetDamagePreview(weaponDamage);
		buttonArea->AddChild(fSecondButton);
	}

	fCancelButton = NULL; // Cancel is now in title bar

	AddChild(buttonArea);
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
