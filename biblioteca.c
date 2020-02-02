//*******************************************************///
//*******************************************************///
//*****Caio Cesar Aguiar Alarcon*************************///
//*****N°7241109*****************************************///
//*******************************************************///
//*******************************************************///
//*******************************************************///
//*******************************************************///

#include "biblioteca.h"

const reg RegLixo = (reg){//Declaração de um registro completo com o caractere definido como lixo
	LIXO,{LIXO,LIXO,LIXO,LIXO},{LIXO,LIXO,LIXO,LIXO},{LIXO,
	LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO},{LIXO,LIXO,LIXO,
	LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO},{LIXO,LIXO,LIXO,
	LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,
	LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,
	LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,
	LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,
	LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO,LIXO}
};

//***FUNCIONALIDADES******************//

//Transfere os registros de um arquivo no formato csv para um arquivo no formato binário.
void csv_to_bin(char * Name){
	char str[MAX_STRING];
	char * fields[5];
	int regCount=0,i=0;
	FILE * fpcsv;
	FILE * fpbin;
	diskpagHeader * header=0;
	diskpagData * pag;
	reg * aux;

	fpcsv = fopen(Name,"r");
	if(!fpcsv){
		printf("Falha no carregamento do arquivo.");
		exit(0);
	}


	fpbin = OpenBinFile("arquivoTrab1si.bin", &header,"wb");

	pag = (diskpagData*)newDiskPage();
	aux = pag->data;

	fgets(str, sizeof str, fpcsv);//Joga fora a primeira linha que é o cabeçalho do csv...

	while (fgets(str, sizeof str, fpcsv)) {//Efetua a leitura do csv linha por linha
		split(str,',',fields);
		aux[regCount].removido = '-';
		*((int*)aux[regCount].encadeamento) = -1;

		if(*fields[0])//Campo nroInscricao
			sscanf(fields[0],"%d",((int*)&aux[regCount].nroInscricao));
		else
			*((int*)aux[regCount].nroInscricao) = -1;

		if(*fields[1])//Campo nota
			sscanf(fields[1],"%lf",(double*)&aux[regCount].nota);
		else{
			*((double*)&aux[regCount].nota) = -1.0;
		}
		if(*fields[2]){//Campo data
			sscanf(fields[2],"%[^\n]s",aux[regCount].data);
			//printf("%d %s",*((int*)&aux[regCount].nroInscricao),aux[regCount].data);
			aux[regCount].data[10]=LIXO;//Devolve o lixo que a função sscanf removeu com o '\0'
		}
		else
			*aux[regCount].data='\0';

		//campos de tamanho variável
		i=0;
		if(*fields[3]>32){//Primeiro campo variável, cidade
			i+=5;
			sscanf(fields[3],"%[^\n]s",&aux[regCount].variavel[i]);//Lê do campo direto pra estrutura
			*((int*)&aux[regCount].variavel[0])=strlen(&aux[regCount].variavel[i])+2;//Guarda o tamanho na estruct
			aux[regCount].variavel[4]='4';
			i += strlen(fields[3])+1;
		}

		if(*fields[4]>32){//Segundao campo variável, nome da escola
			sscanf(fields[4],"%[^\n]s",&aux[regCount].variavel[i+5]);//Le do campo para a estrutura
			*((int*)&aux[regCount].variavel[i])=strlen(&aux[regCount].variavel[i+5])+2;//Tamanho na struct
			aux[regCount].variavel[i+4]='5';
		}

		if(++regCount==200){//Buffer cheio...
			regCount = 0;//Reiniciar o uso do buffer
			//salvar o buffer no disco:
			fwrite(aux,DISK_PAGE_SIZE,1,fpbin);
			//limpa o buffer
			for(i=0;i<200;i++){
				aux[i] = RegLixo;
			}
		}
	}

	fwrite(aux,regCount*80,1,fpbin);//registrar o buffer em disco

	fclose(fpcsv);	//Fecha o arquivo csv
	free(pag);   	//Libera o espaço utilizado
	CloseBinFile(fpbin, header);//Fecha o arquivo binário considerando seu cabeçalho
	printf("arquivoTrab1si.bin");
}

