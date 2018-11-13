#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define PROMPT_LEN 100
#define PATH_MAX 4096

//.kapi <kapı tipi> <giriş sayısı> <çıkış> <giriş listesi><kapının girişlere cevap verme süresi>

struct gate {		//TYPEDEF OLAYINA GIRSEMMI
	char name[5];
	int inpCount;
	char outLine;
	char output_state;
	char inplist[5];
	char inpLine_state[5];
	int lag;
	//int outLine_state;
}loGAtes[10];		 //DINMAIK OLUSTURMA COK MU ZOR OLURDU ? 10 maksimum

struct mainIO{
	char inp[10];
	char out[10];
	int inpCount;
	int outCount;
}mIO;

char * pathKOM();
char * pathDEG();
char * pathDEV();
int getPrompt(int * gateCount);
char * getParams(const char * promptBuf);
void addPrompt(const char * promptBuf);
char * getPath(const char * param);
void getLog(const char * string);
void printStruct(struct gate loGAtes[],int * gateCount);
int parseGate(struct gate loGAtes[],const char * gateString,int * index);
void highDEG(const char * promptBuf,struct gate loGAtes[],int * gateCount);
void initDEG(int * gateCount,struct gate loGAtes[]);
void impGate(const char * gateName,struct gate loGAtes[],int ind,int * gateCount,int zaman);
int initDEV(const char * promptBuf, struct gate loGAtes[],int * gateCount);
char * getMainInp(char * buffer,int inclFlag);
void lowDEG(const char * promptBuf,struct gate loGAtes[],int * gateCount);

void psSimSDEV(char fp1,char fp2,char fp3,int zaman){
	FILE * SDEV;
	SDEV = fopen("simule.txt","a+");
	if (SDEV == NULL) {
		printf("!SIMULE ACILAMADI\n" );
		return;
	} else {
		fprintf(SDEV, "%d.sn %c '%c' -> '%c'\n",zaman,fp1,fp2,fp3 );
		//printf("LOG EKLENDI \n" );
		fclose(SDEV);
	}
}

void simSDEV(){
	FILE * SDEV;
	SDEV = fopen("simule.txt","r");
	if (SDEV == NULL) {
		printf("!SIMULE ACILAMADI\n" );
		return;
	} else {
		char c;
		while ((c = getc(SDEV)) != EOF) {
			printf("%c",c );
		}
	}
}

void showIO(struct gate loGAtes[],const char * buffer,int * gateCount) {

	int i,j,k;
	int gtCount = (int) * gateCount;
	char * params = getParams(buffer);
	int paramSize = 0;
	printf("params->%s\n",params );
	for (i = 0; params[i] != '\0'; i++){
		//printf("params -> %c \n",params[i] );
	}

	//printf("paramsize -> %d\n",paramSize );
	for (i = 0; params[i] != '\0'; i++) paramSize++;
	if (params[0] == '*') {
		char * printArray;
		printArray = malloc(sizeof(char));
		int paSize = 0;
		for (i = 0; i < gtCount; i++) {
			for (j = 0; j < loGAtes[i].inpCount; j++) {
				printArray[paSize] = loGAtes[i].inplist[j];
				paSize++;
				printArray = realloc(printArray,paSize);
			}
			printArray[paSize] = loGAtes[i].outLine;
			paSize++;
			printArray = realloc(printArray,paSize);
		}
		printArray[paSize] = '\0';
		for (i = 0; i < paSize; i++) {
			for (j = i + 1; j < paSize; j++) {
				if (printArray[i] == printArray[j]) {
					printArray[j] = '.';
				}
			}
		}
		printf("printArray ->'%s'\n",printArray );
		for (i = 0; i < gtCount; i++) {
			for (j = 0; j < loGAtes[i].inpCount; j++) {
				for (k = 0; k < paSize; k++) {
					if ( loGAtes[i].inplist[j] == printArray[k]) {
						//printf("DEBUG A" );
						printf("%c -> '%c'\n",loGAtes[i].inplist[j],loGAtes[i].inpLine_state[j] );
						printArray[k] = '.';
						// !!! LOG AL

					}
				}
			}
		}
		for (i = 0; i < gtCount; i++) {
			for (k = 0; k < paSize; k++) {
				if (loGAtes[i].outLine == printArray[k]) {
					//printf("DEBUG B " );
					printf("%c -> '%c'\n",loGAtes[i].outLine,loGAtes[i].output_state );
					printArray[k] = '.';
					// !!! LOG AL

				}
			}
		}
	} else {
		for (i = 0; i < gtCount; i++) {
			for (j = 0; j < loGAtes[i].inpCount; j++) {
				for (k = 0; k < paramSize; k++) {
					if ( loGAtes[i].inplist[j] == params[k]) {
						printf("%c -> '%c'\n",loGAtes[i].inplist[j],loGAtes[i].inpLine_state[j] );
						// !!! LOG AL
						params[k] = '.';
					}
				}
			}
			for (k = 0; k < paramSize; k++) {
				if (loGAtes[i].outLine == params[k]) {
					printf("%c -> '%c'\n",loGAtes[i].outLine,loGAtes[i].output_state );
					// !!! LOG AL
					params[k] = '.';
				}
			}
		}
	}
	return;
}

