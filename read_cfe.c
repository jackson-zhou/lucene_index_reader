// LucentTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdio.h"
#include <arpa/inet.h>
#include <memory.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "read_cfe.h"

const char *indexPath = "index_test";

struct CompoundEntries* readCfe(const char* fn)
{
	FILE* fp = fopen(fn,"r");
	if (NULL == fp) 
	{
		printf("文件不存在\n");
	}
	printf("<<<<<<<,read_cfe>>>>>>>\n");
	char szBuf[1024] = { 0 };
    int format = readIndexHeader(fp,szBuf);
	int FileCount = readVInt(fp);

	printf("FileCount= %d\n",FileCount);

	CompoundEntries * ce_head = NULL;
	CompoundEntries * pNode = ce_head;
	for (int i = 0; i < FileCount;i++)
	{
		struct CompoundEntries *newNode = (struct CompoundEntries *) malloc(sizeof(CompoundEntries));

		char* fileName = readStringP(fp,szBuf,"FileName");
		strcpy(newNode->fileName, fileName);

		uint64_t dataOffset = readLongP(fp,"DataOffset");
		uint64_t dataLength = readLongP(fp,"DataLength");

		newNode->dataOffset = dataOffset;
		newNode->dataLength = dataLength;
		newNode->next = NULL;

		if(NULL == ce_head) {
			pNode = ce_head = newNode;
		} else {
			pNode->next = newNode;
			pNode = newNode;
		}
	}
	return ce_head;
}
