#include <stdio.h>
#include <stdlib.h>
#include <registro.h>
#include <arvoreB.h>
#include <string.h>
#include <binarionatela.h>

#define VAZIO -1
#define PROMOVE -3
#define NAOPROMOVE -4
#define DEURUIM -5
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

// Criar cabecalho da arqvore b
ARVOREB* criarCabecalhoArvoreB(){
    ARVOREB* aux;
    int i;
    
    // Alocar espacp
    aux = calloc(1, sizeof(ARVOREB));

    aux->noRaiz = VAZIO;

    // Preencher com lixo quando necessario
    for(i = 0; i < 55; i++){
        aux->lixo[i] = '$';
    }

    return aux;
}

// Criar pagina(no) da arvore
PAGINA * criarPagina(){
    PAGINA* no;
    no = calloc(1, sizeof(PAGINA));
    int i;

    // Inicializando os campos da arvore
    for(i = 0; i < M - 1; i++){
        no->P[i] = VAZIO;
        no->Pr[i] = VAZIO;
        no->C[i] = VAZIO;
    }
    no->P[M-1] = VAZIO;

    return no;
}

// Ler cabecalho de uma arvore ja existente
ARVOREB *lerCabecalhoArvoreB(FILE *index){
    ARVOREB *aux = calloc(1, sizeof(ARVOREB));
    
    // Caso haja falha na alocacao, nao insere-se nada no cabecalho
    if(aux == NULL) return aux;
    
    fread(&(aux->status), sizeof(unsigned char), 1, index);
    fread(&(aux->noRaiz), sizeof(int), 1, index);
    fread(&(aux->nroNiveis), sizeof(int), 1, index);
    fread(&(aux->proxRRN), sizeof(int), 1, index);
    fread(&(aux->nroChaves), sizeof(int), 1, index);
    fread(&(aux->lixo), sizeof(char), 55, index);

    return aux;
}

int salvarCabecalhoArvoreB(FILE *index, ARVOREB *bTree){
    fseek(index, 0, SEEK_SET);
    bTree-> status = '1';
    fwrite(&(bTree->status), sizeof(unsigned char), 1, index);
    fwrite(&(bTree->noRaiz), sizeof(int), 1, index);
    fwrite(&(bTree->nroNiveis), sizeof(int), 1, index);
    fwrite(&(bTree->proxRRN), sizeof(int), 1, index);
    fwrite(&(bTree->nroChaves), sizeof(int), 1, index);
    for(int i = 0; i < 55; i++){
        bTree->lixo[i] = '$';
    }
    fwrite(&(bTree->lixo), sizeof(char), 55, index);

    return SUCESSO;
}

PAGINA *lerPagina(int RRN, FILE *index){
    int i;
    PAGINA *no = criarPagina();
    
    fseek(index, 72 + (72*RRN), SEEK_SET);
    fread(&(no->nivel), sizeof(int), 1, index);
    fread(&(no->n), sizeof(int), 1, index);
    for(i = 0; i < M-1; i++){
        fread(&(no->C[i]), sizeof(int), 1, index);
        fread(&(no->Pr[i]), sizeof(int), 1, index);
    }
    for(i = 0; i < M; i++) fread(&(no->P[i]), sizeof(int), 1, index);

    return no;
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
    for(i = 0; v[i] <= chave && i < tam; i++){
        if(v[i] == chave) return ENCONTROU;
    }
    return i;
}

int split(int novaChave, int novaPr, int filhoDnova, PAGINA* page, int *chavePromovida, 
          int *PrPromovida, int *filhoDPromovido, PAGINA* newpage, int RRN_newpage, int POS){
    int chaves[M], refs[M], childs[M+1];
    int i, j;

    for(i = 0; i < POS; i ++){
        chaves[i] = page->C[i];
        refs[i] = page->Pr[i];
        childs[i] = page->P[i];
    }

    childs[i] = page->P[i];
    chaves[i] = novaChave;
    refs[i] = novaPr;
    childs[i+1] = filhoDnova;
    
    for(i = i, j = i + 1; j < M; i++, j++){
        chaves[j] = page->C[i];
        refs[j] = page->Pr[i];
        childs[j+1] = page->P[i];
    }

    *chavePromovida = chaves[M/2];
    *PrPromovida = refs[M/2];
    *filhoDPromovido =  RRN_newpage;

    for(i = 0; i < M/2; i++){
        page->C[i] = chaves[i];
        page->Pr[i] = refs[i];
        page->P[i] = childs[i];
    }

    page->P[i] = childs[i];

    for(j = i; j < M - 1; j++){
        page->C[j] = VAZIO;
        page->Pr[j] = VAZIO;
        page->P[j+1] = VAZIO;
    }


    
    for(i = (M/2)+1, j= 0; i < M; j++, i++){
        newpage->C[j] = chaves[i];
        newpage->Pr[j] = refs[i];
        newpage->P[j] = childs[i];
    }
    
    newpage->P[j] = childs[i];
    newpage->nivel = page->nivel;
    newpage->n = 2;
    page->n = 3;
    
    return SUCESSO;
}

