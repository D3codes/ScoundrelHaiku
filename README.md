# Scoundrel for Haiku

A native Haiku OS port of the rogue-like dungeon card solitaire game.

![Haiku OS](https://img.shields.io/badge/platform-Haiku%20OS-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Version](https://img.shields.io/badge/version-1.0.0-orange)

## About

Scoundrel is a rogue-like card game where every shuffle could be your last. The deck is your dungeon, and each room is revealed one hand at a time. Make strategic decisions, manage your health, and battle through an unpredictable gauntlet of monsters, weapons, and potions.

This is a native port for Haiku OS, built using the Be API (C++).

## How to Play

- **Explore the Deck** - Each room consists of four cards drawn from the deck
- **Make Your Move** - Interact with three cards to advance - fight, heal, or equip
- **Plan Ahead** - The fourth card carries over to the next room. Choose wisely
- **Flee** - Run away to discard the room, but you can only flee once per dungeon
- **Survive** - If your health reaches 0, your run is over

### Card Types

| Card | Action |
|------|--------|
| **Monsters** (Spades/Clubs) | Attack to defeat. Unarmed attacks deal damage equal to monster strength. Weapons reduce damage taken |
| **Weapons** (Diamonds) | Equip to gain armor and attack power. Weapons degrade with each use |
| **Health Potions** (Hearts) | Drink to restore health up to your maximum of 20 |

### Combat Tips

- Weapons can only attack monsters weaker than the last monster defeated with that weapon
- Equipping a new weapon replaces your current one
- Health potions restore health equal to their value, but cannot exceed 20

## Building

### Requirements

- Haiku OS (R1/beta4 or later recommended)
- GCC compiler (included with Haiku)
- Standard Haiku development libraries

### Compile

```bash
cd ScoundrelHaiku
make
```

The compiled application will be created in the current directory.

### Run

```bash
./Scoundrel
```

Or double-click the Scoundrel application in Tracker.

## Features

- **Native Haiku Application** - Built with the Be API for authentic Haiku look and feel
- **Rogue-Like Gameplay** - No two runs are the same
- **Local High Scores** - Track your best runs
- **Save & Resume** - Your game is automatically saved when you quit
- **Background Music** - 8 atmospheric tracks that play continuously
- **Sound Effects** - Audio feedback for all game actions
- **Settings** - Adjustable volume for music and sound effects

## Project Structure

```
ScoundrelHaiku/
├── src/
│   ├── models/      # Game logic (Card, Deck, Player, Room, Game)
│   ├── views/       # UI components (GameWindow, CardView, etc.)
│   │   └── modals/  # Modal windows (Pause, Settings, etc.)
│   ├── helpers/     # Utilities (ResourceLoader, SoundPlayer, etc.)
│   └── utils/       # Constants and message codes
├── data/
│   ├── images/      # Card art, icons, backgrounds
│   └── sounds/      # Sound effects and music
└── resources/       # Haiku resource definitions
```

## Credits

**Created by** [David Freeman](https://d3.codes/about)

### Also Available

Scoundrel is also available on the App Store for iOS devices.

## License

MIT License - See LICENSE file for details.

---

*How deep can you go before your luck runs out?*