int main(){
	/*
	!!! KAPILARA EN FAZLA 2 GIRIS ALIYORUZ 3CÜ GIRISI EKLE
	// HER DEGER TXT YI DEGISTIREN OLAYDAN SONRA DEVREYI SIMULE ET (ARKA PLANDA DOSYAYI DEGISTIR)
	//Devre  (Y)(I) komutlarından sonra H, L, S, G, G*, K, C komutları aktif olmalıdır.
	// FONSKYONDAN DEGER DONDURMELER BIRAZ UYDURUK OLDU AMA SKINTI YARATMADI
	//DINAMIK LOGLARI SPRINTF ILE AL
	//FILE DEGISTIRMELERIN LOGUNU AL
	// 2 KERE YUKLEYINCE INP mIO SIKINTIYA GIRIYOR BUNU ILK YORUMU HALLEDERKEN YAPARSIN
	//char * logPath = getPath("/log.txt");
	//printf("%s\n",logPath );
	//printf("outCount -> %d outstr -> %s\n",mIO.outCount,mIO.out ); GEREK VARMI
	//printf("DEBUG GATECOUNT -> %d \n",gateCount );
	*/
	mIO.inpCount = 0;
	mIO.outCount = 0;
	printf(">>> KAPI SIMULATORU \n" );
	int gateCount;
	while (1) {
		if (getPrompt(&gateCount) == 0) break; // DAHA KOLAY IFADE EDILEBILIR
	}
	printStruct(loGAtes,&gateCount);
	printf("inpCount -> %d inpstr -> %s\n",mIO.inpCount,mIO.inp );

	return 0;
}

void impGate(const char * gateName,struct gate loGAtes[],int ind,int * gateCount,int zaman){
	//printf("DEBUG loGAtes[%d] -> %s outline -> %c %c %c\n",ind,loGAtes[ind].name,loGAtes[ind].outLine,loGAtes[ind].inpLine_state[0],loGAtes[ind].inpLine_state[1] );
	char newVal;
	if (!strcmp(gateName,"NAND")) {
		//printf("nand evet nand\n");
		char fp1 = loGAtes[ind].outLine;
		char fp2 = loGAtes[ind].output_state;
		int fp4 = zaman;
		printf("%c '%c' ->> ",loGAtes[ind].outLine,loGAtes[ind].output_state);
		if (loGAtes[ind].inpLine_state[0] == '0' && loGAtes[ind].inpLine_state[1] == '0') {
			loGAtes[ind].output_state = '1';
			newVal = '1';
			//chgDEG(loGAtes[ind].outLine,'1');
		}
		if (loGAtes[ind].inpLine_state[0] == '1' && loGAtes[ind].inpLine_state[1] == '0') {
			loGAtes[ind].output_state = '1';
			newVal = '1';
			//chgDEG(loGAtes[ind].outLine,'1');
		}
		if (loGAtes[ind].inpLine_state[0] == '0' && loGAtes[ind].inpLine_state[1] == '1') {
			loGAtes[ind].output_state = '1';
			newVal = '1';
			//chgDEG(loGAtes[ind].outLine,'1');
		}
		if (loGAtes[ind].inpLine_state[0] == '1' && loGAtes[ind].inpLine_state[1] == '1') {
			loGAtes[ind].output_state = '0';
			newVal = '0';
			//chgDEG(loGAtes[ind].outLine,'0');
		}
		char fp3 = loGAtes[ind].output_state;
		psSimSDEV(fp1,fp2,fp3,fp4);
		printf("'%c'\n",loGAtes[ind].output_state );
	}
	if (!strcmp(gateName,"XOR")) {
		//printf("xor yeah xor\n");
		char fp1 = loGAtes[ind].outLine;
		char fp2 = loGAtes[ind].output_state;
		int fp4 = zaman;
		printf("%c '%c' ->> ",loGAtes[ind].outLine,loGAtes[ind].output_state);
		if (loGAtes[ind].inpLine_state[0] == '0' && loGAtes[ind].inpLine_state[1] == '0') {
			loGAtes[ind].output_state = '0';
			newVal = '0';
			//chgDEG(loGAtes[ind].outLine,'0');
		}
		if (loGAtes[ind].inpLine_state[0] == '1' && loGAtes[ind].inpLine_state[1] == '0') {
			loGAtes[ind].output_state = '1';
			newVal = '1';
			//chgDEG(loGAtes[ind].outLine,'1');
		}
		if (loGAtes[ind].inpLine_state[0] == '0' && loGAtes[ind].inpLine_state[1] == '1') {
			loGAtes[ind].output_state = '1';
			newVal = '1';
			//chgDEG(loGAtes[ind].outLine,'1');
		}
		if (loGAtes[ind].inpLine_state[0] == '1' && loGAtes[ind].inpLine_state[1] == '1') {
			loGAtes[ind].output_state = '0';
			newVal = '0';
			//chgDEG(loGAtes[ind].outLine,'0');
		}
		char fp3 = loGAtes[ind].output_state;
		psSimSDEV(fp1,fp2,fp3,fp4);
		printf("'%c'\n",loGAtes[ind].output_state );
	}
	if (!strcmp(gateName,"NOR")) {
		//printf("nor eved nor\n");
		char fp1 = loGAtes[ind].outLine;
		char fp2 = loGAtes[ind].output_state;
		int fp4 = zaman;
		printf("%c '%c' ->> ",loGAtes[ind].outLine,loGAtes[ind].output_state);
		if (loGAtes[ind].inpLine_state[0] == '0' && loGAtes[ind].inpLine_state[1] == '0') {
			loGAtes[ind].output_state = '1';
			newVal = '1';
			//chgDEG(loGAtes[ind].outLine,'1');
		}
		if (loGAtes[ind].inpLine_state[0] == '1' && loGAtes[ind].inpLine_state[1] == '0') {
			loGAtes[ind].output_state = '0';
			newVal = '0';
			//chgDEG(loGAtes[ind].outLine,'0');
		}
		if (loGAtes[ind].inpLine_state[0] == '0' && loGAtes[ind].inpLine_state[1] == '1') {
			loGAtes[ind].output_state = '0';
			newVal = '0';
			//chgDEG(loGAtes[ind].outLine,'0');
		}
		if (loGAtes[ind].inpLine_state[0] == '1' && loGAtes[ind].inpLine_state[1] == '1') {
			loGAtes[ind].output_state = '0';
			newVal = '0';
			//chgDEG(loGAtes[ind].outLine,'0');
		}
		char fp3 = loGAtes[ind].output_state;
		psSimSDEV(fp1,fp2,fp3,fp4);
		printf("'%c'\n",loGAtes[ind].output_state );
	}

	//printf("DEBUG loGAtes[%d] -> %s %c %c\n",ind,loGAtes[ind].name,loGAtes[ind].inpLine_state[0],loGAtes[ind].inpLine_state[1] );
	// BURDAN ASSAGISI ANLAMSIZ

	int i,j;
	int gtCount = (int)*gateCount;
	char changed = loGAtes[ind].outLine;
	for (i = 0; i < gtCount; i++) {
		for (j = 0; j < loGAtes[i].inpCount; j++) {
			if (loGAtes[i].inplist[j] == changed) {
				loGAtes[i].inpLine_state[j] = newVal;
				//printf("%s degisen girisler %c ->  %c\n",loGAtes[i].name,loGAtes[i].inplist[j],loGAtes[i].inpLine_state[j] );
				//SIMILEDE BUNU GOSTER
			}
		}
	}

	return;
}

