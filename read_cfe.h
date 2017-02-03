#pragma once
#include <dirent.h>
struct  CompoundEntries
{
	char fileName[NAME_MAX+1];
	int	dataOffset;
	int	dataLength;
	struct  CompoundEntries *next;
};
struct CompoundEntries* readCfe(const char* fn);
