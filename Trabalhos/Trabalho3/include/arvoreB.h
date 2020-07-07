#ifndef H_ARVOREB_
#define H_ARVOREB_

typedef struct _arvoreB ARVOREB;
typedef struct _noB PAGINA;

int criarIndiceArvoreB(FILE* bin, FILE* indice);
int buscarIndice(FILE* indexBtree, int chave);
int inserirChave(ARVOREB *bTree, REGISTRO atual, int RRNregistro, FILE *index);
ARVOREB* criarCabecalhoArvoreB();
int buscaArvoreB(FILE* dataset, FILE* indexBtree, int chave);

#endif