#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "CodecUtil.h"
#include <string.h>
void read_tip_with_fp(FILE* indexIn, int numFields,int f_start, int f_len) {
	printf("<<<<<<<,read_tip>>>>>>>\n");
	char szBuf[1024] = { 0 };
	// Header
	checkIndexHeader(indexIn,szBuf);

	printf("bein enum fields:------\n");
	// 一共有numFields
	for (int i = 0; i < numFields; ++i) {
		long curpos = ftell(indexIn);
		curpos	-= f_start;
		printf("file current pos = %ld\n",curpos);

		// FSTIndex,也就是一整棵FST数。这个内容其实是由FST::save写入
		// 每个FSTIndex由 header, packet, <packet? , FSTentity>, FSTentity
		int version = checkHeader(indexIn, szBuf);
		printf("version = %d\n",version);
		char packed = (readByte(indexIn) == 1);
		printf("packed = %d\n",packed);
		if (readByte(indexIn) == 1) {
			printf("-->accepts empty string\n");
			int numBytes = readVIntP(indexIn,"numBytes");
	
			unsigned char *szFST = (unsigned char*)malloc(numBytes+10);
			memset(szFST,0,numBytes+10);
			fread(szFST,numBytes,1,indexIn);
			printf("first FST==>");
			for (int nIndex = 0; nIndex <numBytes; nIndex++)
			{
				printf("%02X",szFST[nIndex]);
			}
			printf("\n");
			//printf("%s\n",szFST);

		}
		char t = readByteP(indexIn,"t");
		/*
		  switch(t) {
		  case 0:
			  inputType = INPUT_TYPE.BYTE1;
			  break;
		  case 1:
			  inputType = INPUT_TYPE.BYTE2;
			  break;
		  case 2:
			  inputType = INPUT_TYPE.BYTE4;
			  break;
		  default:
			  throw new IllegalStateException("invalid input type " + t);
																					      }
		 */

		readVLongP(indexIn,"startNode");
		int numBytes = readVIntP(indexIn,"numBytes");
		unsigned char *szFST = (unsigned char*)malloc(numBytes+10);
		memset(szFST,0,numBytes+10);
		fread(szFST,numBytes,1,indexIn);
		printf("second FST==>");
		for (int nIndex = 0; nIndex <numBytes; nIndex++)
		{
			printf("%02X",szFST[nIndex]);
		}
		printf("\n");
		//printf("%s\n",szFST);
		printf("--------------\n");
	}

	for (int i = 0; i < numFields; ++i) {
		// IndexStartFP
		uint64_t indexStartFP = readVLongP(indexIn,"indexStartFP");
	}
	int dirOffset = readLongP(indexIn,"DirOffset");
}
void read_tip(const char* indexName, int numFields) {
	FILE* indexIn = fopen(indexName,"r");
	if (NULL == indexIn) {
		printf("文件不存在:%s\n",indexName);
	}
	read_tip_with_fp(indexIn, numFields, 0, -1);
}


uint64_t readVLong_withBuf(unsigned char*bytes, int pos) {
	char b = bytes[pos++];
	if (b >= 0) return b;
	uint64_t i = b & 0x7FL;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 7;
	if (b >= 0) return i;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 14;
	if (b >= 0) return i;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 21;
	if (b >= 0) return i;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 28;
	if (b >= 0) return i;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 35;
	if (b >= 0) return i;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 42;
	if (b >= 0) return i;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 49;
	if (b >= 0) return i;
	b = bytes[pos++];
	i |= (b & 0x7FL) << 56;
	if (b >= 0) return i;
	printf("Invalid vLong detected (negative values disallowed)");
	return 0;
}
void FieldReader(FILE* indexIn, unsigned char* rootCode,int numBytes,int docCount,
              uint64_t indexStartFP,  unsigned char* minTerm, int minTermLen, unsigned char* maxTerm, int maxTermLen) {

	uint64_t rootBlockFP = 0;
	rootBlockFP = readVLong_withBuf(rootCode,0);
	printf("%s:%llu\n", "rootBlockFP", rootBlockFP);
	const int OUTPUT_FLAGS_NUM_BITS = 2;
    rootBlockFP = rootBlockFP >> OUTPUT_FLAGS_NUM_BITS;
	printf("%s:%llu\n", "rootBlockFP", rootBlockFP);

    if (indexIn != NULL) {
		char szBuf[1024] = { 0 };
		fseek(indexIn,indexStartFP,SEEK_SET);//
		int version = checkHeader(indexIn, szBuf);
		printf("version = %d\n",version);
		char packed = (readByte(indexIn) == 1);
		printf("packed = %d\n",packed);
		if (readByte(indexIn) == 1) {
			printf("accepts empty string\n");
			int numBytes = readVIntP(indexIn,"numBytes");
			fseek(indexIn,numBytes, SEEK_CUR);
		}
		char t = readByteP(indexIn,"t");
		readVLongP(indexIn,"startNode");
		int numBytes = readVIntP(indexIn,"numBytes");
		fseek(indexIn,numBytes, SEEK_CUR);
	}
}