int inserir(FILE *index, ARVOREB *Btree, int RRNatual,int chave,
    int Pr,int *chavePromovida,int *PrPromovida,int *filhoDPromovido){
    PAGINA *page, *newpage;
    int POS, retorno, chavePromovidaInf, PrPromovidaInf, filhoDPromovidoInf, i;
    
    // printf("Estou no RRN = %d\n", RRNatual);
    if(RRNatual == VAZIO){
        *chavePromovida = chave;
        *PrPromovida = Pr;
        *filhoDPromovido = VAZIO;  
        return PROMOVE;
    } 
    else{
        page = lerPagina(RRNatual, index);
        POS = buscaInterna(page->C, page->n, chave);
        // printf("Pagina de nivel %d tem %d chaves de valor = ", page->nivel, page->n);
        // for(int i = 0; i < M -1; i++) printf("%d ", page->C[i]);
        // printf("\nBuscou chave= %d, deveria está  no RRN = %d ou pos=%d depois da chave\n", chave, page->P[POS], POS);
        
        if(POS ==  ENCONTROU) {
            free(page);
            return DEURUIM;
        }

        retorno = inserir(index, Btree, page->P[POS], chave, Pr, &chavePromovidaInf, &PrPromovidaInf, &filhoDPromovidoInf);                    
        
        if(retorno == DEURUIM || retorno == NAOPROMOVE){ 
            free(page);
            return retorno;
        }
        
        else if(page->n < M-1){
            // printf("Inserindo na própria pagina de RRN = %d, de nivel = %d\n", RRNatual, page->nivel);
            
            // SHIFTA P/ FRENTE E INSERE
            for(i = (page->n - 1); i >= POS; i--){
                swap(&(page->C[i]), &(page->C[i+1]));
                swap(&(page->Pr[i]), &(page->Pr[i+1]));
                swap(&(page->P[i+1]), &(page->P[i+2]));
            }

            page->C[POS] = chavePromovidaInf;
            page->Pr[POS] = PrPromovidaInf;
            page->P[POS+1] = filhoDPromovidoInf;
            page->n++;

            salvarPagina(RRNatual, index, page); 
            Btree->nroChaves++;      
            // printf("Chaves apos insercao = ");
            // for(int i = 0; i < M -1; i++) printf("%d ", page->C[i]);
            // printf("\n");
            //  printf("Refes apos insercao = ");
            // for(int i = 0; i < M ; i++) printf("%d ", page->P[i]);
            // printf("\n");
            free(page);
            return NAOPROMOVE;
        } else{ //Deu overflow e temos que criar uma nova pagina
            // printf("Deu overflow na pagina de RRN=%d....\n", RRNatual);
            newpage = criarPagina();
            split(chavePromovidaInf, PrPromovidaInf, filhoDPromovidoInf, page,
            chavePromovida, PrPromovida, filhoDPromovido, newpage, Btree->proxRRN, POS);
            // printf("Inseriu na nova pagina de RRN = %d e promoveu a chave= %d com filhoD = %d\n", Btree->proxRRN, *chavePromovida, *filhoDPromovido);
            salvarPagina(RRNatual, index, page);
            salvarPagina(Btree->proxRRN, index, newpage);
            Btree->proxRRN++;
            // printf("Prox RRN: %d\n", Btree->proxRRN);

            // printf("Chaves a esquerda apos insercao e overflow = ");
            // for(int i = 0; i < M -1; i++) printf("%d ", page->C[i]);
            // printf("\n");
            
            // printf("Chaves apos insercao a direita apos overfow = ");
            // for(int i = 0; i < M -1; i++) printf("%d ", newpage->C[i]);
            // printf("\n");

            free(page);
            free(newpage);
            return PROMOVE;
        }
    }
}

