/*
 * DataArchiveStore.cpp
 *
 *  Created on: 06/09/2013
 *      Author: victor
 */

#include "DataArchiveStore.h"
#include "tre3/TreeArchive.h"
#include "tre3/TOCArchive.h"

DataArchiveStore::DataArchiveStore() : Logger("DataArchiveStore") {
	treeDirectory = NULL;
	tocDirectory = NULL;
}

DataArchiveStore::~DataArchiveStore() {
	delete treeDirectory;
	treeDirectory = NULL;

	delete tocDirectory;
	tocDirectory = NULL;
}

byte* DataArchiveStore::getData(const String& path, int& size) {
	ReadLocker locker(this);
	//read from local dir else from tres
	File* file = new File(path);
	byte* data = NULL;
	size = 0;

	try {
		Reference<FileReader*> test = new FileReader(file);

		if (file->exists()) {
			size = file->size();

			data = new byte[size];

			test->read((char*)data, size);

			test->close();

			test = NULL;

			delete file;

			return data;
		}
	} catch (Exception& e) {

	}

	delete file;

	if (tocDirectory != NULL) {
		data = tocDirectory->getBytes(path, size);
	}

	if (size == 0 && treeDirectory != NULL) {
		data = treeDirectory->getBytes(path, size);
	}

	if (size == 0)
		return NULL;

	return data;
}

int DataArchiveStore::loadTres(String& path, Vector<String>& treFilesToLoad) {
	Locker locker(this);

	if (treeDirectory != NULL) {
		//error("tree directory already loaded");

		return 0;
	}

	info("Loading TRE archives...");

	treeDirectory = new TreeArchive();

	int j = 0;

	for (int i = 0; i < treFilesToLoad.size(); ++i) {
		String file = treFilesToLoad.get(i);

		String fullPath = path + "/";
		fullPath += file;

		treeDirectory->unpackFile(fullPath);
	}

	info("Finished loading TRE archives.");

	return 0;
}

int DataArchiveStore::loadTOCs(String& path, Vector<String>& tocFilesToLoad) {
	Locker locker(this);

	if (tocDirectory != NULL) {
		return 0;
	}

	if (path.length() <= 1)
		return 2;

	if (tocFilesToLoad.size() == 0)
		return 3;

	info("Loading TOC archives...");

	tocDirectory = new TOCArchive();

	int j = 0;

	for (int i = 0; i < tocFilesToLoad.size(); ++i) {
		String file = tocFilesToLoad.get(i);

		String fullPath = path + "/";
		fullPath += file;

		tocDirectory->unpackFile(fullPath);
	}

	info("Finished loading TOC archives.");

	return 0;
}

IffStream* DataArchiveStore::openIffFile(const String& fileName) {
	if (fileName.isEmpty())
		return NULL;

	IffStream* iffStream = NULL;

	int size = 0;

	byte* data = getData(fileName, size);

	if (size == 0)
		return NULL;

	iffStream = new IffStream();

	if (iffStream != NULL) {
		try {
			if (!iffStream->parseChunks(data, size, fileName)) {
				delete iffStream;
				iffStream = NULL;
			}
		} catch (Exception& e) {
			delete iffStream;
			iffStream = NULL;
		}
	}

	delete [] data;

	return iffStream;
}

