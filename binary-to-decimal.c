#include <stdio.h>
#include <stdlib.h>

int main(int argc,  char** argv){
	FILE *fp = fopen(argv[1], "r");
	if (!fp) {printf("File Read Error!\n");return 1;}

	char buf[17];
	while ( fread(buf, 1, sizeof(buf), fp)){
   		int i=0;
   		int ins=0;
   		for (i=0; i<16; i++){
   			printf("%c",buf[i]);
   			ins = ins*2 + (buf[i] - 48);
   		}
   		printf(" -> %u \n",ins);
   	}
   	fclose(fp);
  
  return 0;
}
