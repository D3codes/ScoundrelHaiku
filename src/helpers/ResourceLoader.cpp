#include "ResourceLoader.h"

#include <Application.h>
#include <Bitmap.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Roster.h>
#include <TranslationUtils.h>

#include <stdio.h>

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
	bool found = false;

	if (be_app != NULL && be_app->GetAppInfo(&info) == B_OK) {
		BPath appPath(&info.ref);
		appPath.GetParent(&fAppPath);
		fDataPath = fAppPath;
		fDataPath.Append("data");

		// Verify path exists
		BDirectory testDir(fDataPath.Path());
		if (testDir.InitCheck() == B_OK) {
			found = true;
		}
	}

	if (!found) {
		// Try current working directory
		fAppPath.SetTo(".");
		fDataPath.SetTo("./data");
		BDirectory testDir(fDataPath.Path());
		if (testDir.InitCheck() != B_OK) {
			// Last resort - absolute path for development
			fAppPath.SetTo("/boot/home/Developer/ScoundrelHaiku");
			fDataPath.SetTo("/boot/home/Developer/ScoundrelHaiku/data");
		}
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


BPath
ResourceLoader::GetAppPath() const
{
	return fAppPath;
}


BBitmap*
ResourceLoader::GetAppDirImage(const char* imageName)
{
	BString cacheKey;
	cacheKey.SetToFormat("appdir/%s", imageName);

	// Check cache first
	BBitmap* cached = FindCached(cacheKey.String());
	if (cached != NULL)
		return cached;

	BPath path(fAppPath);
	path.Append(imageName);

	BBitmap* bitmap = LoadFromPath(path);
	if (bitmap != NULL)
		AddToCache(cacheKey.String(), bitmap);

	return bitmap;
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
	// Try .png first
	BString fullPath;
	fullPath.SetToFormat("images/ui/%s.png", uiImageName);
	BBitmap* bitmap = LoadBitmap(fullPath.String());
	if (bitmap != NULL)
		return bitmap;

	// Try .jpg as fallback
	fullPath.SetToFormat("images/ui/%s.jpg", uiImageName);
	return LoadBitmap(fullPath.String());
}


BBitmap*
ResourceLoader::GetBackground(const char* backgroundName)
{
	BString fullPath;
	fullPath.SetToFormat("images/backgrounds/%s.png", backgroundName);
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
