#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define COLOR_GREEN "\033[22;32m"
#define COLOR_DEFAULT "\e[m"


void printTODO(char* filename){
	
	FILE* f=fopen(filename, "r");
	
	if(!f){
		printf("opening file %s failed!", filename);
		return;
	}
	
	char c;
	char t[6];
	char todo[100]; //TODO variable size, dynamic allocing maybe?
	int first = 1; //flag for file name print, print only if todos found and for oce only
	
	while((c=fgetc(f))!=EOF){
		if(c=='/'){
			fgets(t, 6, f);
			if(strcmp(t, "/TODO")==0){ //Todo found!!!
				if(first)
					printf("\nIn file %s:\n", filename);
				first=0;

				fgets(todo, 100,f);
				//TODO row and cloumn numbers
				if(*todo==' ')
					printf(COLOR_GREEN "\t%s" COLOR_DEFAULT,todo+1);//ditching eccess space
				else
					printf(COLOR_GREEN "\t%s" COLOR_DEFAULT,todo+1);
			}
		}		
	}
}


int main(int argc, char* argv[]){
	
	DIR* dir;
	struct dirent* dp;
	
	dir=opendir(".");
	
	printf("Your current tasks in this directory:\n");

	while((dp = readdir(dir))!=NULL){
		if(dp->d_name[strlen(dp->d_name)-1]=='c' && 
			dp->d_name[strlen(dp->d_name)-2]=='.'){
			//printf("%s\n", dp->d_name);
			printTODO(dp->d_name);	
		}		
	}
	closedir(dir);
	return 0;
}




