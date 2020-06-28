#ifndef H_ARVOREB_
#define H_ARVOREB_

typedef struct _arvoreB ARVOREB;
typedef struct _folhaB PAGINA;

int criarIndiceArvoreB(FILE* bin, FILE* indice);
ARVOREB* criarCabecalhoArvoreB();

#endif