//Exibe todos os registros.
void dump_file(char * binName){
	FILE * fpbin;
	int i,j=0;
	diskpagHeader * _header;
	diskpagData * _data;
	reg * aux;
	_data = (diskpagData*)newDiskPage();
	aux = _data->data;

	fpbin = OpenBinFile(binName,&_header,"rb");
	j++;
	while(fread(_data,DISK_PAGE_SIZE/200,200,fpbin))
		for(i=0,j++;i<200;i++){
			printReg(&aux[i]);
		}

	printf("Número de páginas de disco acessadas: %d",j);
	fclose(fpbin);
	free(_data);   	//Libera o espaço utilizado
}

//Imprime todos os registros a partir de uma busca por campo.
int find_register(char *binName, char * fieldName, char * ToSearch){//Retorna o numero de páginas acessadas
	FILE * fpbin;
	int i,j=0,nreg=0,fim=0;
	char S[MAX_STRING];
	diskpagHeader * _header;
	diskpagData * _data;
	reg * aux;
	_data = (diskpagData*)newDiskPage();
	aux = _data->data;

	fpbin = OpenBinFile(binName,&_header,"rb");
	j++;

	while(!fim && fread(_data,DISK_PAGE_SIZE/200,200,fpbin)){
		j++;
		for(i=0;i<200;i++){
			if(!strcmp(fieldName,"data")){
				sprintf(S,"%.10s",data(&aux[i]));
				if(!strcmp(ToSearch,S)){
					printReg(&aux[i]);
					nreg++;
				}
			}

			if(!strcmp(fieldName,"nroInscricao")){
				sprintf(S,"%d",insc(&aux[i]));
				if(!strcmp(ToSearch,S)){
					printReg(&aux[i]);
					nreg++;
					fim = 1;
				}
			}

			if(!strcmp(fieldName,"nota")){
				sprintf(S,"%.1lf",nota(&aux[i]));
				if(!strcmp(ToSearch,S)){
					printReg(&aux[i]);
					nreg++;
				}
			}

			if(!strcmp(fieldName,"cidade")){
				sprintf(S,"%s",cidade(&aux[i])?cidade(&aux[i]):"");
				if(!strcmp(ToSearch,S)){
					printReg(&aux[i]);
					nreg++;
				}
			}

			if(!strcmp(fieldName,"nomeEscola")){
				if(*ToSearch=='"')
					sprintf(S,"\"%s\"",nome(&aux[i])?nome(&aux[i]):"");
				else
					sprintf(S,"%s",nome(&aux[i])?nome(&aux[i]):"");
				if(!strcmp(ToSearch,S)){
					printReg(&aux[i]);
					nreg++;
				}
			}
		}


	}


	if(nreg)
		printf("Número de páginas de disco acessadas: %d",j);
	else
		printf("Registro inexistente.");
	fclose(fpbin);
	free(_data);
	return j;
}

//Busca um registro baseado em sua posição no arquivo.
int find(char *binName,char * ToSearch){
	FILE * fpbin;
	int j=0,k;
	diskpagHeader * _header;
	diskpagData * _data;
	reg * aux;
	_data = (diskpagData*)newDiskPage();
	aux = _data->data;


	fpbin = OpenBinFile(binName,&_header,"rb");
	j++;

	sscanf(ToSearch,"%d",&k);

	fseek(fpbin,k*DISK_PAGE_SIZE/200,SEEK_CUR);
	j++;
	if(!fread(_data,DISK_PAGE_SIZE/200,1,fpbin))
		printf("Registro inexistente.");
	else{
		printReg(aux);
		printf("Número de páginas de disco acessadas: %d",j);
	}

	fclose(fpbin);
	free(_data);
	return j;
}

