#include "read_cfe.h"
#include <string.h>
int main()
{
	char fn [NAME_MAX+1];
	strcpy(fn, "index_test/_0.cfe");
	readCfe(fn);
	return 0;
}