void highDEG(const char * promptBuf,struct gate loGAtes[],int * gateCount){
	int i;
	/*
	char * degPath = pathDEG();
	FILE * DEG;
	*/
	//printf("kom path ->\"%s\"\n",degPath );
	char * params = getParams(promptBuf);
	int paramSize = 0;
	for (i = 0; params[i] != '\0'; i++) printf("params -> %c \n",params[i] );
	//printf("paramsize -> %d\n",paramSize );
	for (i = 0; params[i] != '\0'; i++) paramSize++;
	char param[paramSize];
	strcpy(param,params);
	/*
	DEG = fopen(degPath,"r+");
	if (DEG == NULL) {
		printf("!DEGER.TXT ACILAMADI\n");
		getLog("!DEGER.TXT ACILAMADI");
		return;
	} else {
		FILE * tmpDEG;
		tmpDEG = fopen("degertmp.txt","a");
		if (tmpDEG == NULL) {
			printf("!DEGERtmp.TXT ACILAMADI\n");
			getLog("!DEGERtmp.TXT ACILAMADI");
		} else {
			while(!feof(DEG)){ // SON ELEMANI 2 KERE YAZIYOR
				char buf[5];
				int check = 0;
				fgets(buf,5,DEG);
				for (i = 0; i < paramSize; i++) {
					if (params[i] == buf[0]) {
						params[i] = '.';
						check = 1;
						//printf("DEBUG %c -> %c\n",params[i],buf[0] );
						break;
					}
				}
				//printf("buf > \"%s\"\n",buf );
				if (check) {
					buf[2] = '1';
					fprintf(tmpDEG,"%s",buf );
				} else {
					//printf("DEBUG3\n" );
					fprintf(tmpDEG,"%s",buf );
					//fputs(tmpDEG,buf);
				}
				if (fgetc(DEG) == EOF) {
					break;
				} else {
					fseek(DEG,-1,SEEK_CUR); // SAKSES :)
				}
			}
		}
		fclose(tmpDEG);
		fclose(DEG);
		remove("deger.txt");
		rename("degertmp.txt","deger.txt");
	}
	*/
	//initDEG(gateCount,loGAtes);
	//DOSYADA OLUYOR AMA STRUCTTA DEGER DEGISMIYOR YA BIR DAHA INIT EDECEGIZ YADA YENIDEN YUKLEYECEGIZ
	// BURADA DEGISTIRILEN DEGERI STRCUTTTA DEGISTIR (SADECE SN 0 ICIN)

	printf("SIMULE DEAMON START\n" );
	int zaman = 0,j,l,k;
	int gtCount = (int) *gateCount;
	printStruct(loGAtes,gateCount);
	for (i = 0; i < paramSize; i++) {
		for (j = 0; j < gtCount; j++) {
			for (l = 0; l < loGAtes[j].inpCount; l++) {
				if (loGAtes[j].inplist[l] == param[i]) {
					loGAtes[j].inpLine_state[l] = '1'; // BUNU DIGER FONKSYONDA DEGISTIR
					//printf("loGAtes[%d].inpLine_state -> 1 | %c\n",j,loGAtes[j].inplist[l]);
				}
			}
		}
	}
	for (i = 0; i < paramSize; i++) {
		printf("%d.nsde %c 0 -> 1\n",zaman,param[i] );
		psSimSDEV(param[i],'0','1',zaman);
	}
	int flag = 0;
	zaman ++;
	while (zaman < 8) {
		printf("TIME %d\n",zaman );
		for (i = 0; i < gtCount; i++) {
			for (j = 0; j < loGAtes[i].inpCount; j++) {
				for (k = 0; k < mIO.inpCount; k++) {
					if (loGAtes[i].inplist[j] == mIO.inp[k]) {
						flag++;
					}
				}
			}
			if (flag == loGAtes[i].inpCount && loGAtes[i].lag == zaman) {
				printf("loGAtes[%d] %s giris kapısı ve güya irtere oldu \n",i,loGAtes[i].name );
				//impGate(loGAtes[i].name,loGAtes,i,gateCount);
				impGate(loGAtes[i].name,loGAtes,i,gateCount,zaman);
				//const char * gateName,struct gate loGAtes[],int ind,int * gateCount
				// GIRIS KAPISINI ITERE ET
				// birdaha birdaha itere etmek birşey fark ettirmeyecek

			}
			flag = 0;
		}
		flag = 0;

		for (i = 0; i < gtCount; i++) {  // !!! BU YONTEM GİRİSİN HİC DOGRUDAN 2. KADEMEYE BAGLANMADIGINI VAR SAYAR
			for (j = 0; j < loGAtes[i].inpCount; j++) {
				for (k = 0; k < gtCount; k++) {
					if (loGAtes[i].inplist[j] == loGAtes[k].outLine) {
						flag++;
					}
				}
			}
			if (flag > 0) {
				int ii,jj;
				// inputlarının hangilerinin outputları oldukları belli geriye islet ve lagları uygunsa implement et
				// gerideki kapının inputları main inp olana kadar devam et
				// geriden gelen zaman suanki zamana denk ise
				int lastFlag = 0; //!!! BU YONTEM 3 KATMANLI DEVRELERI COZEMEZ
				while (!lastFlag) {
					int gecikme = 0;
					for (ii = 0; ii < gtCount; ii++) {
						for (jj = 0; jj < loGAtes[i].inpCount; jj++) {
							if (loGAtes[ii].outLine == loGAtes[i].inplist[jj]) {
								//printf("loGAtes[%d] %s loGAtes[%d] %s ye baglıdır\n",ii,loGAtes[ii].name,i,loGAtes[i].name );
								gecikme += (loGAtes[ii].lag + loGAtes[i].lag);
								if (zaman == gecikme) {
									printf("loGAtes[%d] %s loGAtes[%d] %s ye baglıdır ve %d zamanda güya birdaha yinelenir\n",ii,loGAtes[ii].name,i,loGAtes[i].name,zaman );
									//const char * gateName,struct gate loGAtes[],int ind,int * gateCount
									impGate(loGAtes[ii].name,loGAtes,ii,gateCount,zaman);//BUNU VERBOSE OLMADAN YAP
									impGate(loGAtes[i].name,loGAtes,i,gateCount,zaman);
								//	impGate(loGAtes[ii].name,loGAtes,ii,gateCount);//BUNU VERBOSE OLMADAN YAP
								//	impGate(loGAtes[i].name,loGAtes,i,gateCount);
									//ONCE ILK SONRA IKINCI KAPIYI YAP YADA SADECE SON KAPIYI YAP BELKI FARK ETMEZ
								}
								lastFlag = 1;
							}
						}
						gecikme = 0;
					}
				}
			}
			flag = 0;
		}
		zaman++;
	}
	printStruct(loGAtes,gateCount);
	return;
}

