#include "ResourceLoader.h"

#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Roster.h>
#include <TranslationUtils.h>

ResourceLoader* ResourceLoader::sInstance = NULL;


ResourceLoader*
ResourceLoader::Instance()
{
	if (sInstance == NULL)
		sInstance = new ResourceLoader();
	return sInstance;
}


void
ResourceLoader::Destroy()
{
	delete sInstance;
	sInstance = NULL;
}


ResourceLoader::ResourceLoader()
	:
	fCache(20, true) // owns items
{
	// Get application directory
	app_info info;
	if (be_app->GetAppInfo(&info) == B_OK) {
		BPath appPath(&info.ref);
		appPath.GetParent(&fDataPath);
		fDataPath.Append("data");
	}
}


ResourceLoader::~ResourceLoader()
{
	// Delete cached bitmaps
	for (int i = 0; i < fCache.CountItems(); i++) {
		BitmapCacheEntry* entry = fCache.ItemAt(i);
		if (entry != NULL) {
			delete entry->bitmap;
		}
	}
}


BPath
ResourceLoader::GetDataPath() const
{
	return fDataPath;
}


BBitmap*
ResourceLoader::LoadBitmap(const char* name)
{
	// Check cache first
	BBitmap* cached = FindCached(name);
	if (cached != NULL)
		return cached;

	BPath path(fDataPath);
	path.Append(name);

	BBitmap* bitmap = LoadFromPath(path);
	if (bitmap != NULL)
		AddToCache(name, bitmap);

	return bitmap;
}


BBitmap*
ResourceLoader::GetCardImage(const char* cardImageName)
{
	BString fullPath;
	fullPath.SetToFormat("images/cards/%s.png", cardImageName);
	return LoadBitmap(fullPath.String());
}


BBitmap*
ResourceLoader::GetGlyph(const char* glyphName)
{
	BString fullPath;
	fullPath.SetToFormat("images/glyphs/%s.png", glyphName);
	return LoadBitmap(fullPath.String());
}


BBitmap*
ResourceLoader::GetUIImage(const char* uiImageName)
{
	BString fullPath;
	fullPath.SetToFormat("images/ui/%s.png", uiImageName);
	return LoadBitmap(fullPath.String());
}


BBitmap*
ResourceLoader::LoadFromPath(const BPath& path)
{
	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return NULL;

	return BTranslationUtils::GetBitmapFile(path.Path());
}


BBitmap*
ResourceLoader::FindCached(const char* name)
{
	for (int i = 0; i < fCache.CountItems(); i++) {
		BitmapCacheEntry* entry = fCache.ItemAt(i);
		if (entry != NULL && entry->name == name)
			return entry->bitmap;
	}
	return NULL;
}


void
ResourceLoader::AddToCache(const char* name, BBitmap* bitmap)
{
	BitmapCacheEntry* entry = new BitmapCacheEntry();
	entry->name = name;
	entry->bitmap = bitmap;
	fCache.AddItem(entry);
}
