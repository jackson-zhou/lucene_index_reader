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


const char *indexPath = "index_test";
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

char readByte(FILE* fp)
{
	char c = 0;
	char sz[1024] = {0};
	fread(sz,1, 1, fp);
	return sz[0];
}

uint32_t readInt(FILE* fp)
{
	uint32_t i = 0;

	fread(&i, 4, 1, fp);
	i = ntohl(i);
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

int readVInt(FILE* fp)
{
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
	if (b >= 0) return htonl(i);
	b = readByte(fp);
	i |= (b & 0x7F) << 14;
	if (b >= 0) return htonl(i);
	b = readByte(fp);
	i |= (b & 0x7F) << 21;
	if (b >= 0) return htonl(i);
	b = readByte(fp);
	// Warning: the next ands use 0x0F / 0xF0 - beware copy/paste errors:
	i |= (b & 0x0F) << 28;
	if ((b & 0xF0) == 0) return htonl(i);
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

void readSi(const char* segment)
{
	//char fn [NAME_MAX+1];
	//sprintf(fn,"%s/%s.si",indexPath,segment);
	
	FILE* fp = fopen(segment,"r");
	if (NULL == fp) 
	{
		printf("文件不存在:%s\n",segment);
	}
	char szBuf[1024] = { 0 };
    int format = readIndexHeader(fp,szBuf);
	readVersion(fp,szBuf);
//	const char* SegVersion = readString(fp,szBuf);
//	printf("SegVersion=%s\n",SegVersion);
	uint32_t SegSize = readInt(fp);
	printf("SegSize = %u\n",SegSize);
	char IsCompoundFile = readByte(fp);
	printf("IsCompoundFile = %d\n",IsCompoundFile);
	printf("Diagnositics:<-----------\n");
	readMapStringString(fp,szBuf);
	printf("Diagnositics:----------->\n");

	printf("Files:<--------------\n");
	readSetOfStrings(fp,szBuf);
	printf("Files:<-------------->\n");

	printf("Attributes:<-----------\n");
	readMapStringString(fp,szBuf);
	printf("Attributes:----------->\n");
	
	uint32_t IndexSort = readVInt(fp);
	printf("IndexSort=%u\n", IndexSort);
	for (int i = 0; i < IndexSort; i ++)
	{
		//TODO:
		char* fieldName = readString(fp,szBuf);
		int sortTypeID = readVInt(fp);
		char b = readByte(fp);
		b = readByte(fp);
	}

	readFooter(fp);
}

void readFieldInfo(FILE*fp, char* szBuf)
{
	fseek(fp,1182,0);
    int format = readIndexHeader(fp,szBuf);
	int FieldsCount = readVInt(fp);
	printf("FieldsCount = %d",FieldsCount);
	for (int i = 0; i < FieldsCount;i++)
	{
		printf("FieldName:%s\n",readString(fp,szBuf));
		printf("FieldNumber:%d\n",readVInt(fp));
		printf("FieldBits:%d\n",readByte(fp));
		printf("DocValuesBits:%d\n",readByte(fp));
		printf("DocValuesGen:%llu\n",readLong(fp));
		printf("Attributes:<-----------\n");
		readMapStringString(fp,szBuf);
		printf("Attributes:----------->\n");

//		printf("DimensionCount:%d\n",readVInt(fp));
//		printf("DimensionNumBytes:%d\n",readVInt(fp));
	}	
	readFooter(fp);
}
void readCfe(const char* fn)
{
	FILE* fp = fopen(fn,"r");
	if (NULL == fp) 
	{
		printf("文件不存在\n");
	}
	char szBuf[1024] = { 0 };
    int format = readIndexHeader(fp,szBuf);
	int FileCount = readVInt(fp);

	printf("FileCount= %d\n",FileCount);
	for (int i = 0; i < FileCount;i++)
	{
		char* FileName = readString(fp,szBuf);
		printf("FileName = %s\n",FileName);
		uint64_t DataOffset = readLong(fp);
		printf("DataOffset = %llu\n",DataOffset);
		uint64_t DataLength = readLong(fp);
		printf("DataLength = %llu\n",DataLength);
	}
}

long getLastCommitGeneration(const char* indexPath)
{
	const char* SEGMENTS = "segments";
	const char* OLD_SEGMENTS_GEN = "segments.gen";
	int len_segments = strlen(SEGMENTS);


	/** Name of the generation reference file name */
	DIR    *dir;
	struct    dirent    *ptr;
	dir = opendir(indexPath); ///open the dir
	long ret = -1;
	while((ptr = readdir(dir)) != NULL) ///read the list of this dir
	{
		//	printf("d_name: %s\n", ptr->d_name);
		const char *src = SEGMENTS;
		const char *dst = ptr->d_name;

		int cmp = 0;     
		while(!(cmp= *src - *dst)
				&& *src)     
		{
			++src, ++dst; 
		}
		if (*src == 0)
		{//ptr->d_name.startsWith(SEGMENTS)
			;
			long seg = atol(&ptr->d_name[len_segments+1]);
			if (seg > ret) 
				ret = seg;
		}
	}
	closedir(dir);
	return ret;
}

void readSegemet_N(const char *fn)
{
	FILE* fp = fopen(fn,"r");
	if (NULL == fp) {
		printf("文件不存在\n");
	}

	char szBuf[1024] = { 0 };
	// checkHeaderNoMagic
    int format = readIndexHeader(fp,szBuf);
	
	if ( format >= 6)//VERSION_53
	{
		readLuceneVersion(fp,szBuf);
	}
	uint64_t version = readLong(fp);
	printf("version = %llu\n",version);
	int NameCounter = readInt(fp);
	printf("NameCounter = %u\n",NameCounter);
	int SegCount = readInt(fp);
	printf("SegCount = %u\n",SegCount);

	if ( format >= 6)//VERSION_53
	{
		if (SegCount > 0)
		{
			int  major, minor, bugfix;
			major = readVInt(fp);
			minor = readVInt(fp);
			bugfix = readVInt(fp);
			printf("minSegmentLuceneVersion major=%d, minor = %u, bugfix = %u\n"
					,major,minor,bugfix );
		}
	}
//	int64_t infors_version = readLong(fp);
//	int infors_cou
	printf("begin read segment:-----------------\n");
	char segment[128];
	for (int  seg = 0; seg < SegCount; seg++)
	{
		char* segName = readString(fp,szBuf);
		printf("segName = %s\n",segName);
		strcpy(segment,segName);
		char hasID = readByte(fp);
		if (1 == hasID)
		{
			unsigned char ObjectID[ID_LENGTH] = {0};
			fread(ObjectID, ID_LENGTH, 1, fp);

			printf("SegID:");
			for (int nIndex = 0; nIndex <ID_LENGTH; nIndex++)
			{
				printf("%02X",ObjectID[nIndex]);
			}
			printf("\n");
		}
		char* codec = readString(fp,szBuf);
		printf("SegCodec = %s\n",codec);
		printf("\n\n\n<<<<<<<<<<<\n");
		readSi(segment);
		printf(">>>>>>>>>>\n\n\n");

		int64_t DelGen = readLong(fp);
		printf("DelGen = %lld\n",DelGen);
		uint32_t DeletionCount = readInt(fp);
		printf("DeletionCount = %d\n",DeletionCount );
		uint64_t FieldInfosGen = readLong(fp);
		printf("FieldInfosGen = %lld\n",FieldInfosGen);
		uint64_t DocValuesGen = readLong(fp);
		printf("DocValuesGen = %lld\n",DocValuesGen);
		printf("FieldInfosFiles Files:<--------------\n");
		if (format >= 5)//VERSION_51
		{
			readSetOfStrings(fp,szBuf);
		}
		printf("FieldInfosFiles Files:<--------------\n");

		int numDVFields = readInt(fp);
		printf("numDVFields = %d\n",numDVFields );
		printf("DVFields:<--------------\n");
		for (int i = 0; i < numDVFields; i++)
		{//没有数据测试
			int nKey = readInt(fp);
			printf("key = %d\n",numDVFields );
			readSetOfStrings(fp,szBuf);
		}
		printf("DVFields:<--------------\n");

	}
	printf("==============\n");
}


int main()
{
	char fn [NAME_MAX+1];
	const char *indexPath = "index_test";
	long gen = getLastCommitGeneration(indexPath);
	sprintf(fn,"%s/segments_%d",indexPath,gen);
	//readSegemet_N(fn);
	sprintf(fn,"%s/_%df.si",indexPath,9);
	//readSi(fn);

	strcpy(fn, "index_test/_9f.cfe");
	readCfe(fn);
//	fn = "index_test/_0.cfs";
	FILE* fp = fopen(fn,"r");
	if (NULL == fp) 
	{
		printf("文件不存在\n");
	}
	char szBuf[1024] = { 0 };
	readFieldInfo(fp,szBuf);
	return 0;
}