void lowDEG(const char * promptBuf,struct gate loGAtes[],int * gateCount){
	int i;
	/*
	char * degPath = pathDEG();
	FILE * DEG;
	*/
	//printf("kom path ->\"%s\"\n",degPath );
	char * params = getParams(promptBuf);
	int paramSize = 0;
	for (i = 0; params[i] != '\0'; i++) printf("params -> %c \n",params[i] );
	//printf("paramsize -> %d\n",paramSize );
	for (i = 0; params[i] != '\0'; i++) paramSize++;
	char param[paramSize];
	strcpy(param,params);
	/*
	DEG = fopen(degPath,"r+");
	if (DEG == NULL) {
		printf("!DEGER.TXT ACILAMADI\n");
		getLog("!DEGER.TXT ACILAMADI");
		return;
	} else {
		FILE * tmpDEG;
		tmpDEG = fopen("degertmp.txt","a");
		if (tmpDEG == NULL) {
			printf("!DEGERtmp.TXT ACILAMADI\n");
			getLog("!DEGERtmp.TXT ACILAMADI");
		} else {
			while(!feof(DEG)){ // SON ELEMANI 2 KERE YAZIYOR
				char buf[5];
				int check = 0;
				fgets(buf,5,DEG);
				for (i = 0; i < paramSize; i++) {
					if (params[i] == buf[0]) {
						params[i] = '.';
						check = 1;
						//printf("DEBUG %c -> %c\n",params[i],buf[0] );
						break;
					}
				}
				//printf("buf > \"%s\"\n",buf );
				if (check) {
					buf[2] = '1';
					fprintf(tmpDEG,"%s",buf );
				} else {
					//printf("DEBUG3\n" );
					fprintf(tmpDEG,"%s",buf );
					//fputs(tmpDEG,buf);
				}
				if (fgetc(DEG) == EOF) {
					break;
				} else {
					fseek(DEG,-1,SEEK_CUR); // SAKSES :)
				}
			}
		}
		fclose(tmpDEG);
		fclose(DEG);
		remove("deger.txt");
		rename("degertmp.txt","deger.txt");
	}
	*/
	//initDEG(gateCount,loGAtes);
	//DOSYADA OLUYOR AMA STRUCTTA DEGER DEGISMIYOR YA BIR DAHA INIT EDECEGIZ YADA YENIDEN YUKLEYECEGIZ
	// BURADA DEGISTIRILEN DEGERI STRCUTTTA DEGISTIR (SADECE SN 0 ICIN)

	printf("SIMULE DEAMON START\n" );
	int zaman = 0,j,l,k;
	int gtCount = (int) *gateCount;
	printStruct(loGAtes,gateCount);
	for (i = 0; i < paramSize; i++) {
		for (j = 0; j < gtCount; j++) {
			for (l = 0; l < loGAtes[j].inpCount; l++) {
				if (loGAtes[j].inplist[l] == param[i]) {
					loGAtes[j].inpLine_state[l] = '0'; // BUNU DIGER FONKSYONDA DEGISTIR
					//printf("loGAtes[%d].inpLine_state -> 1 | %c\n",j,loGAtes[j].inplist[l]);
				}
			}
		}
	}
	for (i = 0; i < paramSize; i++) {
		printf("%d.nsde %c 1 -> 0\n",zaman,param[i] ); // DOGRU KULLANIM VAR  SAYILIYOR
		psSimSDEV(param[i],'1','0',zaman);
	}
	int flag = 0;
	zaman ++;
	while (zaman < 8) {
		printf("TIME %d\n",zaman );
		for (i = 0; i < gtCount; i++) {
			for (j = 0; j < loGAtes[i].inpCount; j++) {
				for (k = 0; k < mIO.inpCount; k++) {
					if (loGAtes[i].inplist[j] == mIO.inp[k]) {
						flag++;
					}
				}
			}
			if (flag == loGAtes[i].inpCount && loGAtes[i].lag == zaman) {
				printf("loGAtes[%d] %s giris kapısı ve güya irtere oldu \n",i,loGAtes[i].name );
				//impGate(loGAtes[i].name,loGAtes,i,gateCount);
				impGate(loGAtes[i].name,loGAtes,i,gateCount,zaman);
				//const char * gateName,struct gate loGAtes[],int ind,int * gateCount
				// GIRIS KAPISINI ITERE ET
				// birdaha birdaha itere etmek birşey fark ettirmeyecek

			}
			flag = 0;
		}
		flag = 0;

		for (i = 0; i < gtCount; i++) {  // !!! BU YONTEM GİRİSİN HİC DOGRUDAN 2. KADEMEYE BAGLANMADIGINI VAR SAYAR
			for (j = 0; j < loGAtes[i].inpCount; j++) {
				for (k = 0; k < gtCount; k++) {
					if (loGAtes[i].inplist[j] == loGAtes[k].outLine) {
						flag++;
					}
				}
			}
			if (flag > 0) {
				int ii,jj;
				// inputlarının hangilerinin outputları oldukları belli geriye islet ve lagları uygunsa implement et
				// gerideki kapının inputları main inp olana kadar devam et
				// geriden gelen zaman suanki zamana denk ise
				int lastFlag = 0; //!!! BU YONTEM 3 KATMANLI DEVRELERI COZEMEZ
				while (!lastFlag) {
					int gecikme = 0;
					for (ii = 0; ii < gtCount; ii++) {
						for (jj = 0; jj < loGAtes[i].inpCount; jj++) {
							if (loGAtes[ii].outLine == loGAtes[i].inplist[jj]) {
								//printf("loGAtes[%d] %s loGAtes[%d] %s ye baglıdır\n",ii,loGAtes[ii].name,i,loGAtes[i].name );
								gecikme += (loGAtes[ii].lag + loGAtes[i].lag);
								if (zaman == gecikme) {
									printf("loGAtes[%d] %s loGAtes[%d] %s ye baglıdır ve %d zamanda güya birdaha yinelenir\n",ii,loGAtes[ii].name,i,loGAtes[i].name,zaman );
									//const char * gateName,struct gate loGAtes[],int ind,int * gateCount
									impGate(loGAtes[ii].name,loGAtes,ii,gateCount,zaman);//BUNU VERBOSE OLMADAN YAP
									impGate(loGAtes[i].name,loGAtes,i,gateCount,zaman);
								//	impGate(loGAtes[ii].name,loGAtes,ii,gateCount);//BUNU VERBOSE OLMADAN YAP
								//	impGate(loGAtes[i].name,loGAtes,i,gateCount);
									//ONCE ILK SONRA IKINCI KAPIYI YAP YADA SADECE SON KAPIYI YAP BELKI FARK ETMEZ
								}
								lastFlag = 1;
							}
						}
						gecikme = 0;
					}
				}
			}
			flag = 0;
		}
		zaman++;
	}
	printStruct(loGAtes,gateCount);
	return;
}

