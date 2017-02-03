all: lucene_reader read_tim read_doc read_cfe t
lucene_reader: lucene_reader.cc 
	g++ -g lucene_reader.cc -o lucene_reader
read_tim:read_tim.c read_cfe.c	 read_tim_test.c util.c CodecUtil.c read_tip.c 
	g++ -g -O0  -o read_tim read_tim.c read_cfe.c read_tim_test.c util.c CodecUtil.c read_tip.c  
read_doc:read_doc.c util.c CodecUtil.c 
	g++ -g -O0  -o read_doc read_doc.c util.c CodecUtil.c 
read_cfe:read_cfe.c  read_cfe_test.c util.c CodecUtil.c 
	g++ -g -O0  -o read_cfe read_cfe.c read_cfe_test.c util.c CodecUtil.c 
t: tim_t.c
	g++ -g -O0 -o t tim_t.c

