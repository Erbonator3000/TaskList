#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define COLOR_GREEN "\033[22;32m"
#define COLOR_YELLOW "\x1b[33;1m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RED "\x1b[31m"
#define COLOR_DEFAULT "\e[m"

//minimum line length alloceted at once
#define MIN_LENGTH 100

#define END_LIB ".c"
#define HELP_TEXT "Usage:\t tasklist <args>\n\nOptions:\n\t-r:\tIs the search recursive\n\t-e:\tdefine the filename extensions to be read\n\t-f:\tspecify files to be read\n\t-l:\tShow line numbers\n\t-h:\tprint this help text\n"

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

	char* line=malloc(MIN_LENGTH*sizeof(char));
	line[0]=0; //making sure it is empty
	
	int first = 1; //flag for file name print, print only if todos found and for once only
	int row = 0; //counting for line number

	while((fgets(line+strlen(line), MIN_LENGTH, f))!=NULL){
		
		if(strstr(line, "\n")==NULL){ //if the whole line wasn't captured
			line=realloc(line, (strlen(line)+MIN_LENGTH)*sizeof(char)); //reserve more space...
			continue; //...and read more
		}
		
		row++;
		
		//TODO excluding todos inside of a string?

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

		line=realloc(line, MIN_LENGTH*sizeof(char));
		line[0]=0;//remove earlier string
		
	}
	free(line);
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
		//printf("%s\n", ending);

		//This is messy but hang on... is specified file or match the ending or no spesification and match the default
		if(dp->d_name[0]!='.' && ( //don't include hidden filel //TODO make hidden files option? 
				(var.files!=0 && isonlist(var.fileslist, var.files, dp->d_name)) || //most fundamental rule
				(ending!=NULL && ( //ending is fine
					(var.files==0 && var.end_lib==NULL && isCompatible(END_LIB, ending)) || //no ending specified(default ending)
					(var.end_lib!=NULL && isCompatible(*var.end_lib, ending))) //ending specified
				)
			) 
		){
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
	
	
	
	variable var={0, NULL, NULL, 0, 0};//all options set to default

	//messy code :(
	for(int i=1; i<argc; i++){
		if(argv[i][0]=='-' && argv[i][1]!='-'){//if is option	
			switch(argv[i][1]){
				case 'r': var.recursive=1;
						break;

				case 'e': i++;
						if(i>=argc){
							printf(HELP_TEXT);
							return 0;
						}
						var.end_lib=argv+i;
						break;
				
				case 'f': i++;
						if(i>=argc){
							printf(HELP_TEXT);
							return 0;
						}
						var.fileslist=argv+i; //setting the pointer to first filename pointer and counting how many we got
						for(int j=0; j+i<argc && argv[i][0]!='-'; j++){ //loop tropugh following filenames
							var.files++;
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
