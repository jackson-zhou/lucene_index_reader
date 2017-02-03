void read_tip(const char* indexName, int numFields);
void read_tip_with_fp(FILE* indexIn, int numFields,int f_start, int f_len);
void FieldReader(FILE* indexIn, unsigned char* rootCode,int numBytes,int docCount,
              uint64_t indexStartFP,  unsigned char* minTerm, int minTermLen, unsigned char* maxTerm, int maxTermLen) ;


