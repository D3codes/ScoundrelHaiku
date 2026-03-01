#include "StatsBarView.h"
#include "models/Player.h"
#include "helpers/ResourceLoader.h"
#include "utils/Constants.h"

#include <Bitmap.h>
#include <String.h>
#include <Window.h>


StatsBarView::StatsBarView(BRect frame)
	:
	BView(frame, "statsBarView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fPlayer(NULL)
{
	// Solid color fallback - Draw() will paint over it
	SetViewColor(80, 60, 40);
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

	// Draw wood background
	BBitmap* woodBg = ResourceLoader::Instance()->GetUIImage("wood2");
	if (woodBg != NULL) {
		SetDrawingMode(B_OP_COPY);
		DrawBitmap(woodBg, woodBg->Bounds(), bounds);
	} else {
		// Fallback
		SetHighColor(kBackgroundColor);
		FillRect(bounds);
	}

	// Draw top shadow
	SetHighColor(0, 0, 0, 100);
	StrokeLine(BPoint(bounds.left, bounds.top),
		BPoint(bounds.right, bounds.top));
	StrokeLine(BPoint(bounds.left, bounds.top + 1),
		BPoint(bounds.right, bounds.top + 1));

	float padding = 15;
	float iconBoxSize = 50;
	float barHeight = 10;

	// === HEALTH ROW ===
	float healthY = 10;

	// Draw health icon box (heart + number)
	DrawIconBox(BRect(padding, healthY, padding + iconBoxSize, healthY + iconBoxSize),
		"heart1", fPlayer != NULL ? fPlayer->Health() : 0);

	// Draw health progress bar (capsule style)
	float barX = padding + iconBoxSize + 10;
	float barWidth = bounds.Width() - barX - padding;
	BRect healthBarRect(barX, healthY + (iconBoxSize - barHeight) / 2,
		barX + barWidth, healthY + (iconBoxSize + barHeight) / 2);
	DrawProgressBar(healthBarRect, kHealthBarColor,
		fPlayer != NULL ? (float)fPlayer->Health() / kMaxHealth : 0);

	// === WEAPON ROW ===
	float weaponY = healthY + iconBoxSize + 10;

	// Draw shield icon box (weapon strength)
	DrawIconBox(BRect(padding, weaponY, padding + iconBoxSize, weaponY + iconBoxSize),
		"shield1", fPlayer != NULL && fPlayer->HasWeapon() ? fPlayer->Weapon() : 0);

	// Draw sword icon box (strongest monster attackable)
	float swordX = padding + iconBoxSize + 8;
	int strongestMonster = fPlayer != NULL ? fPlayer->StrongestMonsterCanAttack() : 0;
	DrawIconBox(BRect(swordX, weaponY, swordX + iconBoxSize, weaponY + iconBoxSize),
		"sword1", strongestMonster);

	// Draw weapon progress bar (capsule style)
	float weaponBarX = swordX + iconBoxSize + 10;
	float weaponBarWidth = bounds.Width() - weaponBarX - padding;
	BRect weaponBarRect(weaponBarX, weaponY + (iconBoxSize - barHeight) / 2,
		weaponBarX + weaponBarWidth, weaponY + (iconBoxSize + barHeight) / 2);
	DrawProgressBar(weaponBarRect, kWeaponBarColor,
		strongestMonster > 0 ? (float)strongestMonster / kMaxMonsterStrength : 0);
}


void
StatsBarView::DrawIconBox(BRect boxRect, const char* iconName, int value)
{
	// Draw rounded rect background with glass-like material effect
	SetHighColor(60, 60, 70, 200);
	FillRoundRect(boxRect, 10, 10);

	// Draw border
	SetHighColor(40, 40, 50);
	SetPenSize(2);
	StrokeRoundRect(boxRect, 10, 10);
	SetPenSize(1);

	// Draw icon
	float iconSize = 30;
	float iconX = boxRect.left + (boxRect.Width() - iconSize) / 2;
	float iconY = boxRect.top + 5;

	BBitmap* icon = ResourceLoader::Instance()->GetGlyph(iconName);
	if (icon != NULL) {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(icon, BRect(0, 0, icon->Bounds().Width(), icon->Bounds().Height()),
			BRect(iconX, iconY, iconX + iconSize, iconY + iconSize));
		SetDrawingMode(B_OP_COPY);
	}

	// Draw value text below icon
	BFont font;
	font.SetSize(18);
	font.SetFace(B_BOLD_FACE);
	SetFont(&font);
	SetHighColor(kTextColor);

	BString valueStr;
	valueStr.SetToFormat("%d", value);
	float textWidth = StringWidth(valueStr.String());
	float textX = boxRect.left + (boxRect.Width() - textWidth) / 2;
	float textY = boxRect.bottom - 5;
	DrawString(valueStr.String(), BPoint(textX, textY));
}


void
StatsBarView::DrawProgressBar(BRect barRect, rgb_color fillColor, float fillRatio)
{
	float radius = barRect.Height() / 2;

	// Draw capsule background with material effect
	SetHighColor(40, 40, 50, 200);
	FillRoundRect(barRect, radius, radius);

	// Draw filled portion
	if (fillRatio > 0) {
		BRect fillRect = barRect;
		fillRect.right = fillRect.left + (barRect.Width() * fillRatio);
		if (fillRect.Width() > radius * 2) {
			SetHighColor(fillColor);
			FillRoundRect(fillRect, radius, radius);
		} else if (fillRect.Width() > 0) {
			// For very small fills, just draw a circle
			SetHighColor(fillColor);
			FillEllipse(BPoint(fillRect.left + radius, fillRect.top + radius),
				radius, radius);
		}
	}

	// Draw gradient highlight on top
	SetHighColor(255, 255, 255, 40);
	BRect highlightRect = barRect;
	highlightRect.bottom = highlightRect.top + barRect.Height() / 3;
	FillRoundRect(highlightRect, radius / 2, radius / 2);

	// Draw border
	SetHighColor(100, 100, 110);
	StrokeRoundRect(barRect, radius, radius);
}


BRect
StatsBarView::GetSwordBoxRect()
{
	float padding = 15;
	float iconBoxSize = 50;
	float healthY = 10;
	float weaponY = healthY + iconBoxSize + 10;
	float swordX = padding + iconBoxSize + 8;

	return BRect(swordX, weaponY, swordX + iconBoxSize, weaponY + iconBoxSize);
}


void
StatsBarView::MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage)
{
	UpdateTooltipForPoint(where);
	BView::MouseMoved(where, transit, dragMessage);
}


void
StatsBarView::UpdateTooltipForPoint(BPoint point)
{
	if (fPlayer == NULL) {
		SetToolTip((const char*)NULL);
		return;
	}

	// Check if hovering over sword icon
	BRect swordRect = GetSwordBoxRect();
	if (swordRect.Contains(point)) {
		int strongestMonster = fPlayer->StrongestMonsterCanAttack();
		BString tipText;
		if (strongestMonster > 0) {
			tipText.SetToFormat("Can attack monsters with strength %d or less",
				strongestMonster);
		} else {
			tipText = "No weapon equipped";
		}
		SetToolTip(tipText.String());
		return;
	}

	// Not over any icon, clear tooltip
	SetToolTip((const char*)NULL);
}
