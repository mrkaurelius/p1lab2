#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PROMPT_LEN 100
#define PATH_MAX 4096

char * pathKOM();
char * pathDEG();
char * pathDEV();
void addPrompt(const char * promptBuf);
int getPrompt();

int initDEV(const char * promptBuf){
	char * devPath = pathDEV();
	FILE * DEV;
	DEV = fopen(devPath,"r");
	if (DEV == NULL) {
		printf("!DEVRE.TXT ACILAMADI\n");
		return 0;
	}else{
		printf("DEVRE.TXT ACILDI\n" );
	}
	printf("DEVRE.TXT KAPANDI\n" );
	fclose(DEV);
	return 1;
}
/*
		H VE L KOMUTLARINI IMPEMENT ET
*/
int main(){
	int i;
	getPrompt();
	getPrompt();
	return 0;
}

int getPrompt(){
	int i;
	char promptBuffer[PROMPT_LEN];
	printf("> " );
	fgets(promptBuffer,PROMPT_LEN,stdin);
	for (i = 0; i < PROMPT_LEN; i++) {
		if (promptBuffer[i] == '\n') {
			promptBuffer[i] = '\0';
		}if (promptBuffer[i] == '#') {
			promptBuffer[i] = '\0';
		}
	}

	printf("alınan komut -> \"%s\" \n",promptBuffer );
	char prompt = promptBuffer[0];
	switch (prompt) {
		case 'y':
		case 'Y':
		//printf("y veya Y ye basıldı\n" );
		addPrompt(promptBuffer);
		initDEV(promptBuffer);
		break;
		default:
		printf("!HATALI GIRDI\n");
	}
	return 0;
}

void addPrompt(const char * promptBuf){
	//int i;
	char * komPath = pathKOM();
	FILE * KOM;
	//printf("kom path ->\"%s\"\n",komPath );
	KOM = fopen(komPath,"a");
	if (KOM == NULL) {
		printf("!KOM.TXT ACILAMADI\n");
		return;
	}else{
		//printf("KOM.TXT ACILDI\n" );
	}
	fputs(promptBuf,KOM);
	fputc('\n',KOM);
	printf("KOMUT KOMUT.TXT EKLENDI\n" );
	fclose(KOM);
	return;
}

char * pathDEV(){
	char cwd[PATH_MAX];
	if (getcwd(cwd,sizeof(cwd)) == 	NULL) {
		printf("> devre.txt konuma ulaşılamadı\n");
	}else{
		strcat(cwd,"/devre.txt");
	}
	//printf("devre.txt yolu -> %s\n",cwd );
	char * ptr = cwd;
	return ptr;
}

char * pathDEG(){
	char cwd[PATH_MAX];
	if (getcwd(cwd,sizeof(cwd)) == 	NULL) {
		printf("> devre.txt konuma ulaşılamadı\n");
	}else{
		strcat(cwd,"/deger.txt");
	}
	//printf("deger.txt yolu -> %s\n",cwd );
	char * ptr = cwd;
	return ptr;
}

char * pathKOM(){
	char cwd[PATH_MAX];
	if (getcwd(cwd,sizeof(cwd)) == 	NULL) {
		printf("> devre.txt konuma ulaşılamadı\n");
	}else{
		strcat(cwd,"/komut.txt");
	}
	//printf("kumut.txt yolu -> %s\n",cwd );
	char * ptr = cwd;
	return ptr;
}
