#include "stdio.h"
#include <stdlib.h>


int checkHeader(FILE*fp, char*szBuf);
int checkHeaderNoMagic(FILE*fp, char* szBuf);
uint32_t checkIndexHeader(FILE*fp, char*szBuf);

void writeByte(FILE* fp, char c);
void writeVInt(FILE* fp, int i); 
char readByte(FILE* fp);
char readByteP(FILE* fp,const char* szName);
uint32_t readInt(FILE* fp);
uint32_t readIntP(FILE* fp, const char* szName);
char * readString(FILE* fp,char* szBuf);
char * readStringP(FILE* fp,char* szBuf, const char* szName);
unsigned short readShort(FILE *fp);
uint64_t readLong(FILE*fp)  ;  
uint64_t readLongP(FILE*fp, const char* szName);  
int readVInt(FILE* fp);
int readVIntP(FILE* fp, const char* szName);
uint64_t readVLong(FILE* fp);
uint64_t readVLongP(FILE* fp, const char* szName);
unsigned char* readBytesRef(FILE*fp, const char* szName, int* len);
uint32_t readCodecHeader(FILE*fp,char* szBuf);
void readObjectID(FILE*fp, char* szBuf);
void readObjectSuffix(FILE*fp, char*szBuf);
uint32_t readIndexHeader(FILE*fp, char*szBuf);
void readLuceneVersion(FILE*fp, char* szBuf);
void readVersion(FILE*fp, char* szBuf);
void readSetOfStrings(FILE*fp, char* szBuf);
void readUpdatesFiles(FILE*fp,char* szBuf);
void readMapStringString(FILE*fp, char* szBuf);
void readFooter(FILE*fp);
