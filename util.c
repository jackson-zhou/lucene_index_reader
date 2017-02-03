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


void writeByte(FILE* fp, char c)
{
	fwrite(&c,1,1,fp);
}

void writeVInt(FILE* fp, int i) {
	while ((i & ~0x7F) != 0) {
		writeByte(fp,(char)((i & 0x7F) | 0x80));
		i >>= 7;
	}
	writeByte(fp,(char)i);
}

char readByte(FILE* fp) {
	char c = 0;
	char sz[1024] = {0};
	fread(sz,1, 1, fp);
	return sz[0];
}
char readByteP(FILE* fp,const char* szName) {
	char c = readByte(fp);
	printf("%s = %d\n", szName, c);
	return c;
}

uint32_t readInt(FILE* fp) {
	uint32_t i = 0;

	fread(&i, 4, 1, fp);
	i = ntohl(i);
	return i;
}

uint32_t readIntP(FILE* fp, const char* szName) {
	uint32_t i = readInt(fp);
	printf("%s = %u\n", szName, i);
	return i;
}

char * readString(FILE* fp,char* szBuf)
{
	memset(szBuf,0,1024);
	char c = 0;
	fread(&c,1,  1, fp);
	if (c > 0)
		fread(szBuf, c, 1, fp);
	return szBuf;
}

char * readStringP(FILE* fp, char* szBuf, const char* szName) {
	char* szRet = readString(fp, szBuf);
	printf("%s = %s\n", szName, szRet);
	return szRet;
}

unsigned short readShort(FILE *fp)
{
	unsigned short i = 0;
	fread(&i,2,1,fp);
	i = ntohs(i);
	return i;
}
uint64_t readLong(FILE*fp)  
{  
	uint64_t ll;
	uint32_t temp_low, temp_high;  
	temp_low = readInt(fp);
	temp_high = readInt(fp);

	ll &= 0;  
	ll |= temp_low;  
	ll <<= 32;   
	ll |= temp_high;  
	return ll;  
}  
uint64_t readLongP(FILE*fp, const char* szName) {
	uint64_t l = readLong(fp);
	printf("%s:%llu\n", szName, l);
	return l;
}

int readVInt(FILE* fp) {
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
	char b = readByte(fp);
	if (b >= 0) return b;
	int i = b & 0x7F;
	b = readByte(fp);
	i |= (b & 0x7F) << 7;
	//if (b >= 0) return htonl(i);
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7F) << 14;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7F) << 21;
	if (b >= 0) return i;
	b = readByte(fp);
	// Warning: the next ands use 0x0F / 0xF0 - beware copy/paste errors:
	i |= (b & 0x0F) << 28;
	if ((b & 0xF0) == 0) return i;
}

int readVIntP(FILE* fp, const char* szName) {
	uint32_t i = readVInt(fp);
	printf("%s = %u\n", szName, i);
	return i;
}

uint64_t readVLong(FILE* fp) {
	char b = readByte(fp);
	if (b >= 0) return b;
	uint64_t i = b & 0x7FL;
	b = readByte(fp);
	i |= (b & 0x7FL) << 7;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7FL) << 14;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7FL) << 21;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7FL) << 28;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7FL) << 35;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7FL) << 42;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7FL) << 49;
	if (b >= 0) return i;
	b = readByte(fp);
	i |= (b & 0x7FL) << 56;
	if (b >= 0) return i;
	//if (allowNegative) {
	if (1) {
		b = readByte(fp);
		i |= (b & 0x7FL) << 63;
		if (b == 0 || b == 1) return i;
		printf("Invalid vLong detected (more than 64 bits)");
	} else {
		printf("Invalid vLong detected (negative values disallowed)");
	}
	return 0;
}
uint64_t readVLongP(FILE* fp, const char* szName) {
	int64_t l = readVLong(fp);
	printf("%s:%llu\n", szName, l);
	return l;
}
uint32_t readCodecHeader(FILE*fp,char* szBuf)
{
	uint32_t magic= readInt(fp);
	printf("magic number = %u\n",magic);

	char* codec = readString(fp,szBuf);
	printf("codec = %s\n",codec);

	uint32_t version = readInt(fp);
	printf("version = %u\n", version);
	return version;
}

void readObjectID(FILE*fp, char* szBuf)
{
#define ID_LENGTH 16
	unsigned char ObjectID[ID_LENGTH] = {0};
	fread(ObjectID, ID_LENGTH, 1, fp);

	printf("ObjectID:");
	for (int nIndex = 0; nIndex <ID_LENGTH; nIndex++)
	{
		printf("%02X",ObjectID[nIndex]);
	}
	printf("\n");
}

void readObjectSuffix(FILE*fp, char*szBuf)
{
	char suffixLength = readByte(fp);
	memset(szBuf,0,1024);
	fread(szBuf,suffixLength,1,fp);
	printf("suffixLength = %u, suffix=%s\n",suffixLength,szBuf);
}
	
uint32_t readIndexHeader(FILE*fp, char*szBuf)
{
	uint32_t version = readCodecHeader(fp,szBuf);
	readObjectID(fp,szBuf);
	readObjectSuffix(fp,szBuf);
	return version;
}

void readLuceneVersion(FILE*fp, char* szBuf)
{
	int  major, minor, bugfix;
	major = readVInt(fp);
	minor = readVInt(fp);
	bugfix = readVInt(fp);
	printf("major=%d, minor = %u, bugfix = %u,\n"
			,major,minor,bugfix);

	;
}

void readVersion(FILE*fp, char* szBuf)
{
	int  major, minor, bugfix;
	major = readInt(fp);
	minor = readInt(fp);
	bugfix = readInt(fp);
	printf("major=%d, minor = %u, bugfix = %u,\n"
			,major,minor,bugfix);

	;
}
unsigned char* readBytesRef(FILE*fp, const char* szName,int* len)
{
	int length = readVInt(fp);
	*len = length;
	unsigned char* bytes = (unsigned char*) malloc(length * sizeof(char));
	fread(bytes,length, 1, fp);
	printf("%s:",szName);
	for (int nIndex = 0; nIndex <length; nIndex++)
	{
		printf("%02X",bytes[nIndex]);
	}
	printf("\n");
	return bytes;
}



void readSetOfStrings(FILE*fp, char* szBuf)
{
	int count = readVInt(fp);
	for (int i = 0; i < count; i++)
	{
		char *szUpdateFiles = readString(fp,szBuf);
		printf("-setString->%s\n",szUpdateFiles);
	}
}

void readUpdatesFiles(FILE*fp,char* szBuf)
{
	int numDVFields = readInt(fp);
	for (int i = 0; i < numDVFields; i++) {
		uint32_t id = readInt(fp);
		printf("-->%u",id);
		readSetOfStrings(fp, szBuf);
	}
}

void readMapStringString(FILE*fp, char* szBuf)
{
	int numDVFields = readVInt(fp);
	printf("readMapStringString:size=%d\n",numDVFields);
	for (int i = 0; i < numDVFields; i++) {
		char* szKey = readString(fp,szBuf);
		printf("%s-->",szKey);
		char* szValue = readString(fp,szBuf);
		printf("%s\n",szValue);
	}
}
	
void readFooter(FILE*fp)
{
	uint32_t Magic  = readInt(fp);
	uint32_t AlgorithmID = readInt(fp);
	uint64_t Checksum = readLong(fp);
	printf("Magic=%u, AlgorighmID=%u, Checksum= %llu\n"
			,Magic, AlgorithmID, Checksum);
}