// Recebe um REGISTRO e o insere no arquivo de indice fazendo os balenceamentos necessarios
int inserirChave(ARVOREB *bTree, int chave, int Pr, FILE *index){
    PAGINA *no;
    int raiz, chavePromovida, PrPromovida, filhoDPromovido;
    // nao ha nenhum regitro na arvore
    if(bTree->noRaiz == VAZIO){
        // printf("Criar a raiz\n");

        no = criarPagina();

        // a primeira pagina sera adicionada, entao cria-se o primeiro nivel da arvore
        bTree->nroNiveis++;
        // o no raiz recebe RRN 0
        bTree->noRaiz = bTree->proxRRN;
        // o proximo RRN eh o 1
        bTree->proxRRN = 1;

        // atualizar as informacoes do no que esta sendo inserido na arvore
        no->C[0] = chave;
        no->Pr[0] = Pr;
        no->n ++;
        no->nivel = 1;

        bTree->nroChaves++;

        salvarPagina(bTree->noRaiz, index, no);

        free(no);

        return SUCESSO;
    } else{
        int retorno;
        PAGINA* leftpage;
        raiz = bTree->noRaiz;
        // printf("Raiz esta no RRN=%d\n", bTree->noRaiz);
        if((retorno = inserir(index, bTree, raiz, chave, Pr, &chavePromovida, 
        &PrPromovida, &filhoDPromovido)) == PROMOVE){
            // printf("Cria nova raíz\n");
            no = criarPagina();
            leftpage = lerPagina(raiz, index);
            no->C[0] = chavePromovida;
            no->Pr[0] = PrPromovida;
            no->P[0] = raiz;
            no->P[1] = filhoDPromovido;
            no->n++;
            no->nivel= leftpage->nivel + 1;
            raiz = bTree->proxRRN;
            bTree->noRaiz = raiz;
            bTree->nroChaves++;
            bTree->nroNiveis++;
            bTree->proxRRN++;
            // printf("Prox RRN: %d\n", bTree->proxRRN);
            salvarPagina(raiz, index, no);
            free(no);
            free(leftpage);
        } else if(retorno == ERRO){
            // printf("ERRO\n");
            return ERRO;    
        } else{
            // printf("INSERIU\n");
        }
    }
    return SUCESSO;
}

// Recebe um arquivo (seguindo as especificacoes do trab 1) preenche o arquivo com as informacoes existentes no arquivo
// @return 0 se o status for inconsistente, ou 1 caso contrario (terminou corretamente)
int criarIndiceArvoreB(FILE* dataset, FILE* indexBtree){
    CABECALHO *header;
    ARVOREB *bTree;
    int i;
    REGISTRO aux;

    header = lerCabecalhoBin(dataset);

    if(!(header->status)){
        free(header);
        return DEURUIM;
    }

    bTree = criarCabecalhoArvoreB();

    for(i = 0; i < (header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos) ; i++){ 
        if(encontrarRegistroBin(dataset, i, &aux) != regDeletado){
            // printf("Inserindo registro, id Nascimento = %d\n", aux.idNascimento);
            inserirChave(bTree, aux.idNascimento, i, indexBtree);
        }
        free(aux.cidadeMae);
        free(aux.cidadeBebe);
    }

    // printf("Tantas chaves %d\n", bTree->nroChaves);
    salvarCabecalhoArvoreB(indexBtree, bTree);

    free(header);
    free(bTree);
    return SUCESSO;
}

int buscaPagina(int RRN, FILE* index, int chave, int *acessos){
    PAGINA *page;
    int POS, ret;

    if(RRN == VAZIO) return DEURUIM;

    page = lerPagina(RRN, index);

    (*acessos)++;

    if((POS = buscaInterna(page->C, page->n, chave)) == ENCONTROU){
        
        for(int i = 0; i < page->n; i++){
            if(page->C[i] == chave){ 
                ret = page->Pr[i];
            }
        }
        free(page);
        return ret;
    } 
    else{
        ret = buscaPagina(page->P[POS], index, chave, acessos);
        free(page);
        return ret;
    } 

    return -7;
}

// Recebe um id nascimento e conta quantos acessos a paginas de disco foram feitos durante a busca 
// @return 0 se nao encontrou 1 em caso sucesso
int buscaArvoreB(FILE* dataset, FILE* indexBtree, int chave){
    ARVOREB *bTree = lerCabecalhoArvoreB(indexBtree);
    CABECALHO *header = lerCabecalhoBin(dataset);
    REGISTRO aux;
    int acessos = 0;
    
    if(header->status == '0' || bTree->status == '0'){
        free(header);
        free(bTree);
        return DEURUIM;
    }

    int RRNRegistro = buscaPagina(bTree->noRaiz, indexBtree, chave, &acessos);

    if(RRNRegistro == DEURUIM) printf("Registro inexistente.\n");
    else{
        encontrarRegistroBin(dataset, RRNRegistro, &aux);
        imprimirRegistro(aux);
        printf("Quantidade de paginas da arvore-B acessadas: %d\n", acessos);
    } 

    free(header);
    free(bTree);
    return SUCESSO;
}

// Insere conforme a funcionalidade 6 do trabalho pratico, atualiza o arquivo da arvore B
// imprime binario na tela
int inserirRegistroArvoreB(FILE* dataset, FILE* indexBtree){
    int idNascimento, Pr;
    ARVOREB *bTree;

    if(inserirRegistro(dataset, &idNascimento, &Pr) == ERRO) return ERRO;

    bTree = lerCabecalhoArvoreB(indexBtree);

    if(bTree->status == '0') return ERRO;

    inserirChave(bTree, idNascimento, Pr, indexBtree);

    free(bTree);

    return SUCESSO;
}

