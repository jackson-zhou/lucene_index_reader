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
#include "CodecUtil.h"
#include "read_tip.h"
#include "read_tim.h"
int read_tim_with_fp(FILE* termsIn,FILE* indexIn,int f_start, int f_len) {
	printf("<<<<<<<,read_tim>>>>>>>\n");
    int numFields = 0;
	char szBuf[1024] = { 0 };
	//Header
	checkIndexHeader(termsIn, szBuf);

	const char* segName = "_6x";
	const char* segmentSuffix = "Lucene50_0";
	char indexName[NAME_MAX+1];
	sprintf(indexName,"%s/%s_%s.tip",indexPath,segName,segmentSuffix);

	if (NULL == indexIn) {
		indexIn = fopen(indexName,"r");
		if (NULL == indexIn) {
			printf("文件不存在:%s\n",indexName);
		}
	}
	
	//PostingsHeader
	checkIndexHeader(termsIn, szBuf);
	int indexBlockSize = readVIntP(termsIn,"indexBlockSize");
	
	//DirOffset
	const int footerLength  = 16;
	int offset = (0 - (footerLength + 8));
	if (-1 == f_len) {
		fseek(termsIn, offset, SEEK_END);
	} else {
		fseek(termsIn,f_start + f_len + offset,SEEK_SET);//
	}

	uint64_t dirOffset = readLongP(termsIn,"dirOffset");
	fseek(termsIn,f_start + dirOffset,SEEK_SET);//

	printf("read:%s<<<<<<<<<<<<<\n",indexName);
	offset = (0 - (footerLength + 8));
	fseek(indexIn, offset, SEEK_END);
	dirOffset = readLongP(indexIn,"dirOffset");
	fseek(indexIn,dirOffset,SEEK_SET);//
	printf("read:%s>>>>>>>>>>>>>\n",indexName);

	//FieldSummary
    numFields = readVIntP(termsIn,"numFields");
	//read_tip_with_fp(indexIn,numFields);
    for (int i = 0; i < numFields; ++i) {
		int field = readVIntP(termsIn,"field");
		uint64_t numTerms = readVLongP(termsIn,"numTerms");
		int numBytes = readVIntP(termsIn,"numBytes");
		unsigned char* rootCode = (unsigned char*) malloc((numBytes + 1) * sizeof(char*));
		memset(rootCode,numBytes+1,0);
		fread(rootCode,numBytes, 1, termsIn);
		printf("rootCode:");
		for (int nIndex = 0; nIndex <numBytes; nIndex++)
		{
			printf("%02X",rootCode[nIndex]);
		}
		printf("\n");

		uint64_t sumTotalTermFreq = readVLongP(termsIn,"sumTotalTermFreq");
		uint64_t sumDocFreq = readVLongP(termsIn,"sumDocFreq");
		int docCount = readVIntP(termsIn,"docCount");
		int longSize = readVIntP(termsIn,"LongSize");
		int minTermLen = 0;
		unsigned char* minTerm = readBytesRef(termsIn, "minTerm", &minTermLen);
		int maxTermLen = 0;
		unsigned char* maxTerm = readBytesRef(termsIn, "maxTerm", &maxTermLen);
		printf("read:%s<<<<<<<<<<<<<\n",indexName);
		uint64_t indexStartFP = readVLongP(indexIn,"indexStartFP");
		FieldReader(indexIn, rootCode, numBytes, docCount, indexStartFP,  minTerm, minTermLen, maxTerm,maxTermLen);
		//break;
		printf("read:%s>>>>>>>>>>>>>\n",indexName);

		free(rootCode);
		free(minTerm);
		free(maxTerm);
		printf("-----------------------------------------\n");
	}
	return numFields;
}
int read_tim(const char* indexPath) {
	char szBuf[1024] = { 0 };
	FILE* termsIn = fopen(indexPath,"r");
	if (NULL == termsIn) {
		printf("文件不存在:%s\n",indexPath);
	}
	return read_tim_with_fp(termsIn,NULL,0,-1);
}