void initDEG(int * gateCount,struct gate loGAtes[]){
	int gtCount = (int)*gateCount;
	int i,j,k=0,l;
	char * degPath = pathDEG();
	FILE * DEG;
	DEG = fopen(degPath,"r");
	if (DEG == NULL) {
		printf("!DEGER.TXT ACILAMADI\n");
		getLog("!DEGER.TXT ACILAMADI");
		return;
	} else {
		char * degNarray = malloc(sizeof(char));
		char * degDarray = malloc(sizeof(char));
		char buf[5];
		while (!feof(DEG)) {
			if (fgetc(DEG) == EOF) {
				break;
			} else {
				fseek(DEG,-1,SEEK_CUR); // SAKSES :)
			}
			fgets(buf,5,DEG);
			degNarray[k] = buf[0];
			degDarray[k] = buf[2];
			k++;
			degNarray = realloc(degNarray,k);
			degDarray = realloc(degDarray,k);
		}
		// !!! BUFFER OVERFLOW VARDI BIRDAHA OLABILIR DIKKAT ET
		degNarray[k] = '\0';
		degDarray[k] = '\0';
		printf("degNarray ->'%s' degDarray -> '%s' \n",degNarray,degDarray );
		for (i = 0; i < gtCount; i++) {
			//printf("DEBUG\n" );
			for (j = 0; loGAtes[i].inplist[j] != '\0'; j++) {
				for (l = 0; l < k; l++) {
					if (loGAtes[i].inplist[j] == degNarray[l]) {
						loGAtes[i].inpLine_state[j] = degDarray[l];
					}
				}
			}
		}
		for (i = 0; i < gtCount; i++) {
			//impGate(loGAtes[i].name,loGAtes,i);
		}
		/// !!! CIKIS DEGERKERİNİ İNİT ET
		for (i = 0; i < gtCount; i++) {
			for (j = 0; j < k; j++) {
				if (loGAtes[i].outLine == degNarray[j]) {
					loGAtes[i].output_state = degDarray[j];
				}
			}
		}
		printf("INIT BITTI\n" );
		printStruct(loGAtes,gateCount);
	}
	return;
}

