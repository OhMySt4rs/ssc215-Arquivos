/* Criado por Mateus de Souza Santos 11366913
    e Wellington Matos Amaral 11315054
*/

#ifndef H_REGISTRO_
#define H_REGISTRO_

//Códigos de controle

#define SUCESSO 0
#define ERRO 1
#define regDeletado 2
 
struct registroCabecalho{
    unsigned char status;           // Sempre que uma modificacao inicia assume `0`, se concluir com sucesso assume `1`
    int RRNproxRegistro;            // Byteoffset do proximo registro
    int numeroRegistrosInseridos;   // Inicialmente vale 0 e incrementa sempre que existir um novo registro
    int numeroRegistrosRemovidos;   // Os registros ser'ao logicamente removidos, incrementam sempre que isso ocorrer
    int numeroRegistrosAtualizados; // Sempre que alguma informacao em um registro for atualizada, ele sera incrmentado
    char lixo[111];                 // Armazena apenas `$` para manter o padrao de tamanho de 128 bites
};

/* Organizacao hibrida de registros, haverao campos de tamanho fixo, campo de tamanho variavel 
 * (indicando o tamanho do campo) e delimitador entre registros `#`
 */

struct registro{

    // Campos de tamanho fixo: tamanho maximo de 23 bytes
    int idNascimento;               // código sequencial que identifica univocamente cada registro do arquivo de dados)
    int idadeMae;                   // idade da mãe do bebê
    char dataNascimento[11];        // formato AAAA-MM-DD; data de nascimento do bebê
    char sexoBebe;                  // sexo  do  bebê pode assumir os valores ‘0’(ignorado), ‘1’(masculino) e ‘2’(feminino)
    char estadoMae[3];              // sigla  do  estado  da cidade de residência da mãe
    char estadoBebe[3];             // sigla  do  estado  da cidade na qual o bebê nasceu
    int tam_CidadeMae;              // tamanho do campo cidadeMae
    int tam_CidadeBebe;             // tamanho do campo cidadeBebe

    // Campos  de  tamanho  variável: tamanho  máximo  de 105 bytes incluindo  os espaços reservados para os indicadores de tamanho
    char* cidadeMae;                // cidade de residência da mãe
    char* cidadeBebe;               // cidade na qual o bebê nasceu
};


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
int inserirRegistro(FILE* bin, int*, int*);                                     // Faz insercao de novos registros registros

#endif