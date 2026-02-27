#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <Bitmap.h>
#include <ObjectList.h>
#include <Path.h>
#include <String.h>

struct BitmapCacheEntry {
	BString		name;
	BBitmap*	bitmap;
};

class ResourceLoader {
public:
	static ResourceLoader*	Instance();
	static void				Destroy();

						~ResourceLoader();

	BBitmap*			LoadBitmap(const char* name);
	BBitmap*			GetCardImage(const char* cardImageName);
	BBitmap*			GetGlyph(const char* glyphName);
	BBitmap*			GetUIImage(const char* uiImageName);
	BBitmap*			GetBackground(const char* backgroundName);

	BPath				GetDataPath() const;

private:
						ResourceLoader();

	BBitmap*			LoadFromPath(const BPath& path);
	BBitmap*			FindCached(const char* name);
	void				AddToCache(const char* name, BBitmap* bitmap);

	static ResourceLoader*		sInstance;
	BObjectList<BitmapCacheEntry>	fCache;
	BPath				fDataPath;
};

#endif // RESOURCE_LOADER_H