int initDEV(const char * promptBuf, struct gate loGAtes[],int * gateCount){
	// SONUNA INITI ALDIGINI BELIRTEN PRINTF I KOY bel
	// DEVRELERI BIRLESTIRMEYE GEREK VARMI SONCTA STRUCTTA ATACAGIZ ?
	// !!! PROMPT BUFF KULLANILMIYOR KOMUT SATIRINDAN GIRILEN DEVRENIN FOPEN OLMASI LAZIM
	//printf("%s\n",getParams(promptBuf));
	*gateCount = 0;
	int incFlag = 0,i;
	char * devPath = pathDEV();
	FILE * DEV;
	DEV = fopen(devPath,"r");
	if (DEV == NULL) {
		printf("!DEVRE.TXT ACILAMADI\n");
		getLog("!DEVRE.TXT ACILAMADI");
		return 0;
	}else{
		int i;
		printf("DEVRE.TXT OKUMA YAZMA MODUNDA ACILDI\n" );
		getLog("DEVRE.TXT OKUMA YAZMA MODUNDA ACILDI");
		//foef The C library function int feof(FILE *stream) tests the end-of-file indicator for the given stream.
		//	FILE * tmpDEV;
		//	tmpDEV = fopen("tmpdevre.txt","w");
		char tmpArg1[50];
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
			}
			if( strstr(buf,".kapi") != NULL ){
				//printf("KAPI BULUNDU DEBUG ->%d. KAPI\n",*gateCount);
				//getLog("KAPI BULUNDU");//BUNU DUZELT
				parseGate(loGAtes,buf,gateCount);
				(*gateCount)++;
			}
			if( (strstr(buf,".giris") != NULL)){
				getMainInp(buf,0);
			}
			if((strstr(buf,".cikis") != NULL)){
				strcpy(tmpArg1,buf);

			}
			//fputs(buf,tmpDEV);
			if (strstr(buf,".son") != NULL) {
				//printf("buf -> %s\n",buf );
				// LOOPTA HATA OLABILIR FEOF
				break;
			}
		}
		//getMainOut(tmpArg1,0);
		//fclose(tmpDEV);
	}
	//printf("incflag -> %d\n",incFlag );
	if(incFlag == 1){
		FILE * baskaDEV;
		baskaDEV = fopen("baskadevre.txt","r");
		if (baskaDEV == NULL) {
			printf("!BASKADEVRE.TXT ACILAMADI\n");
			getLog("!BASKADEVRE.TXT ACILAMADI");
		} else {
		// KAPI SAYISINI BUL ONA GORE STRUCT CAGIR ?
		printf("BASKADEVRE.TXT ACILDI\n");
		getLog("BASKADEVRE.TXT ACILDI");
		//char tmpArg2[50];
		while (!feof(baskaDEV)) {
			char buf[100];
			fgets(buf,100,baskaDEV);
			for (i = 0; i < 100 ; i++) {
				if (buf[i] == '#' || buf[i] == '\v' ) { //TEST ET
					buf[i - 1] = '\n';
					buf[i] = '\0';
					break;
				}
			}
			//printf("baska dev buf -> %s\n",buf );
			if( strstr(buf,".kapi") != NULL ){
				for (i = 0; i < 100; i++) {
					if (buf[i] == 13) {
						//printf("DEBUG v buldu\n" ); FORMAT ICIN
						buf[i + 1] = '\0';
						break;
					}
				}
				//printf("KAPI BULUNDU DEBUG ->%d. KAPI\n",*gateCount);
				//getLog("KAPI BULUNDU");
				parseGate(loGAtes,buf,gateCount);
				(*gateCount)++;
			}
			if( (strstr(buf,".giris") != NULL)){
				getMainInp(buf,1);
			}
			if((strstr(buf,".cikis") != NULL)){
				//strcpy(tmpArg2,buf);
				//getMainOut(buf,1);
			}
			if( strstr(buf,".son") != NULL ){
				break;
				}
			}
		//getMainOut(tmpArg2,1);
		fclose(baskaDEV);
		}
	}
	//printf("KAPI SAYISI: %d\n",gateCount ); BUNU DINAMIK SEKILDE FONKSYONA ATAMA
	fclose(DEV);
	printf("DEVRE.TXT KAPANDI\n" );
	getLog("DEVRE.TXT KAPANDI");
	if ((*gateCount) != 0) {
		return 1;
	} else {
		printf("DEVREDE KAPI BOLUNAMADI\n" );
		getLog("DEVREDE KAPI BOLUNAMADI");
		return 0;
	}
}

