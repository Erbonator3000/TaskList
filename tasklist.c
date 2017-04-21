#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define COLOR_GREEN "\033[22;32m"
#define COLOR_YELLOW "\x1b[33;1m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RED "\x1b[31m"
#define COLOR_DEFAULT "\e[m"

#define END_LIB ".c"
#define HELP_TEXT "Options:\n\t-r:\tIs the search recursive\n\t-e:\tdefine the filename extensions to be read\n\t-f:\tspecify files to be read\n\t-l:\tShow line numbers\n\t-h:\tprint this help text\n"

//Global option variables
typedef struct glob_st{
int recursive;
char** end_lib;
char** fileslist;
int files;
int line;
}variable;


void printtasks(char* filename, variable var){
	
	FILE* f=fopen(filename, "r");
	
	if(!f){
		printf(COLOR_RED "opening file %s failed!\n" COLOR_DEFAULT, filename);
		return;
	}

	char line[100]; //TODO variable line length
	int first = 1; //flag for file name print, print only if todos found and for oce only

	int row = 0;
	while((fgets(line, 100, f))!=NULL){
		
		row++;
		
		//TODO excuding strings containing todo (skip over "..." parts)		
		char* task=strstr(line, "//TODO");
		
		
		if(task!=NULL){ //we have a task!!!
			if(first){
				printf(COLOR_BLUE "in file %s\n" COLOR_DEFAULT, filename);
				first=0;
			}
			if(var.line)
				printf("\tLine %i: "COLOR_GREEN "%s" COLOR_DEFAULT, row, task);
			else
				printf("\t"COLOR_GREEN "%s" COLOR_DEFAULT, task);
		}
	}
	fclose(f);
}




int isCompatible(char* comp, char* lib){
	return strstr(lib, comp)!=NULL && strlen(comp)>1;// so . wont be included(is included by default)
}


int isonlist(char** list, int count, char* comp){
	if(list==NULL) //true by default
		return 1;

	for(int i=0; i<count; i++){
		if(strcmp(list[i], comp)==0)
			return 1;
	}
	return 0;
}


void scandirtasks(char* dirName, variable var){
	
	DIR* dir=opendir(dirName);
	
	struct dirent* dp;
	
	//scan first for files only
	while((dp = readdir(dir))!=NULL){
		char* ending=strstr(dp->d_name, "."); //ending of the file name
		if(ending!=NULL &&
				isCompatible(ending, *var.end_lib) && //TODO filename list superior over file extensions
				isonlist(var.fileslist, var.files, dp->d_name)){//if file is ok

			char completeName[strlen(dirName) + strlen(dp->d_name)+2];
			strcpy(completeName,dirName);
			strcat(completeName, "/");
			strcat(completeName, dp->d_name);
			printtasks(completeName, var);
		}	
	}

	if(var.recursive){
		rewinddir(dir);
		while((dp = readdir(dir))!=NULL){
			if(dp->d_type==DT_DIR && dp->d_name[0]!='.'){

				char newdir[strlen(dirName) + strlen(dp->d_name)+2];
				strcpy(newdir,dirName);
				strcat(newdir, "/");
				strcat(newdir, dp->d_name);
				scandirtasks(newdir, var);
			}
		}
	}
	closedir(dir);
}




int main(int argc, char* argv[]){
	
	
	
	char* end_lib=END_LIB;
	variable var={0, &end_lib, NULL, 0, 0};//all options set to default

	//messy code :(
	for(int i=1; i<argc; i++){
		if(argv[i][0]=='-' && argv[i][1]!='-'){//if is option	
			switch(argv[i][1]){
				case 'r': var.recursive=1;
						break;

				case 'e': i++;
						var.end_lib=argv+i;
						break;
				
				case 'f': i++;
						var.fileslist=argv+i; //setting the pointer to first filename pointer and counting how many we got
						while(i<argc && argv[i][0]!='-'){ //loop tropugh following filenames
							var.files++;
							i++;
						}
						break;
				case 'l': var.line=1;
						
						break;
				
				case 'h': printf(HELP_TEXT); //made ose other way please
						return 0;
						break;
			}
		}
	}
	
	scandirtasks(".", var);
	
	return 0;
	
}
