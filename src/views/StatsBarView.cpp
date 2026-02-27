#include "StatsBarView.h"
#include "models/Player.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"

#include <Bitmap.h>

StatsBarView::StatsBarView(BRect frame)
	:
	BView(frame, "statsBarView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM,
		B_WILL_DRAW),
	fPlayer(NULL)
{
	SetViewColor(kBackgroundColor);
}


StatsBarView::~StatsBarView()
{
}


void
StatsBarView::SetPlayer(Player* player)
{
	fPlayer = player;
	Refresh();
}


void
StatsBarView::Refresh()
{
	Invalidate();
}


void
StatsBarView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();
	float padding = 20;
	float barHeight = 25;
	float barWidth = bounds.Width() - padding * 2;
	float iconSize = 24;

	// Draw health bar
	BRect healthBarRect(padding + iconSize + 10, 10,
		padding + iconSize + 10 + barWidth - iconSize - 10, 10 + barHeight);
	DrawHealthBar(healthBarRect);

	// Draw heart icon
	BBitmap* heartIcon = ResourceLoader::Instance()->GetGlyph("heart1");
	if (heartIcon != NULL) {
		DrawBitmap(heartIcon, BPoint(padding, 10));
	} else {
		SetHighColor(kHealthBarColor);
		FillEllipse(BRect(padding, 10, padding + iconSize, 10 + iconSize));
	}

	// Draw weapon bar
	BRect weaponBarRect(padding + iconSize + 10, 45,
		padding + iconSize + 10 + barWidth - iconSize - 10, 45 + barHeight);
	DrawWeaponBar(weaponBarRect);

	// Draw sword icon
	BBitmap* swordIcon = ResourceLoader::Instance()->GetGlyph("sword1");
	if (swordIcon != NULL) {
		DrawBitmap(swordIcon, BPoint(padding, 45));
	} else {
		SetHighColor(kWeaponBarColor);
		FillRect(BRect(padding + 8, 45, padding + 16, 45 + iconSize));
	}
}


void
StatsBarView::DrawHealthBar(BRect barRect)
{
	// Draw background
	SetHighColor(60, 30, 30);
	FillRoundRect(barRect, 5, 5);

	// Draw filled portion
	if (fPlayer != NULL && fPlayer->Health() > 0) {
		float fillRatio = (float)fPlayer->Health() / kMaxHealth;
		BRect fillRect = barRect;
		fillRect.right = fillRect.left + (barRect.Width() * fillRatio);

		SetHighColor(kHealthBarColor);
		FillRoundRect(fillRect, 5, 5);
	}

	// Draw border
	SetHighColor(100, 50, 50);
	StrokeRoundRect(barRect, 5, 5);

	// Draw health text
	if (fPlayer != NULL) {
		SetHighColor(kTextColor);
		BFont font;
		font.SetSize(kBodyFontSize);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);

		BString healthStr;
		healthStr.SetToFormat("%d / %d", fPlayer->Health(), kMaxHealth);
		float textWidth = StringWidth(healthStr.String());
		float textX = barRect.left + (barRect.Width() - textWidth) / 2;
		float textY = barRect.top + barRect.Height() / 2 + 5;
		DrawString(healthStr.String(), BPoint(textX, textY));
	}
}


void
StatsBarView::DrawWeaponBar(BRect barRect)
{
	// Draw background
	SetHighColor(30, 30, 60);
	FillRoundRect(barRect, 5, 5);

	// Draw filled portion based on weapon strength
	if (fPlayer != NULL && fPlayer->HasWeapon()) {
		float fillRatio = (float)fPlayer->Weapon() / kMaxWeaponStrength;
		BRect fillRect = barRect;
		fillRect.right = fillRect.left + (barRect.Width() * fillRatio);

		SetHighColor(kWeaponBarColor);
		FillRoundRect(fillRect, 5, 5);

		// Draw strongest monster indicator
		int strongestMonster = fPlayer->StrongestMonsterCanAttack();
		if (strongestMonster > 0) {
			// Draw a marker showing attack limit
			float markerX = barRect.left +
				(barRect.Width() * (float)strongestMonster / kMaxMonsterStrength);
			SetHighColor(200, 200, 100);
			StrokeLine(BPoint(markerX, barRect.top),
				BPoint(markerX, barRect.bottom));
		}
	}

	// Draw border
	SetHighColor(50, 50, 100);
	StrokeRoundRect(barRect, 5, 5);

	// Draw weapon text
	SetHighColor(kTextColor);
	BFont font;
	font.SetSize(kBodyFontSize);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);

	BString weaponStr;
	if (fPlayer != NULL && fPlayer->HasWeapon()) {
		int strongest = fPlayer->StrongestMonsterCanAttack();
		if (strongest > 0)
			weaponStr.SetToFormat("Str: %d (max: %d)", fPlayer->Weapon(), strongest);
		else
			weaponStr.SetToFormat("Str: %d", fPlayer->Weapon());
	} else {
		weaponStr = "Unarmed";
	}

	float textWidth = StringWidth(weaponStr.String());
	float textX = barRect.left + (barRect.Width() - textWidth) / 2;
	float textY = barRect.top + barRect.Height() / 2 + 5;
	DrawString(weaponStr.String(), BPoint(textX, textY));
}
