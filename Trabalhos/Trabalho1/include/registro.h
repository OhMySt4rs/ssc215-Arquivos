#ifndef H_REGISTRO_
#define H_REGISTRO_

//Códigos de controle

#define SUCESSO 0
#define ERRO 1
#define regDeletado 2


typedef struct registroCabecalho CABECALHO;
typedef struct registro REGISTRO;

int criaBinario(FILE*, FILE*);          // Cria um arquivo binário com base no arquivo fonte
REGISTRO lerRegistro(FILE*);                 // Ler o registro do arquivo .csv 

int criarCabecalhoBin(FILE*);           // Cabeçalho do arquivo binário       
int inserirRegistroBin(FILE*);          // Inserir registro no binário
int imprimirRegistroBin(FILE*);         // Imprime um registro
int lerBinario(FILE*);                  // Imprime todos os registros armazenados no arquivo bin
CABECALHO * lerCabecalhoBin(FILE*);     // Funcao auxiliar para acesso mais flexivel dos dados do arquivo 


#endif