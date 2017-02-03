#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "read_tim.h"
#include "read_tip.h"
#include "read_cfe.h"
#include <dirent.h>
#include "string.h"

void read_tm_from_compound() {
	char compoundFile [NAME_MAX+1];
	strcpy(compoundFile, "index_test/_0.cfe");
	struct CompoundEntries* ceHead	= readCfe(compoundFile);
	printf("==========================\n");
	int dataOffsetTim = 0;
	int dataLengthTim = 0;		

	int dataOffsetTip = 0;
	int dataLengthTip = 0;		


	for (struct CompoundEntries*pCe = ceHead;
			pCe != NULL;
			pCe = pCe->next) { 
		if (0 == strcmp("_Lucene50_0.tim",pCe->fileName)) {
			dataOffsetTim = pCe->dataOffset;
			dataLengthTim = pCe->dataLength;
		} else 	if (0 == strcmp("_Lucene50_0.tip",pCe->fileName)) {
			dataOffsetTip = pCe->dataOffset;
			dataLengthTip = pCe->dataLength;
		}
	}
	
	strcpy(compoundFile, "index_test/_0.cfs");
	FILE* termsIn = fopen(compoundFile,"rb");
	FILE* indexIn = fopen(compoundFile,"rb");
	// = _Lucene50_0.tim
	fseek(termsIn,dataOffsetTim,SEEK_SET);

	char* content = new char[dataLengthTim];
	FILE* fpW = fopen("_Lucene50_0.tim","wb");
	fread(content,dataLengthTim, 1, termsIn);
	fwrite(content, dataLengthTim, 1, fpW);
	fclose(fpW);
	
	fseek(termsIn,dataOffsetTim,SEEK_SET);
	int numFields = read_tim_with_fp(termsIn, indexIn,dataOffsetTim,dataLengthTim);
	//  _Lucene50_0.tip	
	printf("==========================\n");
	fseek(indexIn,dataOffsetTip,SEEK_SET);

	fpW = fopen("_Lucene50_0.tip","wb");
	fread(content,dataLengthTip, 1, indexIn);
	fwrite(content, dataLengthTip, 1, fpW);
	fclose(fpW);

	fseek(indexIn,dataOffsetTip,SEEK_SET);
	read_tip_with_fp(indexIn,numFields, dataOffsetTip, dataLengthTip);
}
int main()
{
	read_tm_from_compound();

	return 1;
}
