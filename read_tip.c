#include <stdio.h>
#include <stdlib.h>
#include "CodecUtil.h"
#include <string.h>
char readByteR(const char* pBuf,int* pos) {
	return pBuf[(*pos)--];
}
int readVIntR(const char* pBuf,int* pos) {
	/* This is the original code of this method,
	 * but a Hotspot bug (see LUCENE-2975) corrupts the for-loop if
	 * readByte() is inlined. So the loop was unwinded!
	 char b = readByte();
	 int i = b & 0x7F;
	 for (int shift = 7; (b & 0x80) != 0; shift += 7) {
	 b = readByte();
	 i |= (b & 0x7F) << shift;
	 }
	 return i;
	 */
	char b = readByteR(pBuf, pos);
	if (b >= 0) return b;
	int i = b & 0x7F;
	b = readByteR(pBuf, pos);
	i |= (b & 0x7F) << 7;
	//if (b >= 0) return htonl(i);
	if (b >= 0) return i;
	b = readByteR(pBuf, pos);
	i |= (b & 0x7F) << 14;
	if (b >= 0) return i;
	b = readByteR(pBuf, pos);
	i |= (b & 0x7F) << 21;
	if (b >= 0) return i;
	b = readByteR(pBuf, pos);
	// Warning: the next ands use 0x0F / 0xF0 - beware copy/paste errors:
	i |= (b & 0x0F) << 28;
	if ((b & 0xF0) == 0) return i;
}

int readVIntRP(const char* pBuf, int* pos, const char* szName) {
	uint32_t i = readVIntR(pBuf, pos);
	printf("%s = %u\n", szName, i);
	return i;
}

char readByteRP(const char* pBuf, int* pos, const char* szName) {
	char b = readByteR(pBuf, pos);
	printf("%s = %d\n", szName, b);
	return b;
}

void readBytesR(unsigned char* dst, int offset, int len, const char* src, int* pos) {
	for(int i=0;i<len;i++) {
		dst[offset+i] = src[(*pos)--];
		printf("%02x",dst[offset+i]);
	}
	printf("\n");
}
unsigned char* readEmptyOutput(const char* szFstEmpty, int numBytes) {
	int pos = numBytes - 1;
	//  BytesReader reader;
	int len = readVIntRP(szFstEmpty,&pos,"len");
	unsigned char* output = (unsigned char*)malloc(len);
	printf("output:");
	readBytesR(output,0,len,szFstEmpty,&pos);
	return output;
}
static const int BIT_FINAL_ARC = 1 << 0;
static const int BIT_LAST_ARC = 1 << 1;
static const int BIT_TARGET_NEXT = 1 << 2;
static const int BIT_STOP_NODE = 1 << 3;
static const int BIT_ARC_HAS_OUTPUT = 1 << 4;
static const int BIT_ARC_HAS_FINAL_OUTPUT = 1 << 5;
static const int BIT_TARGET_DELTA = 1 << 6;

struct Arc {
 long node;//节点当前在文件中的位置
 int numArcs;//一共有多少条弧线
 int bytesPerArc;//固定长度，以便随机访问节点
 int arcIdx;//表示当前的arc在整个数组中的位置
 long nextArc;//下一个节点的位置。
 long posArcsStart;//第一个节点的位置
 int flags;
 unsigned char* nextFinalOutput;
};



Arc readNextRealArc(Arc& arc, FILE* indexIn,int f_start) {
	arc.arcIdx++;
	fseek(indexIn,arc.posArcsStart,SEEK_SET);//
}
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
		bool bHasEmptyOutput = readByte(indexIn);
		//if (readByte(indexIn) == 1) {
		unsigned char* emptyOutput = NULL;
	    if (bHasEmptyOutput) {	
			printf("-->accepts empty string\n");
			int numBytes = readVIntP(indexIn,"numBytes");
	
			char *szFst = (char*)malloc(numBytes+10);
			memset(szFst,0,numBytes+10);
			fread(szFst,numBytes,1,indexIn);
			printf("first FST<<< \n");
			emptyOutput = readEmptyOutput(szFst, numBytes);
			printf("first FST>>> \n");
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

		int startNode  = readVLongP(indexIn,"startNode");
		int numBytes = readVIntP(indexIn,"numBytes");
		int pos = numBytes - 1;
	    char *szFst = (char*)malloc(numBytes+10);
		memset(szFst,0,numBytes+10);
		fread(szFst,numBytes,1,indexIn);
		printf("second FST<<<<<\n");

		Arc arc;
		memset(&arc,0,sizeof(arc));
		arc.arcIdx = -1;
		if (NULL != emptyOutput) {
			arc.flags = BIT_FINAL_ARC | BIT_LAST_ARC;
			arc.nextFinalOutput = emptyOutput;
			/*	
			if (emptyOutput != NO_OUTPUT) {
				arc.flags |= BIT_ARC_HAS_FINAL_OUTPUT;
			}
			*/	
		} else {
			arc.flags = BIT_LAST_ARC;
			//arc.nextFinalOutput = NO_OUTPUT;
		}
		

		// readFirstRealTargetArc
		readByteRP(szFst,&pos,"ARCS_AS_FIXED_ARRAY(0x20)");
		arc.numArcs = readVIntRP(szFst,&pos, "numArcs");
		arc.bytesPerArc = readVIntRP(szFst,&pos, "bytesPerArc");
		arc.arcIdx = -1;
		arc.nextArc = pos;
		arc.posArcsStart = pos;
		printf("nextArc = %d\n",arc.nextArc);
		readNextRealArc(arc,indexIn,f_start);


		/*
		for (int nIndex = 0; nIndex <numBytes; nIndex++)
		{
			printf("%02X",szFst[nIndex]);
		}
		*/	
		printf("second FST>>>>>\n");
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

