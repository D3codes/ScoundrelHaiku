#ifndef MESSAGE_CODES_H
#define MESSAGE_CODES_H

// Menu messages
const uint32 kMsgNewGame = 'NEWG';
const uint32 kMsgResumeGame = 'RESG';
const uint32 kMsgHowToPlay = 'HELP';
const uint32 kMsgMainMenu = 'MENU';

// Game action messages
const uint32 kMsgCardSelected = 'CARD';
const uint32 kMsgCardAction = 'CACT';
const uint32 kMsgFlee = 'FLEE';
const uint32 kMsgPause = 'PAUS';
const uint32 kMsgContinue = 'CONT';
const uint32 kMsgNextDungeon = 'NXTD';

// Card action types
const uint32 kActionDrink = 'DRNK';
const uint32 kActionEquip = 'EQUP';
const uint32 kActionAttackUnarmed = 'UARM';
const uint32 kActionAttackWeapon = 'WEAP';
const uint32 kActionCancel = 'CNCL';

// Game state change notifications
const uint32 kMsgGameStateChanged = 'GSCH';
const uint32 kMsgRoomUpdated = 'ROOM';
const uint32 kMsgPlayerUpdated = 'PLYR';
const uint32 kMsgScoreUpdated = 'SCOR';

// Window messages
const uint32 kMsgCloseModal = 'CLSM';

// Sound messages
const uint32 kMsgToggleSound = 'TSND';

// Animation messages
const uint32 kMsgAnimationTick = 'ANTK';
const uint32 kMsgDealNextCard = 'DEAL';
const uint32 kMsgDeckCountChanged = 'DKCT';

#endif // MESSAGE_CODES_H
