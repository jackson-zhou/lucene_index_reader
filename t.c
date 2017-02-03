#include <string.h>
#include <stdio.h>
int main()
{
	int t = -31;
	char szTxt[] = {0xe6,0xb8,0xa9,0};
	printf("%s\n",szTxt);
	printf("%d\n",(uint32_t)t >> 3);
	printf("====================\n");
	char szAuth[] = "zhoujunsheng";
	for (int i = 0; i < strlen(szAuth);i++) {
		printf("%c ",szAuth[i]);
	}
	printf("\n");

	for (int i = 0; i < strlen(szAuth);i++) {
		printf("%d ",szAuth[i]);
	}
	printf("\n");

	for (int i = 0; i < strlen(szAuth);i++) {
		printf("%02x ",szAuth[i]);
	}
	printf("\n");

}
