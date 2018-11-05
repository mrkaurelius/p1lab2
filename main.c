#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PROMPT_LEN 100
#define PATH_MAX 4096

char * pathKOM();
char * pathDEG();
char * pathDEV();
char * getParams(const char * promptBuf);

void addPrompt(const char * promptBuf);
int getPrompt();

void lowDEG(const char * promptBuf){
	int i;
	char * degPath = pathDEG();
	FILE * DEG;
	//printf("kom path ->\"%s\"\n",degPath );
	DEG = fopen(degPath,"r+");
	if (DEG == NULL) {
		printf("!DEGER.TXT ACILAMADI\n");
		return;
	}
	char * params = getParams(promptBuf);
	int paramSize = 0;
	//for (i = 0; params[i] != '\0'; i++) printf("params -> %c \n",params[i] )
	for (i = 0; params[i] != '\0'; i++) paramSize++;
	//printf("paramsize -> %d\n",paramSize );
	// DOSYAYI ISLEME
	// ANA GIRIS CIKISLARI BUL
}

char * getMainIO(int flag){
	//include edebildikten sonra bununla uğraş
	char * devPath = pathDEV();
	FILE * DEV;
	DEV = fopen(devPath,"r");
	if (DEV == NULL) {
		printf("!ANA GIRIS CIKIS ICIN DEVRE.TXT ACILAMADI");
		return 0;
	}
	if (flag == 1) {
		int i;
		char * ptr = malloc(sizeof(char));

		//GIRISILERI OKU
		return ptr;
	}if (flag == 0) {
		//CIKISICIKISLARI OKU
	}
}

int initDEV(const char * promptBuf){
	// include bitmeden digerlerine gecme
	// DEGER RETURN ETMEK YERINE DEVRE.TXT YI DEGISTIR
	// devre.txt işle ve master.txt de birleştir sonra tmp sonra rename yap (tmp)?
	char * devPath = pathDEV();
	int incFlag = 0;
	FILE * DEV;
	DEV = fopen(devPath,"r");
	if (DEV == NULL) {
		printf("!DEVRE.TXT ACILAMADI\n");
		return 0;
	}else{
		int i;
		printf("DEVRE.TXT OKUMA YAZMA MODUNDA ACILDI\n" );
		// TMP OLUSTUR FSCANF ILE STREAMIN SONUNA KADAR TARA # YORUMLARI ATLA
		// YORUMDAN ARINDIR DOSYAYI BUFFER A AL ? TMP DOSYAYA AL
		FILE * tmpDEV;
		tmpDEV = fopen("tmpdevre.txt","w");
		//foef The C library function int feof(FILE *stream) tests the end-of-file indicator for the given stream.
		while (!feof(DEV)) {
			char buf[100];
			fgets(buf,100,DEV);
			for (i = 0; i < 100 ; i++) {
				if (buf[i] == '#') {
					buf[i - 1] = '\n';
					buf[i] = '\0';
					break;
				}
			}
			if( strstr(buf,".include") != NULL ){
				incFlag++;
				//INCLUDE GEREK VARMI continue;
			}
			fputs(buf,tmpDEV);
			if (strstr(buf,".son") != NULL) {
				printf("buf -> %s\n",buf );
				// ikikere .son yazmasının sebebi ?? (stream feof loop)
				break;
			}
		}
		fclose(tmpDEV);
	}
	//printf("incflag -> %d\n",incFlag );
	if(incFlag){
		FILE * baskaDEV;
		baskaDEV = fopen("baskadevre.txt","r");
	}
	/*

	*/
	fclose(DEV);
	printf("DEVRE.TXT KAPANDI\n" );
	return 1;
}

int main(){
	printf(">>> KAPI SIMULATORU \n" );
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
		case 'l':
		case 'L':
			addPrompt(promptBuffer);
			lowDEG(promptBuffer);
			break;
		default:
		printf("!HATALI GIRDI\n");
	}
	return 0;
}

char * getParams(const char * promptBuf){
	int k = 0,i;
	char * params = malloc(sizeof(char));
	for (i = 1; promptBuf[i] != '\0'; i++) {
		if ((int)promptBuf[i] > 96 && (int)promptBuf[i] < 123 ) {
			params[k] = promptBuf[i];
			k++;
			params = realloc(params,k + 1);
		}
	}
	params[k] = '\0';
	//for (i = 0; i < k; i++) printf("params -> %c \n",params[i] )
	return params;
}

void addPrompt(const char * promptBuf){
	//int i;
	char * komPath = pathKOM();
	FILE * KOM;
	//printf("kom path ->\"%s\"\n",komPath );
	KOM = fopen(komPath,"a");
	if (KOM == NULL) {
		printf("!KOMUT.TXT ACILAMADI\n");
		return;
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
		printf("!DEVRE.TXT KONUMUNA ULASILAMADI\n");
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