int getPrompt(int * gateCount){
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
	//printf("alınan komut -> \"%s\" \n",promptBuffer );
	char prompt = promptBuffer[0];
	switch (prompt) {
		case 'i':
		case 'I':
			addPrompt(promptBuffer);
			getLog(promptBuffer);
			initDEG(gateCount,loGAtes);
			break;
		case 'y':
		case 'Y':
			addPrompt(promptBuffer);
			getLog(promptBuffer);
			initDEV(promptBuffer,loGAtes,gateCount);
			break;
		case 'G':
		case 'g':
			showIO(loGAtes,promptBuffer,gateCount);
			getLog(promptBuffer);
			break;
		case 's':
		case 'S':
			simSDEV();
			getLog(promptBuffer);
			//initDEV(promptBuffer,loGAtes,gateCount); !!! BUNU NEDEN BRAYA KOYMUSUM ??
			break;
		case 'l':
		case 'L':
			addPrompt(promptBuffer);
			getLog(promptBuffer);
			lowDEG(promptBuffer,loGAtes,gateCount);
			break;
		case 'h':
		case 'H':
			addPrompt(promptBuffer);
			getLog(promptBuffer);
			highDEG(promptBuffer,loGAtes,gateCount);
			break;
		case 'e':
		case 'E':
			addPrompt(promptBuffer);
			getLog(promptBuffer);
			return 0;
			break;
		default:
		printf("!HATALI GIRDI\n");
		getLog(promptBuffer);
		getLog("!HATALI GIRDI");
	}
	//gateCount = initDEV(promptBuffer); // COK VERIMLI OLMADI AMA OLDU
	return 1;
}

char * getMainInp(char * buffer,int inclFlag){ // TIPI DEGISTIR
	// BASKADEVRED DEVREDE OLMAYAN ANA GIRIS VARSA ANA GIRISI EKLE
	int i,j;
	/*
	for (i = 0; buffer[i] != '\0'; i++) {
		printf("%d - ",i);
		printf("%3d",buffer[i]);
		if (isprint(buffer[i])) {
			printf(" %c ",buffer[i] );
		}
		printf("\n" );
		bufSize++;
	}
	*/
	if (inclFlag) {
		for (i = 5; buffer[i] != '\0'; i++) {
			if ( buffer[i] == 9) {
				for (j = 0; j < mIO.inpCount; j++) {
					if (buffer[i + 1] == mIO.inp[j] ) {
						buffer[i + 1] = '.';
					}
				}
			}
		}
		//printf("buf -> %s\n",buffer );
		for (i = 7; buffer[i] != '\0'; i++) {
			if (isalpha(buffer[i])) {
				mIO.inp[mIO.inpCount] = buffer[i];
				mIO.inpCount++;
			}
		}
		//return 0;
	} else {
		for (i = 5; buffer[i] != '\0'; i++) {
			if ( buffer[i] == 9) {
				mIO.inp[mIO.inpCount] = buffer[i + 1];
				//printf("%d %c\n",mIO.inpCount,mIO.inp[mIO.inpCount] );
				//printf("-> %c ",mIO.inp[mIO.inpCount] );
				mIO.inpCount++;
			}
		}
		printf("\n" );
	}
	return 0;
}

