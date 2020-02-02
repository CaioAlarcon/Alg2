//*******************************************************///
//*******************************************************///
//*****Caio Cesar Aguiar Alarcon*************************///
//*****N°7241109*****************************************///
//*******************************************************///
//*******************************************************///
//*******************************************************///
//*******************************************************///

#include "biblioteca.h"
#include <stdlib.h>

//Função sugerida para escrever o binario na tela
void binarioNaTela2(char *nomeArquivoBinario) {

	/* Escolha essa função se você já fechou o ponteiro de arquivo 'FILE *'.
	*  Ela vai abrir de novo para leitura e depois fechar. */

	unsigned char *mb;
	unsigned long i;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela2): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar? Se você não fechou ele, pode usar a outra função, binarioNaTela1, ou pode fechar ele antes de chamar essa função!\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);
	for(i = 0; i < fl; i += sizeof(unsigned char)) {
		printf("%02X ", mb[i]);
		if((i + 1) % 16 == 0)	printf("\n");
	}
	free(mb);
	fclose(fs);
}

void ReadFields(int * inscr, double * Nota, char * Data, char * Cidade, char * NomeEscola){
	char linha[MAX_STRING];
	char aux[MAX_STRING];

	scanf("%d %[^\r\n]",inscr,linha);

	sscanf(linha,"%s %[^\r\n]", aux, linha);//Le a próxima palavra em busca de NULO
	if(!strcmp(aux,"NULO"))//Se foi lido nulo
		*Nota = -1.0;
	else//caso contrário, um double foi lido, então:
		sscanf(aux,"%lf", Nota);

	sscanf(linha,"%s %[^\r\n]", aux, linha);//Faz a leitura da data ou de NULO
	if(!strcmp(aux,"NULO"))//Se foi lido nulo
		*Data = '\0';
	else //caso contrário, algum valor de data foi lido:
		sscanf(aux,"\"%[^\"]", Data);




	sscanf(linha,"%s", aux);//Verifica se é o NULO no campo Cidade, mas sem consumir da entrada
	if(!strcmp(aux,"NULO")){//Se for o nulo
		*Cidade = '\0';
		sscanf(linha,"%s %[^\r\n]", aux, linha);//Consome o nulo
	}
	else{//Caso contrário é necessário ler o valor e enviar para *cidade
		sscanf(linha,"\"%[^\"]\" %[^\r\n]", Cidade, linha);
	}

	sscanf(linha,"%s", aux);//Verifica se é o NULO no campo NomeEscola, mas sem consumir da entrada
	if(!strcmp(aux,"NULO")){//Se for o nulo
		*NomeEscola = '\0';
	}
	else{//Caso contrário é necessário ler o valor e enviar para *NomeEscola
		sscanf(linha,"\"%[^\"]", NomeEscola);
	}

}

int main(int argc, char * argv[]){
	int option, n, i,inscr,rrn;
	char FileName[MAX_STRING];
	char FileName2[MAX_STRING];
	char FileName3[MAX_STRING];
	char fieldName[MAX_STRING];
	char ToSearch[MAX_STRING];
	char NomeEscola[MAX_STRING];
	char Cidade[MAX_STRING];
	char Data[11];//Tem 11 pra caber o \0

	double Nota=-1.0;

	scanf("%d ",&option);//Faz a leitura do número referente à opção.


	switch(option){//Encaminha o fluxo de execução de acordo com a opção selecionada.
		case 1:
			scanf("%s",FileName);
			csv_to_bin(FileName);
		break;
		case 2:
			scanf("%s",FileName);
			dump_file(FileName);
		break;
		case 3:
			scanf("%s",FileName);
			scanf("%s",fieldName);
			scanf(" %[^\r\n]",ToSearch);
			find_register(FileName, fieldName, ToSearch);
		break;
		case 4:
			scanf("%s",FileName);
			scanf(" %s",ToSearch);
			find(FileName, ToSearch);
		break;
		case 5:
			scanf("%s",FileName);
			scanf(" %d",&n);
			for(i=0;i<n;i++){
				scanf("%s",fieldName);
				scanf(" %[^\r\n]",ToSearch);
				remove_register(FileName,fieldName, ToSearch);
			}
			binarioNaTela2(FileName);

		break;
		case 6:
			scanf("%s",FileName);
			scanf(" %d",&n);
			for(i=0;i<n;i++){
				ReadFields(&inscr, &Nota, Data, Cidade, NomeEscola);//Faz a leitura de uma linha
				insert_register(inscr, Nota, Data, Cidade, NomeEscola,FileName);
			}
			binarioNaTela2(FileName);
		break;
		case 7:
			scanf("%s",FileName);
			scanf(" %d",&n);
			for(i=0;i<n;i++){
				scanf("%d %s",&rrn, fieldName);
				scanf(" %[^\r\n]",ToSearch);
				reg_update(rrn,FileName,fieldName, ToSearch);
			}
			binarioNaTela2(FileName);

		break;
		case 8:
			scanf("%s %s",FileName, FileName2);
			sort(FileName, FileName2);//Ordena o arquivo em ram e salva no arquivo FileName2
			binarioNaTela2(FileName2);
		break;

		case 9:
			scanf("%s %s %s",FileName, FileName2, FileName3);
			merging(FileName, FileName2, FileName3);//mescla os arquivos em FileName3
			binarioNaTela2(FileName3);
		break;

		case 10:
			scanf("%s %s %s",FileName, FileName2, FileName3);
			matching(FileName, FileName2, FileName3);//mescla os arquivos em FileName3
			binarioNaTela2(FileName3);
		break;
		case 11:
			scanf("%s %s",FileName, FileName2);
			indexa(FileName, FileName2);//cria o indice em FileName2
			binarioNaTela2(FileName2);
		break;
		case 12:
			scanf("%s %s %s %[^\r\n]",FileName, FileName2, fieldName, ToSearch);
			indexSearch(FileName,FileName2,fieldName,ToSearch);
		break;
		case 13:
			scanf("%s",FileName);
			scanf("%s",FileName2);
			scanf(" %d",&n);
			for(i=0;i<n;i++){
				scanf("%s",fieldName);
				scanf(" %[^\r\n]",ToSearch);
				remove_register(FileName,fieldName, ToSearch);//Remove os registros
			}
			indexa(FileName, FileName2);//Recria o arquivo indice
			binarioNaTela2(FileName2);
		break;
		case 14:
			scanf("%s",FileName);
			scanf("%s",FileName2);
			scanf(" %d",&n);
			for(i=0;i<n;i++){
				ReadFields(&inscr, &Nota, Data, Cidade, NomeEscola);//Faz a leitura de uma linha
				insert_register(inscr, Nota, Data, Cidade, NomeEscola,FileName);
			}
			indexa(FileName, FileName2);//Recria o arquivo indice
			binarioNaTela2(FileName2);
		break;
		case 15:
			scanf("%s %s %s %[^\r\n]",FileName, FileName2, fieldName, ToSearch);
			
			FileIndexToRam(FileName2);//Verifica se o arquivo existe
			printf("*** Realizando a busca sem o auxílio de índice\n");
			i = find_register(FileName, fieldName, ToSearch);
			

			printf("\n\n*** Realizando a busca com o auxílio de um índice secundário fortemente ligado\n");
			i-=indexSearch(FileName,FileName2,fieldName,ToSearch) ;
			printf("\n\nDiferença no número de páginas de disco acessadas: %d",i);
		break;
		default:
		printf("na\n");

	}

	 return 0;
}



