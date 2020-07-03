#include <stdio.h>
#include <stdlib.h>
#include <registro.h>
#include <arvoreB.h>
#include <string.h>
#include <binarionatela.h>

#define VAZIO -1
#define PROMOVE -3
#define NAOPROMOVE -4
#define ENCONTROU -2
#define M 6

struct _arvoreB{
    unsigned char status;       // Teste para saber se o arquivo esta consistente
    int noRaiz;                 // No incial da arvore
    int nroNiveis;              // Altura maxima da arvore (partindo da raiz ate as folhas
    int proxRRN;                // RRN da proxima pagina da arvore B. Sempre que um novo node eh criado, o RRN eh incrementado
    int nroChaves;              // Numero total de chaves inseridas no arquivo
    char lixo[55];
};

struct _noB{
    int nivel;      // O nivel dos nos eh feito de baixo para cima, ou seja, a raiz possui o maior nivel e a folha o menor (1)
    int n;          // Numero de chaves presente no node (m = 6, entao n varia de (m/2)-1 = 2 a m -1 = 5)
    int C[M-1];     // Chave de busca
    int Pr[M-1];    // Campo de referencia do registro que a chave guarda, presente no arquivo de dados
    int P[M];       // Referencia para a proxima subarvore (caso nao exista recebe -1)
};

// dentro de cada pagina, as chaves sao ordenadas em ordem crescente

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;

    return;
}

void bubblesort(int vet[], int *ref, int tam){
    if(tam < 1) return;

    for(int i = 0; i < tam; i++){
        if(vet[i] < vet[i+1])
            swap(vet[i], vet[i+1]);
            swap(ref[i], ref[i+1]);
    }

    bubblesort(vet, ref, tam-1);
}

ARVOREB* criarCabecalhoArvoreB(){
    ARVOREB* aux;
    int i;
    
    aux = calloc(1, sizeof(ARVOREB));

    aux->noRaiz = VAZIO;

    for(i = 0; i < 55; i++){
        aux->lixo[i] = '$';
    }

    return aux;
}

int buscarIndice(FILE* indexBtree, int chave){
    PAGINA aux;
    int i;

    for(i = 0; i < M - 1; i++){
        if(chave > aux.C[i]) return i;
        else if(chave == aux.C[i]) return jaExiste;
    }

    return i;
}

int criarPagina(PAGINA* no){
    PAGINA *no = calloc(1, sizeof(PAGINA));
    int i;

    for(i = 0; i < M - 1; i++){
        no->P[i] = VAZIO;
        no->Pr[i] = VAZIO;
        no->C[i] = VAZIO;
    }
    no->P[M-1] = VAZIO;

    return SUCESSO;
}
int lerCabecalhoArvoreB();
int salvarCabecalhoArvoreB();
int lerPagina(int RRN, FILE *index, PAGINA *no){
    int i;
    
    fseek(index, 72 + (72*RRN), SEEK_SET);
    fread(&(no->nivel), sizeof(int), 1, index);
    fread(&(no->n), sizeof(int), 1, index);
    for(i = 0; i < M-1; i++){
        fread(&(no->C[i]), sizeof(int), 1, index);
        fread(&(no->Pr[i]), sizeof(int), 1, index);
    }
    for(i = 0; i < M; i++) fread(&(no->P[i]), sizeof(int), 1, index);

    return SUCESSO;
}

int salvarPagina(int RRN, FILE *index, PAGINA *no){
    int i;
    
    fseek(index, 72 + (72*RRN), SEEK_SET);
    fwrite(&(no->nivel), sizeof(int), 1, index);
    fwrite(&(no->n), sizeof(int), 1, index);
    
    for(i = 0; i < M-1; i++){
        fwrite(&(no->C[i]), sizeof(int), 1, index);
        fwrite(&(no->Pr[i]), sizeof(int), 1, index);
    }
    
    for(i = 0; i < M; i++) fwrite(&(no->P[i]), sizeof(int), 1, index);

    return SUCESSO;
}

int buscaInterna(int *v, int tam, int chave){
    int i; 
    for(i = 0; v[i] < chave || i < tam; i++){
        if(v[i] == chave) return ENCONTROU;
    }
    return i;
}

int split(int novaChave, int novaPr, int RRNInserido, PAGINA* page, int chavePromovida, 
          int PrPromovida, int filhoDPromovido, PAGINA* newpage, int RRN_newpage){
    int chaves[M], refs[M], childs[M+1];
    int i;

    for(i = 0; i < M -1; i ++){
        chaves[i] = page->C[i];
        refs[i] = page->Pr[i];
        childs[i] = page->P[i];
    }
    childs[i] = page->P[i];
    childs[i] = RRNInserido;
    chaves[i] = novaChave;
    refs[i] = novaPr;

    bubblesort(chaves, refs, M);

    chavePromovida = chaves[M/2];
    PrPromovida = refs[M/2];

    filhoDPromovido =  RRN_newpage;


    for(i = 0; i < M/2; i++){
        page->C[i] = chaves[i];
        page->Pr[i] = refs[i];
        page->P[i] = page->P[i];
    }
}

