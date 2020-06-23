/* Criado por Mateus de Souza Santos 11366913
    e Wellington Matos Amaral 11315054
*/

#ifndef H_REGISTRO_
#define H_REGISTRO_

//Códigos de controle

#define SUCESSO 0
#define ERRO 1
#define regDeletado 2


typedef struct registroCabecalho CABECALHO;
typedef struct registro REGISTRO;

int criaBinario(FILE*, FILE*);                                                  // Cria um arquivo binário com base no arquivo fonte
REGISTRO lerRegistro(FILE*);                                                    // Ler o registro do arquivo .csv 

int imprimirRegistroBin(FILE*, int posReg);                                     // Imprime um registro
int lerBinario(FILE*);                                                          // Imprime todos os registros armazenados no arquivo bin
int criarCabecalhobin(CABECALHO* header, FILE* dest, unsigned char status);     // Adiciona e atualiza o registro de cabecalho em um arquivo binario
CABECALHO * lerCabecalhoBin(FILE*);                                             // Funcao auxiliar para acesso mais flexivel dos dados do arquivo 
int armazenarRegistrobin(REGISTRO,  FILE* dest, int );                          // Escreve um registro no arquivo dest
int buscaRRN(FILE *, int);                                                      // Procurar registros com uma combinacao de campos de um registro
REGISTRO* definirCriteriosBusca(int campos);                                    // Funcao auxiliar para receber os filtros para pesquisa do usuario
int encontrarRegistroBin(FILE *bin, int posReg, REGISTRO *aux);                 // Funcao auxiliar para percorrer o arquivo e encontrar um registro, ultilizando campos
int imprimirRegistro(REGISTRO aux);                                             // Funcao que faz a impressao dos registros na forma pedida
int buscaCombinadaRegistro(FILE* src);                                          // Funcao que retorna um registro a partir dos criterios definidos
int compararRegistro(REGISTRO *src, REGISTRO *aux);                             // Funcao que compara dois registros 
int removerRegistroBin(FILE *src);                                              // Funcao que faz remocao logica de registros que atenderem os criterios especificos
int atualizarRegistroBin(FILE* src, int RRN);                                   // Encontra um registro e o atualiza
int inserirRegistro(FILE* bin);                                                 // Faz insercao de novos registros registros

#endif