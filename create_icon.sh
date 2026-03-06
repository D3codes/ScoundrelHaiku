#!/bin/bash
# Create and set app icon on Haiku
# Run this script on Haiku after building: ./create_icon.sh

set -e

PNG_FILE="deck.png"
HVIF_FILE="icon.hvif"
APP_NAME="Scoundrel"

# Find the built application
if [ -d "objects.x86_64-unknown-haiku" ]; then
    APP_PATH="objects.x86_64-unknown-haiku/$APP_NAME"
elif [ -f "$APP_NAME" ]; then
    APP_PATH="$APP_NAME"
else
    echo "Error: Build the app first with 'make'"
    exit 1
fi

if [ ! -f "$PNG_FILE" ]; then
    echo "Error: $PNG_FILE not found"
    exit 1
fi

echo "Converting $PNG_FILE to HVIF format..."

# Use Haiku's built-in icon tools
# Method 1: Try using 'iconutil' or similar if available
if command -v png2hvif &> /dev/null; then
    png2hvif "$PNG_FILE" "$HVIF_FILE"
elif command -v Icon-O-Matic &> /dev/null; then
    echo "Please manually convert using Icon-O-Matic:"
    echo "1. Open Icon-O-Matic"
    echo "2. Import $PNG_FILE (File > Import)"
    echo "3. Save as $HVIF_FILE (File > Export as HVIF)"
    echo "4. Run: addattr -t icon BEOS:ICON $HVIF_FILE $APP_PATH"
    exit 0
else
    # Use Haiku's Tracker to set icon from PNG
    echo "Setting icon using addattr..."

    # Read PNG and set as icon attribute
    # Haiku can read PNG files for icons in some contexts
    resattr -o "$APP_PATH" "$PNG_FILE" 2>/dev/null || {
        echo ""
        echo "Automatic conversion not available."
        echo "Please set the icon manually:"
        echo ""
        echo "1. Right-click on the built '$APP_NAME' application"
        echo "2. Select 'Icon' or use Icon-O-Matic"
        echo "3. Import deck.png and save"
        echo ""
        echo "Or use Icon-O-Matic from the command line:"
        echo "  Icon-O-Matic -i $PNG_FILE -o $HVIF_FILE"
        echo "  addattr -t icon BEOS:ICON $HVIF_FILE $APP_PATH"
    }
fi

echo "Done! Icon should now be set on $APP_PATH"
