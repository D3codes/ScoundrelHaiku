#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <InterfaceDefs.h>
#include <SupportDefs.h>

// Application signature
#define APP_SIGNATURE "application/x-vnd.Scoundrel"

// Window dimensions
const float kWindowWidth = 400.0f;
const float kWindowHeight = 600.0f;

// Card dimensions
const float kCardWidth = 80.0f;
const float kCardHeight = 120.0f;
const float kCardSpacing = 10.0f;

// Room layout
const float kRoomPadding = 20.0f;

// Top bar dimensions
const float kTopBarHeight = 80.0f;

// Stats bar dimensions
const float kStatsBarHeight = 130.0f;

// Button dimensions
const float kButtonWidth = 100.0f;
const float kButtonHeight = 30.0f;

// Colors
const rgb_color kBackgroundColor = {40, 40, 50, 255};
const rgb_color kCardBackgroundColor = {240, 230, 200, 255};
const rgb_color kMonsterColor = {180, 50, 50, 255};
const rgb_color kWeaponColor = {80, 80, 180, 255};
const rgb_color kPotionColor = {50, 150, 80, 255};
const rgb_color kHealthBarColor = {200, 50, 50, 255};
const rgb_color kWeaponBarColor = {100, 100, 200, 255};
const rgb_color kTextColor = {230, 230, 230, 255};
const rgb_color kDarkTextColor = {30, 30, 30, 255};
const rgb_color kButtonColor = {80, 80, 100, 255};
const rgb_color kButtonHighlightColor = {100, 100, 120, 255};

// Game constants
const int kMaxHealth = 20;
const int kStartingHealth = 20;
const int kMaxWeaponStrength = 10;
const int kMaxMonsterStrength = 14;
const int kMinCardStrength = 2;
const int kRoomSize = 4;

// Animation timing (microseconds)
const bigtime_t kCardDealDelay = 100000; // 100ms

// Font sizes
const float kTitleFontSize = 24.0f;
const float kHeadingFontSize = 18.0f;
const float kBodyFontSize = 14.0f;
const float kSmallFontSize = 12.0f;

#endif // CONSTANTS_H
