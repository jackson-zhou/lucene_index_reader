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
const char *indexPath = "index_test";
void read_doc_with_fp(FILE* docIn) {
	char szBuf[1024] = { 0 };
	checkIndexHeader(docIn, szBuf);
	//FOrUtil
	readVIntP(docIn,"packedIntsVersion");
	for (int bpv = 1; bpv <= 32; ++bpv) {
		unsigned code = readVIntP(docIn,"code");
		int formatId = code >> 5;
		printf("formatId = %d\n",formatId);
		int bitsPerValue = (code & 31) + 1;
		printf("bitsPerValue = %d\n",bitsPerValue);
	}

}
void read_doc(const char* indexPath) {
	char szBuf[1024] = { 0 };
	FILE* docIn = fopen(indexPath,"r");
	if (NULL == docIn) {
		printf("文件不存在:%s\n",indexPath);
	}
	read_doc_with_fp(docIn);
}
int main()
{
	char fn [NAME_MAX+1];
	const char* doc_file = "_6x_Lucene50_0.doc";
	sprintf(fn,"%s/%s",indexPath,doc_file);
	read_doc(fn);
	return 1;
}