char * getParams(const char * promptBuf){
	int k = 0,i;
	char * params = malloc(sizeof(char));
	for (i = 1; promptBuf[i] != '\0'; i++) {
		if (((int)promptBuf[i] > 96 && (int)promptBuf[i] < 123) || promptBuf[i] == '*' ) {
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
		getLog("!KOMUT.TXT ACILAMADI");
		return;
	}
	fputs(promptBuf,KOM);
	fputc('\n',KOM);
	printf("KOMUT KOMUT.TXT EKLENDI\n" );
	getLog("KOMUT KOMUT.TXT EKLENDI");
	fclose(KOM);
	return;
}

void getLog(const char * string){
	time_t rawtime;
    struct tm *info;
    char buffer[80];
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(buffer,80,"%x - %I:%M%p", info);

	FILE * LOG;
	LOG = fopen(getPath("/log.txt"),"a");
	if (LOG == NULL) {
		printf("!LOG ACILAMADI\n" );
		return;
	} else {
		fprintf(LOG, "%s %s\n",buffer,string );
		//printf("LOG EKLENDI \n" );
		fclose(LOG);
	}
	return;
}

int parseGate(struct gate loGAtes[],const char * gateString,int * index){
	int i,gsSize=0,k=0;
	char nulls[6],tmpname[5];
	char buf[50];
	strcpy(buf,gateString);
	int ind = (int)(*index);
	//printf(" index -> %d\nGstr -> /%s/ ",ind,gateString );
	//printf("newbuf -> %s\n",newbuf );
	//printf("gsSize -> %d\n",gsSize );
	for (i = 0; buf[i] != '\0'; i++) {
		//printf("%d - ",i);
		//printf("%3d",buf[i]);
		if (isprint(buf[i])) {
			//printf(" %c ",buf[i] );
		}
		//printf("\n" );
		gsSize++;
	}
	char newbuf[gsSize - 12];
	int formtCheck = 0;
	for (i = 10; i < gsSize - 3; i++) {
		if (!formtCheck) {
			if (buf[i] == 9 && i == 10) {
				formtCheck = 1;
				continue;
			}
		}
		newbuf[k] = buf[i];
		k++;
	}
	newbuf[k] = '\0';
	//printf("sizeof(newbuf) -> %d \n",sizeof(newbuf));
	for (i = 0; i < k + 1 ; i++) {
		//printf("%d - ",i);
		//printf("%3d",newbuf[i]);
		//if (isprint(newbuf[i])) printf(" %c ",newbuf[i] );
		//printf("\n" );
	}
	k = 0;
	for (i = 3 ; newbuf[i] != '\0' ; i++) {
		if (isalpha(newbuf[i])) {
			loGAtes[ind].inplist[k] = newbuf[i];
			//printf("inplist %d. -> %c\n",k,loGAtes[ind].inplist[k]);
			k++;
		}
	}
	loGAtes[ind].inplist[k + 1] = '\0';
	sscanf(buf,"%s %s",nulls,tmpname);
	loGAtes[ind].lag = (int)buf[gsSize - 2] - 48; //ascii magic
	loGAtes[ind].inpCount = (int)(newbuf[0] - 48);
	loGAtes[ind].outLine = newbuf[2];
	strcpy(loGAtes[ind].name,tmpname);
	return 0;
}

void printStruct(struct gate loGAtes[],int * gateCount){
	int i,k;
	int gtSize =(int) *gateCount;
	for (i = 0; i < gtSize; i++) {
		printf(
			"loGAte[%d] ,lag -> %d,name -> %s,inpCnt -> %d,cıkıs -> %c,cıkısDeg ->  %c, inpLstate -> %s\n"
			,i,loGAtes[i].lag,loGAtes[i].name,loGAtes[i].inpCount,loGAtes[i].outLine,loGAtes[i].output_state,loGAtes[i].inpLine_state);
		char tmp[100];
		sprintf(tmp,"loGAte[%d] ,lag -> %d,name -> %s,inpCount -> %d,outLine -> %c",i,loGAtes[i].lag,loGAtes[i].name,loGAtes[i].inpCount,loGAtes[i].outLine );
		getLog(tmp);
		for (k = 0; loGAtes[i].inplist[k] != '\0'; k++) {
			printf("%d. inplistel -> %c\n",k,loGAtes[i].inplist[k] );
			char tmp1[100];
			sprintf(tmp1,"%d. inplistel -> %c",k,loGAtes[i].inplist[k]);
			getLog(tmp1);
		}
		printf("\n" );
	}
}

char * getPath(const char * param){
	char cwd[PATH_MAX];
	if (getcwd(cwd,sizeof(cwd)) == 	NULL) {
		//printf("!DEVRE.TXT KONUMUNA ULASILAMADI\n"); DINAMIK SEKILDE YAZDIRMA FONKSYONA GONDERMEDEN
		//getLog("!DEVRE.TXT KONUMUNA ULASILAMADI");
	}else{
		strcat(cwd,param);
	}
	//printf("devre.txt yolu -> %s\n",cwd );
	char * ptr = cwd;
	return ptr;
}

char * pathDEV(){
	char cwd[PATH_MAX];
	if (getcwd(cwd,sizeof(cwd)) == 	NULL) {
		printf("!DEVRE.TXT KONUMUNA ULASILAMADI\n");
		getLog("!DEVRE.TXT KONUMUNA ULASILAMADI");
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
		printf("!DEGER.TXT KONUMUNA ULASILAMADI\n");
		getLog("!DEGER.TXT KONUMUNA ULASILAMADI");
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
		printf("!KOMUT.TXT KONUMUNA ULASILAMADI\n");
		getLog("!KOMUT.TXT KONUMUNA ULASILAMADI");
	}else{
		strcat(cwd,"/komut.txt");
	}
	//printf("kumut.txt yolu -> %s\n",cwd );
	char * ptr = cwd;
	return ptr;
}
