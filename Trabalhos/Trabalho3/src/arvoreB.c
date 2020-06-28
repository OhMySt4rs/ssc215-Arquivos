#include <stdio.h>
#include <stdlib.h>
#include <registro.h>
#include <arvoreB.h>
#include <string.h>
#include <binarionatela.h>

#define NULO -1
#difine M 6

struct _arvoreB{
    unsigned char status;       // Teste para saber se o arquivo esta consistente
    int noRaiz;                 // No incial da arvore
    int nroNiveis;              // Altura maxima da arvore (partindo da raiz ate as folhas
    int proxRRN;                // RRN da proxima pagina da arvore B. Sempre que um novo node eh criado, o RRN eh incrementado
    int nroChaves;              // Numero total de chaves inseridas no arquivo
    char lixo[55];
};

struct _folhaB{
    int nivel;      // O nivel dos nos eh feito de baixo para cima, ou seja, a raiz possui o maior nivel e a folha o menor (1)
    int n;          // Numero de chaves presente no node (m = 6, entao n varia de (m/2)-1 = 2 a m -1 = 5)
    int C[M-1];     // Chave de busca
    int Pr[M-1];    // Campo de referencia do registro que a chave guarda, presente no arquivo de dados
    int P[M];       // Referencia para a proxima subartvore (caso nao exista recebe -1)
};

// dentro de cada pagina, as chaves sao ordenadas em ordem crescente

ARVOREB* criarCabecalhoArvoreB(){
    ARVOREB* aux;
    int i;
    
    aux = calloc(1, sizeof(ARVOREB));

    aux->noRaiz = NULO;

    for(i = 0; i < 55; i++){
        lixo[i] = '$';
    }

    return aux;
}

// Recebe um REGISTRO e o insere no arquivo de indice fazendo os balenceamentos necessarios
void inserirChave(){

}

// Recebe um arquivo (seguindo as especificacoes do trab 1) preenche o arquivo com as informacoes existentes no arquivo
// @return 0 se o status for inconsistente, ou 1 caso contrario (terminou corretamente)
int criarIndiceArvoreB(FILE* dataset, FILE* indexBtree){
    CABECALHO *header;
    ARVOREB *bTree;

    header = lerCabecalhoBin(dataset);

    if(!(header->status)){
        free(header);
        return ERRO;
    }

    bTree = criarCabecalhoArvoreB();

    /*
    for(i = 0; i < header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos ; i++){
        if(encontrarRegistroBin() != regDeletado){
            ler registro do dataset -> inserir na arvoreB
        }
        else continue;
    }
    Chave=idNascimento, Pr= RRN do registro;
    
}

/*
// Recebe um id nascimento e conta quantos acessos a paginas de disco foram feitos durante a busca 
// @return 0 se nao encontrou 1 em caso sucesso
int buscarRegistro(FILE* dataset, FILE* indexBtree, int idNascimento){

}

// Insere conforme a funcionalidade 6 do trabalho pratico, atualiza o arquivo da arvore B
// imprime binario na tela
void inserirReg(FILE* dataset, FILE* indexBtree){

}*/