int inserir(FILE *index, ARVOREB *Btree, int RRNatual,int chave,
            int Pr,int chavePromovida,int PrPromovida,int filhoDPromovido){
    PAGINA *page, *newpage;
    int POS, retorno, chavePromovidaInf, PrPromovidaInf, filhoDPromovidoInf;
    
    if(RRNatual == VAZIO){
        chavePromovida = chave;
        PrPromovida = Pr;
        filhoDPromovido = VAZIO;
        
        return PROMOVE;
    } 
    else{
        criarPagina(page);
        lerPagina(RRNatual, index, page);
        
        POS = buscaInterna(page->C, page->n, chave);

        if(POS ==  ENCONTROU) return ERRO;

        retorno = inserir(index, Btree, page->P[POS], chave, Pr, chavePromovidaInf, PrPromovidaInf, filhoDPromovidoInf);                    
        
        if(retorno == ERRO || retorno == NAOPROMOVE) return retorno;
        
        else if(page->n < M-1){
            page->C[page->n] = chavePromovidaInf;
            page->Pr[page->n] = PrPromovidaInf;
            page->n++;
            bubblesort(page->C, page->Pr, page->n);  
            salvarPagina(RRNatual, index, page); 
            Btree->nroChaves++;       
            
            return NAOPROMOVE;
        } 
        else{
            criarPagina(newpage);
            split(chavePromovidaInf, PrPromovidaInf, filhoDPromovidoInf, page,
            chavePromovida, PrPromovida, filhoDPromovido, newpage, Btree->proxRRN);
            salvarPagina(RRNatual, index, page);
            salvarPagina(Btree->proxRRN, index, newpage);
            Btree->proxRRN++;
            
            return PROMOVE;
        }
    }
}

// Recebe um REGISTRO e o insere no arquivo de indice fazendo os balenceamentos necessarios
int inserirChave(ARVOREB *bTree, REGISTRO atual, int Pr, FILE *index){
    PAGINA *no;
    int chave = atual.idNascimento, raiz, chavePromovida, PrPromovida, filhoDPromovido;
    
    // nao ha nenhum regitro na arvore
    if(bTree->noRaiz == VAZIO){
        criarPagina(no);

        // a primeira pagina sera adicionada, entao cria-se o primeiro nivel da arvore
        bTree->nroNiveis++;
        // o no raiz recebe RRN 0
        bTree->noRaiz = bTree->proxRRN;
        // o proximo RRN eh o 1
        bTree->proxRRN = 1;

        // atualizar as informacoes do no que esta sendo inserido na arvore
        no->C[0] = atual.idNascimento;
        no->Pr[0] = Pr;
        no->n ++;
        no->nivel = 1;

        bTree->nroChaves++;

        salvarPagina(bTree->noRaiz, index, no);

        return SUCESSO;
    } else{
        raiz = bTree->noRaiz;
        if(inserir(index, bTree, raiz, chave, Pr, chavePromovida, PrPromovida, filhoDPromovido) == PROMOVE){
            criarPagina(no);
            no->C[0] = chavePromovida;
            no->Pr[0] = PrPromovida;
            no->P[0] = raiz;
            no->P[1] = filhoDPromovido;
            raiz = bTree->proxRRN;
            bTree->noRaiz = raiz;
            salvarPagina(raiz, index, no);
        }
    }
    /*busca no arquivo de indice (segundo a ordenacao comparando com a chave registroNascimento)
        o no que deveria receber o registro
        se ja existe no index retorna erro
        senao */

    /*verficar quantas chaves tem no registro atual
        se menor que 5 insere no no atual
            Chave=idNascimento, Pr= RRN do registro;
            dentro no node atual, verificar ordenacao (crescente)
        senao verifica balanceamento ... ... ...
        
        retorna sucesso*/
}

// Recebe um arquivo (seguindo as especificacoes do trab 1) preenche o arquivo com as informacoes existentes no arquivo
// @return 0 se o status for inconsistente, ou 1 caso contrario (terminou corretamente)
int criarIndiceArvoreB(FILE* dataset, FILE* indexBtree){
    CABECALHO *header;
    ARVOREB *bTree;
    REGISTRO aux;

    header = lerCabecalhoBin(dataset);

    if(!(header->status)){
        free(header);
        return ERRO;
    }

    bTree = criarCabecalhoArvoreB();

    for(i = 0; i < header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos ; i++){
        if(encontrarRegistroBin(dataset, i, &aux) != regDeletado){
            inserirChave(bTree, aux, i, indexBtree);
        }
    }

    return SUCESSO;
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