//Remove todos os registros a partir de uma busca por campo.
void remove_register(char * binName, char * fieldName, char * ToSearch){
	FILE * fpbin;
	int i,rrn=0,pags=0,pos,fim=0,paglidas;
	char S[MAX_STRING];
	int pagchanged=0;
	diskpagHeader * _header;
	diskpagData * _data;
	reg * aux;
	_data = (diskpagData*)newDiskPage();
	aux = _data->data;

	fpbin = OpenBinFile(binName,&_header,"rb+");
	pags++;

	while(!fim && (paglidas = fread(_data,DISK_PAGE_SIZE/200,200,fpbin))){
		for(i=0;i<200;i++,rrn++){
			if(!strcmp(fieldName,"data")){
				sprintf(S,"\"%.10s\"",data(&aux[i]));
				if(!strcmp(ToSearch,S)){
					removeReg(&aux[i],_header,rrn);
					pagchanged=1;
				}
			}

			if(!strcmp(fieldName,"nroInscricao")){
				sprintf(S,"%d",insc(&aux[i]));
				if(!strcmp(ToSearch,S)){
					removeReg(&aux[i],_header,rrn);
					pagchanged=1;
					fim = 1;//pois o numero de inscrição é único
				}
			}

			if(!strcmp(fieldName,"nota")){
				sprintf(S,"%.1lf",nota(&aux[i]));
				if(!strcmp(ToSearch,S)){
					removeReg(&aux[i],_header,rrn);
					pagchanged=1;
				}
			}

			if(!strcmp(fieldName,"cidade")){
				sprintf(S,"\"%s\"",cidade(&aux[i])?cidade(&aux[i]):"");
				if(!strcmp(ToSearch,S)){
					removeReg(&aux[i],_header,rrn);
					pagchanged=1;
				}
			}

			if(!strcmp(fieldName,"nomeEscola")){
				sprintf(S,"\"%s\"",nome(&aux[i])?nome(&aux[i]):"");
				if(!strcmp(ToSearch,S)){
					removeReg(&aux[i],_header,rrn);
					pagchanged=1;
				}
			}
		}

		if(pagchanged){
			pos = ftell(fpbin);
			//Volta o cursor pro inicio da pág
			fseek(fpbin, -80*paglidas, SEEK_CUR);

			//printf("%d\n", paglidas);
			//Grava em disco a pág modificada
			fwrite((void*)_data,paglidas*80,1,fpbin);

			fseek(fpbin, pos, SEEK_SET);
			pagchanged = 0;
		}
		pags++;
	}

	CloseBinFile(fpbin, _header);
	free(_data);
}
void insert_register(int inscr,double Nota,char * data,char * cidade,char * nomeescola,char * FileName){
	FILE * fpbin;
	int * toprrn;
	int i=0;
	diskpagHeader * _header;
	reg regaux=RegLixo;

	fpbin = OpenBinFile(FileName,&_header,"rb+");

	//buscar o rrn do topo da pilha no header
	toprrn = (int*)_header->topoPilha;//Se for -1 tem que colocar no final do arquivo!
	//printf("%d\n",*toprrn);

	if(*toprrn != -1){
		fseek(fpbin, *toprrn*80+16000, SEEK_SET);	//Colocar o ponteiro no registro
		//acessar pelo rrn o registro excluído que é o topo da pilha

		fread(&regaux,80,1,fpbin);					//Ler o registro

		//guardar o encadeamento do excluído no header pq ele é o novo topo da pilha


		fseek(fpbin, *toprrn*80+16000, SEEK_SET);	//Colocar o ponteiro no registro

		*toprrn = *(int*)regaux.encadeamento;
	}
	else
		fseek(fpbin, 0, SEEK_END);	//Colocar o ponteiro no final do arquivo



	//montar um registro com os dados a serem inseridos
	*(int*)regaux.nroInscricao = inscr;
	*((double*)regaux.nota) = Nota;
	strcpy((char*)regaux.data,data);
	regaux.data[10]=LIXO;


	if(strcmp(cidade,"")){//Se o campo cidade não for vazio
		i+=5;
		strcpy(&regaux.variavel[i],cidade);
		*((int*)&regaux.variavel[0])=strlen(&regaux.variavel[i])+2;//Guarda o tamanho na estruct
		regaux.variavel[4]='4';
		i += strlen(cidade)+1;
	}

	if(strcmp(nomeescola,"")){//se o campo nomeescola não for vazio
		strcpy(&regaux.variavel[i+5],nomeescola);
		*((int*)&regaux.variavel[i])=strlen(&regaux.variavel[i+5])+2;//Tamanho na struct
		regaux.variavel[i+4]='5';
	}

	//gravar no disco
		//Indicar que não está mais excluido
	*(int*)regaux.encadeamento=-1;
	regaux.removido = '-';

	//printReg(&regaux);

	fwrite(&regaux,80,1,fpbin);					//gravar o registro


	CloseBinFile(fpbin, _header);
}
void reg_update(int rrn, char * FileName,char * fieldName,char *  ToSearch){
	FILE * fpbin;
	int i = 0;
	char * aux;
	diskpagHeader * _header;
	reg regbefore=RegLixo;
	reg regafter=RegLixo;
	char auxcidade[MAX_STRING];
	char auxnome[MAX_STRING];
	//abrir o arquivo
	fpbin = OpenBinFile(FileName,&_header,"rb+");

	//posicionar o ponteiro com base no rrn
	fseek(fpbin, rrn*80+16000, SEEK_SET);

	//carregar o registro
	fread(&regbefore,80,1,fpbin);

	//se o registro for vazio, parar a função pois não faz sentido atualizar um registro excluido
	if(regbefore.removido != '-'){
		CloseBinFile(fpbin, _header);
		return;
	}

	aux = cidade(&regbefore);
	strcpy (auxcidade,aux?aux:"");
	aux = nome(&regbefore);
	strcpy (auxnome, aux?aux:"");
	//montar o registro com a alteração solicitada
	regafter = regbefore;

	strcpy(regafter.variavel,"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	regafter.variavel[52]=LIXO;

	if(!strcmp(fieldName,"data")){
		sscanf(ToSearch,"\"%[^\"]",regafter.data);
	}

	if(!strcmp(fieldName,"nroInscricao")){
		sscanf(ToSearch,"%d", (int*)regafter.nroInscricao);
	}

	if(!strcmp(fieldName,"nota")){
		sscanf(ToSearch,"%lf", (double*)regafter.nota);
	}

	if(!strcmp(fieldName,"cidade")){
		if(!strcmp(ToSearch,"NULO"))
			strcpy(auxcidade,"");
		else
			sscanf(ToSearch,"\"%[^\"]\"",auxcidade);
	}

	if(!strcmp(fieldName,"nomeEscola")){
		sscanf(ToSearch,"\"%[^\"]\"",auxnome);
	}

	if(strcmp(auxcidade,"")){//Se o campo cidade não for vazio
		i+=5;
		strcpy(&regafter.variavel[i],auxcidade);
		*((int*)&regafter.variavel[0])=strlen(&regafter.variavel[i])+2;//Guarda o tamanho na estruct
		regafter.variavel[4]='4';
		i += strlen(auxcidade)+1;
	}

	if(strcmp(auxnome,"")){//se o campo nomeescola não for vazio
		strcpy(&regafter.variavel[i+5],auxnome);
		*((int*)&regafter.variavel[i])=strlen(&regafter.variavel[i+5])+2;//Tamanho na struct
		regafter.variavel[i+4]='5';
	}


	//printf("\nantes:\t");
	//printReg(&regbefore);
	//printf("\ndepois:\t");
	//printReg(&regafter);


	//posicionar novamente o ponteiro
	fseek(fpbin, rrn*80+16000, SEEK_SET);


	//gravar o arquivo
	fwrite(&regafter,80,1,fpbin);


	//printf("%d %s %s %s\n", rrn , FileName, fieldName, ToSearch);

	CloseBinFile(fpbin, _header);

}

//Ordena o arquivo e salva em outro arquivo
void sort(char * in, char * out){
	//int i;
	ramfile * arquivo;

	arquivo = FileToRam(in);//Criar clone do arquivo em ram
	Insercao(arquivo->registros,arquivo->regs);//ordenar o arquivo usando função personalizada

	//for(i=0;i < arquivo->regs;i++){
	//	printReg(&(arquivo->registros[i]));
	//}

	RamToFile(out,arquivo);//salvar arquivo

	free(arquivo->registros);
	free(arquivo);
}

//mescla os dois primeiros arquivos no terceiro
void merging(char*file1, char*file2, char*saida){
	int i=0,j=0,k=0,m,n;
	ramfile * f1=NULL;
	ramfile * f2=NULL;
	ramfile f3;
	reg * m1;
	reg * m2;
	reg * m3;
	f1 = FileToRam(file1);
	f2 = FileToRam(file2);

	m3 = (reg*)malloc(sizeof(reg)*(f1->regs + f2->regs));
	f3.registros = m3;
	m1 = f1->registros;
	m2 = f2->registros;
	m = f1->regs;
	n = f2->regs;



	while (j < m || k < n) {
		if(j == m){
			m3[i++] = m2[k++];
			continue;
		}
		if(k == n){
			m3[i++] = m1[j++];
			continue;
		}
		if (*(int*)m1[j].nroInscricao == *(int*)m2[k].nroInscricao) {
			m3[i++] = m1[j++];
			k++;
			continue;
		}
		if (*(int*)m1[j].nroInscricao < *(int*)m2[k].nroInscricao) {
			m3[i++] = m1[j++];
		}else{
			m3[i++] = m2[k++];
		}

	}
	//for(i=0;i < k;i++){
	//	printReg(&(f3.registros[i]));
	//}

	f3.regs = i;
	f3.h = f2->h;

	RamToFile(saida, &f3);

}

//intersecsiona os dois arquivos no terceiro
void matching(char*file1, char*file2, char*saida){
	int i=0,j=0,k=0,m,n;
	ramfile * f1=NULL;
	ramfile * f2=NULL;
	ramfile f3;
	reg * m1;
	reg * m2;
	reg * m3;
	f1 = FileToRam(file1);
	f2 = FileToRam(file2);

	m3 = (reg*)malloc(sizeof(reg)*(f1->regs + f2->regs));
	f3.registros = m3;
	m1 = f1->registros;
	m2 = f2->registros;
	m = f1->regs;
	n = f2->regs;



	while (j < m && k < n) {
		if (*(int*)m1[j].nroInscricao == *(int*)m2[k].nroInscricao) {
			m3[i++] = m1[j++];
			k++;
			continue;
		}
		if (*(int*)m1[j].nroInscricao < *(int*)m2[k].nroInscricao) {
			j++;
		}else{
			k++;
		}

	}
	//for(i=0;i < k;i++){
	//	printReg(&(f3.registros[i]));
	//}

	f3.regs = i;
	f3.h = f2->h;

	RamToFile(saida, &f3);

}

//Cria o arquivo indice
void indexa(char * FileName, char * FileIndex){
	ramfile * file=NULL;
	ramfileindex inFile;
	id * iFile;

	

	char * aux;
	int i,j;
	//abrir arquivo de dados
	//descobrir seu tamanho
	file = FileToRam(FileName);


	//alocar tamanho suficiente pro arquivo índice na ram
	inFile.registros = (id*)malloc_A(32*file->regs);
	inFile.h = (iph*)malloc_A(DISK_PAGE_SIZE);
	inFile.h->status = '1';
	iFile = inFile.registros;
	
	//percorrer o arquivo de dados anotando no vetor de indice a busca e o rrn
	for(i=0,j=0;i<file->regs;i++){
		aux = nome(&file->registros[i]);
		if(aux){
			strcpy(iFile[j].ChaveBusca, aux);
			*(int*)iFile[j++].RRN = i;
		}
	}
	inFile.regs = j;
	*(int*)inFile.h->regs = j;
	SortIndexFile(iFile,j);
	
	//for(i=0;i<j;i++){
	//	printf("%6.0d %s\n",*(int*)iFile[i].RRN,iFile[i].ChaveBusca);
	//}
	
	//ordenar o arquivo indice pela chave de busca usando heapsort
	//salvar no hd o arquivo indice
	RamToIndexFIle(FileIndex,&inFile);
}
//Busca utilizando o arquivo indice
int indexSearch(char * f1, char * f2, char * field, char * search) {//Retorna o número de páginas de disco acessadas.
	ramfileindex * Fi;
	int i,ic=0,dc=0,x; //Variaveis auxiliares e contadores de leituras de disco
	FILE * dadosfp=NULL;
	reg  re;
	diskpagHeader * dataheader;
	
	dadosfp = OpenBinFile(f1,&dataheader,"rb");
	dc++;
	char aux[MAX_STRING];
	//carregar arquivo indice para a ram
	Fi = FileIndexToRam(f2);
	ic = 2;//Duas leituras do cabeçalho
	ic += 32*Fi->regs/16000 ;//Todas as outras leituras
	//buscar string
	for(x=-1,i=0;x<=0;i++){//Percorre o indice
		sprintf(aux,"\"%s\"",Fi->registros[i].ChaveBusca);
		x = strcmp(aux, search);
		if(!x){//se encontrar, buscar rrn encontrado no arquivo de dados
			fseek(dadosfp,DISK_PAGE_SIZE + (*(int*)Fi->registros[i].RRN) * REG_SIZE,SEEK_SET);
			fread(&re,REG_SIZE,1,dadosfp);
			dc++;
			printReg(&re);
		}
	}
	CloseBinFile(dadosfp,dataheader);
		
	
	if(dc>1){
		printf("Número de páginas de disco para carregar o arquivo de índice: %d\n",ic);//Numero de registros pelo tamanho da pág mais leitura e escrita do cabeçalho
		printf("Número de páginas de disco para acessar o arquivo de dados: %d",dc);

	}
	else{
		printf("Registro inexistente.");
	}
	
	return dc;
	
}


//***Funções de manipulação da struct
int insc(reg * r){			//Retorna o numero de inscrição.
	return *(int*)r->nroInscricao;
}
double nota(reg * r){		//Retorna a nota.
	return *(double*)r->nota;
}
char * data(reg * r){		//Regorna a data.
	return r->data;
}
int tc(reg * r){			//Retorna o tamanho da string que guarda o nome da cidade.
	if(r->variavel[4]=='4')
		return *((int*)&r->variavel[0])-2;
	return 0;
}
char * cidade(reg * r){		//Retorna a string que guarda o nome da cidade.
	if(tc(r)==0)return NULL;
	return ((char*)&r->variavel[5]);
}
int tn(reg * r){			//Retorna o tamanho da string que guarda o nome da escola.
    if(r->variavel[4]=='5')
		return *((int*)&r->variavel[0])-2;
	else if(r->variavel[0]==LIXO)return 0;

	if(r->variavel[*((int*)&r->variavel[0])+8]=='5')
		return r->variavel[*((int*)&r->variavel[0])+4]-2;
	return 0 ;
}
char * nome(reg * r){		//Retorna a string que guarda o nome da escola.
	if(tn(r)==0)return NULL;
	if(r->variavel[4]=='5')
		return ((char*)&r->variavel[5]);
    if(r->variavel[*((int*)&r->variavel[0])+8]=='5')
		return (char*)&r->variavel[*((int*)&r->variavel[0])+9];
	return NULL ;
}
void printReg(reg * aux){	//Imprime um registro no formato especificado.
	if(aux->removido!='-'){
		return;
	}
	if(insc(aux))
		printf("%d",insc(aux));

	if(nota(aux) != -1.0)
		printf(" %.1lf",nota(aux));

	if(*data(aux) > 32)
		printf(" %.10s",data(aux));

	if(tc(aux)!= 0){
		printf(" %d",tc(aux));
		printf(" %s",cidade(aux));
	}

	if(tn(aux)!=0){
		printf(" %d",tn(aux));
		printf(" %s",nome(aux));
	}
	printf("\n");
}

void removeReg(reg * REG,diskpagHeader * header,int rrn){//Remove o registro especificado
	int * rrntopo;
	rrntopo = (int*)(header->topoPilha);
	if(REG->removido!='-')
		return;
	*REG = RegLixo; //Preenche o registro com o lixo
	*(int*)REG->encadeamento = *rrntopo;
	REG->removido = '*';
	*rrntopo = rrn;
	//printf("%d\n",rrn);
}

ramfile * FileToRam(char * FileName){
	FILE * fp;
	diskpagHeader * header;

	ramfile * retval;
	retval = (ramfile*)malloc(sizeof(ramfile));
	fp = OpenBinFile(FileName,&header,"rb");					//Abre o arquivo
	retval->h = (diskpagHeader*)malloc(sizeof(diskpagHeader));	//Aloca um espaço
	*retval->h = *header;										//copia o cabeçalho
	retval->regs = (FileLen(fp)-DISK_PAGE_SIZE)/REG_SIZE;		//Encontra o numero de registros
	retval->registros = (reg*)malloc(retval->regs*REG_SIZE);	//Aloca espaço exato para os registros
	fread(retval->registros,REG_SIZE,retval->regs,fp);			//Carrega do disco pra ram os registros
	CloseBinFile(fp,header);
	return retval;
}

ramfileindex * FileIndexToRam(char * FileName){
	FILE * fp;
	iph * header;

	ramfileindex * retval;
	retval = (ramfileindex*)malloc(sizeof(ramfileindex));
	fp = OpenBinFileIndex(FileName,&header,"rb");				//Abre o arquivo
	retval->h = (iph*)malloc(sizeof(iph));						//Aloca um espaço pro cabeçalho
	*retval->h = *header;										//copia o cabeçalho
	

	retval->regs = *(int*)header->regs;									//Encontra o numero de registros
	retval->registros = (id*)malloc(retval->regs*32);			//Aloca espaço exato para os registros
	fread(retval->registros,32,retval->regs,fp);				//Carrega do disco pra ram os registros
	CloseBinFileIndex(fp,header);
	return retval;
}

void RamToFile(char * FileName, ramfile * rf){
	FILE *fp;
	diskpagHeader * header;
	fp = OpenBinFile(FileName,&header,"wb");			//Abre o arquivo
	fwrite(rf->registros,REG_SIZE,rf->regs,fp);			//descarrega da ram pro arquivo
	CloseBinFile(fp,rf->h);
}
void RamToIndexFIle(char * FileName, ramfileindex * rf){
	FILE *fp;
	iph * header;
	fp = OpenBinFileIndex(FileName,&header,"wb");		//Abre o arquivo
	fwrite(rf->registros,32,rf->regs,fp);			//descarrega da ram pro arquivo
	CloseBinFileIndex(fp,rf->h);
}
//Funções auxiliares:
//Divide uma string em várias partes de acordo com um caractere delimitador.
int split(char str[], char delimiter, char ** vet){
	int i,j=0;

	vet[j++]=str;
	for(i=0;i<MAX_STRING && str[i]!='\n';i++){
		if(str[i] == delimiter){
			str[i] = '\0';
			vet[j++]=&str[i+1];
		}
	}
	return j;
}

//Abre o arquivo binário para leitura ou escrita levando em consideração se ele se encontra corrompido.
FILE * OpenBinFile(char str[],diskpagHeader ** header, const char * modo){
	FILE * fpbin;
	*header = newHeader();
	if(modo[0]=='w'){
		fpbin = fopen(str,modo);//Abre o arquivo.bin
		//Aloca e cria o cabeçalho indicando arquivo corrompido
		fwrite(*header,DISK_PAGE_SIZE,1,fpbin);	//Escreve no arquivo
	}else{
		fpbin = fopen(str,modo);
		fread(*header,DISK_PAGE_SIZE,1,fpbin);

		if((*header)->status=='0'){
			printf("Falha no processamento do arquivo.");
			exit(0);
		}
	}
	return fpbin;
}

//Fecha o arquivo binário mas antes altera o cabecalho para não corrompido
void CloseBinFile(FILE * fp, diskpagHeader * header){
	//Altera o cabeçalho do arquivo para não corrompido e fecha o ponteiro
	header->status='1';
	fseek (fp , 0 , SEEK_SET );
	fwrite(header,DISK_PAGE_SIZE,1,fp);
	free(header);
	fclose(fp);
}

FILE * OpenBinFileIndex(char str[],iph ** header, const char * modo){
	FILE * fpbin;
	*header = (iph*)newDiskPage();
	if(modo[0]=='w'){
		fpbin = fopen(str,modo);//Abre o arquivo.bin
		//Aloca e cria o cabeçalho indicando arquivo corrompido
		fwrite(*header,DISK_PAGE_SIZE,1,fpbin);	//Escreve no arquivo
	}else{
		fpbin = fopen(str,modo);
		fread(*header,DISK_PAGE_SIZE,1,fpbin);

		if((*header)->status == '0'){
			printf("Falha no processamento do arquivo.");
			exit(0);
		}
	}
	return fpbin;
}

//Fecha o arquivo binário mas antes altera o cabecalho para não corrompido
void CloseBinFileIndex(FILE * fp, iph * header){
	//Altera o cabeçalho do arquivo para não corrompido e fecha o ponteiro
	header->status='1';
	fseek (fp , 0 , SEEK_SET );
	fwrite(header,DISK_PAGE_SIZE,1,fp);
	free(header);
	fclose(fp);
}

//Aloca e preenche o espaço alocado com o caractere definido em 'define LIXO'.
void * malloc_A(int size){
	void * aux;					 //Variável auxiliar.
	int i;						  //Contador.
	aux = malloc(size);			 //O espaço definido em bytes no argumento size é alocado.
	for(i=0;i<size;i++)			 //Um laço é usado para preencher o espaço alocado com o caractere definido em 'define LIXO'.
		((char*)aux)[i]=LIXO;
	return aux;					 //O endereço do espaço alocado é retornado.
}

//Aloca e retorna o espaço de uma página de disco 'limpa'
void * newDiskPage(){
	return (diskpag*)malloc_A(DISK_PAGE_SIZE);
}

//Cria uma página de disco de cabeçalho já preenchida
diskpagHeader * newHeader(){
	diskpagHeader * buffer;
	buffer = (diskpagHeader*)newDiskPage();

	*((int*)buffer->topoPilha)=-1;


	buffer->status = '0';

	buffer->tagCampo1 = '1';
	strcpy(buffer->desCampo1, "numero de inscricao do participante do ENEM");

	buffer->tagCampo2 = '2';
	strcpy(buffer->desCampo2, "nota do participante do ENEM na prova de matematica");

	buffer->tagCampo3 = '3';
	strcpy(buffer->desCampo3, "data");

	buffer->tagCampo4 = '4';
	strcpy(buffer->desCampo4, "cidade na qual o participante do ENEM mora");

	buffer->tagCampo5 = '5';
	strcpy(buffer->desCampo5, "nome da escola de ensino medio");
	return buffer;
}

//Encontra o tamanho do arquivo baseado no file pointer:
int FileLen(FILE * fp){
	int pos;
	int Lenght;
	pos = ftell(fp);
	fseek(fp, 0,  SEEK_END);	//Colocar o ponteiro no final do arquivo
	Lenght = ftell(fp);
	fseek(fp, pos, SEEK_SET);	//Colocar o ponteiro onde estava
	return Lenght;
}

//Funções de ordenação

void troca(reg * A, reg * B){
	reg aux;
	aux = *A;
	*A = *B;
	*B = aux;
}
void Insercao(reg A[], int n){//Algoritmo de inserção
	int i;
	for (i=0; i<n; i++) {
		int aux=*(int*)A[i].nroInscricao;
		int j=i;
		while (j>0 && aux<*(int*)A[j-1].nroInscricao){//percorre o vetor enquanto houver trocas a fazer
			troca(&A[j], &A[j-1]);
			j--;
		}
	}
}

//Preciso adaptar a comparação e a função troca
void trocai(id * A, id * B){
	id aux;
	aux = *A;
	*A = *B;
	*B = aux;
}

int compID(id A, id B){
	int i;
	i = strcmp(A.ChaveBusca,B.ChaveBusca);
	if(i==0){
		return *(int*)A.RRN > *(int*)B.RRN;
	}
	return i>0;
}
void heapify(id A[], int n, int i){//Função auxiliar do heapsort
	int maior = i;
	int l = 2*i + 1;
	int r = 2*i + 2;
	if (l<n && compID(A[l],A[maior]))		//Se o filho da esquerda for maior que a raiz
		maior = l;
	if (r<n && compID(A[r],A[maior]))		//Se o filho da direita for maior que a raiz
		maior = r;
	if (maior != i){				//Se o maior não for a raiz
		trocai(&A[i], &A[maior]);
		heapify(A, n, maior);		//Cria o heap recursivamente
	}
}
void heapSort(id A[], int n){		//Função HeapSort
	int i;
	for (i=n/2-1; i>=0; i--)		//Cria o heap rearanjando o vetor
		heapify(A, n, i);

	for (i=n-1; i>=0; i--){			//Extrai um por um dos elementos do heap
		trocai(&A[0], &A[i]);		//Move para o fim a raiz atual
		heapify(A, i, 0);			//cria o heap no heap reduzido
	}
}

void SortIndexFile(id * ifile, int max){
	heapSort(ifile,max);
}
