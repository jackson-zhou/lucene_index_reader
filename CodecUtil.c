#include <stdio.h>
#include "CodecUtil.h"
#include "util.h"
int checkHeader(FILE*fp, char*szBuf) {
	int checkHeader = readIntP(fp, "checkHeader");
	return checkHeaderNoMagic(fp, szBuf);
}

int checkHeaderNoMagic(FILE*fp, char* szBuf) {
	char* actualCodec = readStringP(fp,szBuf,"actualCodec");

	uint32_t actualVersion = readIntP(fp,"actualVersion");
	return actualVersion;
}
uint32_t checkIndexHeader(FILE*fp, char*szBuf)
{
	uint32_t version = checkHeader(fp,szBuf);
	readObjectID(fp,szBuf);
	readObjectSuffix(fp,szBuf);
	return version;
}


