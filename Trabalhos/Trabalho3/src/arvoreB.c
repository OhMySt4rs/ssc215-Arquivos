#include <stdio.h>
#include <stdlib.h>
#include <registro.h>
#include <arvoreB.h>
#include <string.h>
#include <binarionatela.h>


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
    
    fseek(index, 0, SEEK_SET);
    // Caso haja falha na alocacao, nao insere-se nada no cabecalho
    if(aux == NULL) return aux;
    
    // Leitura de todos os campos
    fread(&(aux->status), sizeof(unsigned char), 1, index);
    fread(&(aux->noRaiz), sizeof(int), 1, index);
    fread(&(aux->nroNiveis), sizeof(int), 1, index);
    fread(&(aux->proxRRN), sizeof(int), 1, index);
    fread(&(aux->nroChaves), sizeof(int), 1, index);
    fread(&(aux->lixo), sizeof(char), 55, index);

    return aux;
}

// Apos alteracores em uma arvore, estas devem ser salvas no seu respectivo arquivo
int salvarCabecalhoArvoreB(FILE *index, ARVOREB *bTree){
    // Apontar para o inicio do arquivo
    fseek(index, 0, SEEK_SET);
    // Mudar o status do arquivo
    bTree-> status = '1';
    // Escrever as informacoes existitentes na arvore (sejam novas, ou nao)
    fwrite(&(bTree->status), sizeof(unsigned char), 1, index);
    fwrite(&(bTree->noRaiz), sizeof(int), 1, index);
    fwrite(&(bTree->nroNiveis), sizeof(int), 1, index);
    fwrite(&(bTree->proxRRN), sizeof(int), 1, index);
    fwrite(&(bTree->nroChaves), sizeof(int), 1, index);
    // Escrever lixo nos lugares pertinentes
    for(int i = 0; i < 55; i++){
        bTree->lixo[i] = '$';
    }
    fwrite(&(bTree->lixo), sizeof(char), 55, index);

    return SUCESSO;
}

// Ler um pagina ja existente
PAGINA *lerPagina(int RRN, FILE *index){
    int i;
    // Utilizaremos uma pagina auxiliar para receber os valores da leitura
    PAGINA *no = criarPagina();
    
    // Ajustar o ponteiro para o RRN correto
    fseek(index, 72 + (72*RRN), SEEK_SET);
    // Ler os campos
    fread(&(no->nivel), sizeof(int), 1, index);
    fread(&(no->n), sizeof(int), 1, index);
    for(i = 0; i < M-1; i++){
        fread(&(no->C[i]), sizeof(int), 1, index);
        fread(&(no->Pr[i]), sizeof(int), 1, index);
    }
    for(i = 0; i < M; i++) fread(&(no->P[i]), sizeof(int), 1, index);

    return no;
}

// Guardar uma pagina
int salvarPagina(int RRN, FILE *index, PAGINA *no){
    int i;
    
    // Ajustar o ponteiro para a posicao correta
    fseek(index, 72 + (72*RRN), SEEK_SET);
    // Escrever todos os campos
    fwrite(&(no->nivel), sizeof(int), 1, index);
    fwrite(&(no->n), sizeof(int), 1, index);
    
    for(i = 0; i < M-1; i++){
        fwrite(&(no->C[i]), sizeof(int), 1, index);
        fwrite(&(no->Pr[i]), sizeof(int), 1, index);
    }
    
    for(i = 0; i < M; i++) fwrite(&(no->P[i]), sizeof(int), 1, index);

    return SUCESSO;
}

// Percorrer o vetor do no para procurar uma chave
int buscaInterna(int *v, int tam, int chave){
    int i; 
    // Eh possivel que esta chave ja exista na arvore ou nao exista espaco para esta chave no local
    for(i = 0; v[i] <= chave && i < tam; i++){
        if(v[i] == chave) return ENCONTROU;
    }
    // Retorna um indice, para caso encontre uma posicao correta, ou a chave nao "pertenca" a este lugar
    return i;
}

// Dividir o vetor de chaves
int split(int novaChave, int novaPr, int filhoDnova, PAGINA* page, int *chavePromovida, 
          int *PrPromovida, int *filhoDPromovido, PAGINA* newpage, int RRN_newpage, int POS){
    int chaves[M], refs[M], childs[M+1];
    int i, j;

    // Atribuir uma metada das informacoes para um vetor
    for(i = 0; i < POS; i ++){
        chaves[i] = page->C[i];
        refs[i] = page->Pr[i];
        childs[i] = page->P[i];
    }

    // Guardar as informacoes do indice do meio
    childs[i] = page->P[i];
    chaves[i] = novaChave;
    refs[i] = novaPr;
    childs[i+1] = filhoDnova;
    
    // Atribuir as informacoes da outra metade
    for(i = i, j = i + 1; j < M; i++, j++){
        chaves[j] = page->C[i];
        refs[j] = page->Pr[i];
        childs[j+1] = page->P[i];
    }

    // Guardar as informacoes importantes, para nao perder a referencia
    *chavePromovida = chaves[M/2];
    *PrPromovida = refs[M/2];
    *filhoDPromovido =  RRN_newpage;

    // As informacoes referenciadas anteriormente serao atribuidas agora
    for(i = 0; i < M/2; i++){
        page->C[i] = chaves[i];
        page->Pr[i] = refs[i];
        page->P[i] = childs[i];
    }

    page->P[i] = childs[i];

    // Inicializar as outras posicoes dos novos vetores para evitar sujeiras
    for(j = i; j < M - 1; j++){
        page->C[j] = VAZIO;
        page->Pr[j] = VAZIO;
        page->P[j+1] = VAZIO;
    }

    // Inicializar as outras posicoes dos novos vetores para evitar sujeiras
    for(i = (M/2)+1, j= 0; i < M; j++, i++){
        newpage->C[j] = chaves[i];
        newpage->Pr[j] = refs[i];
        newpage->P[j] = childs[i];
    }
    
    // Atualizar informacoes do pagina
    newpage->P[j] = childs[i];
    newpage->nivel = page->nivel;
    newpage->n = 2;
    page->n = 3;
    
    return SUCESSO;
}

// Guardar uma nova chave e suas informacoes
int inserir(FILE *index, ARVOREB *Btree, int RRNatual,int chave,
    int Pr,int *chavePromovida,int *PrPromovida,int *filhoDPromovido){
    PAGINA *page, *newpage;
    int POS, retorno, chavePromovidaInf, PrPromovidaInf, filhoDPromovidoInf, i;
    
    // Verificar se a posicao atual esta vazia, caso sim fazer a atribuicao
    if(RRNatual == VAZIO){
        *chavePromovida = chave;
        *PrPromovida = Pr;
        *filhoDPromovido = VAZIO;  
        return PROMOVE;
    } 
    else{
        // Ler a pagina atual e procurar a posicao certa
        page = lerPagina(RRNatual, index);
        POS = buscaInterna(page->C, page->n, chave);

        // Casp ja exista essa chave, finaliza a operacao
        if(POS ==  ENCONTROU) {
            free(page);
            return DEURUIM;
        }

        // Recursivamente verifica se eh possivel guardar a pagina na posicao atual da recursao
        retorno = inserir(index, Btree, page->P[POS], chave, Pr, &chavePromovidaInf, &PrPromovidaInf, &filhoDPromovidoInf);                    
        
        if(retorno == DEURUIM || retorno == NAOPROMOVE){ 
            free(page);
            return retorno;
        }
        
        // Caso na pagina atual ainda exista espaco para esta chave, sera escrito neste local
        else if(page->n < M-1){

            // Shifita os itens para frente e os insere, esta em uma ordenacao do vetor bastante eficiente para este caso
            for(i = (page->n - 1); i >= POS; i--){
                swap(&(page->C[i]), &(page->C[i+1]));
                swap(&(page->Pr[i]), &(page->Pr[i+1]));
                swap(&(page->P[i+1]), &(page->P[i+2]));
            }

            // Escrever as informacoes
            page->C[POS] = chavePromovidaInf;
            page->Pr[POS] = PrPromovidaInf;
            page->P[POS+1] = filhoDPromovidoInf;
            page->n++;

            // Salvar pagina
            salvarPagina(RRNatual, index, page); 
            Btree->nroChaves++;      

            free(page);
            return NAOPROMOVE;
        // Deu overflow e temos que criar uma nova pagina e apos isso fazer split
        } else{ 
            // Cria pagina
            newpage = criarPagina();
            // Faz overflow
            split(chavePromovidaInf, PrPromovidaInf, filhoDPromovidoInf, page,
            chavePromovida, PrPromovida, filhoDPromovido, newpage, Btree->proxRRN, POS);
            
            // Escreve as novas paginas
            salvarPagina(RRNatual, index, page);
            salvarPagina(Btree->proxRRN, index, newpage);
            Btree->proxRRN++;

            // Libera as alocacoes
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
    // Nao ha nenhum regitro na arvore
    if(bTree->noRaiz == VAZIO){

        // Cria uma pagina vazia
        no = criarPagina();

        // a primeira pagina sera adicihonada, entao cria-se o primeiro nivel da arvore
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

        // Salva pagina
        salvarPagina(bTree->noRaiz, index, no);

        free(no);

        return SUCESSO;
    } else{
        int retorno;
        PAGINA* leftpage;
        // Recebe o no raiz da subarvore
        raiz = bTree->noRaiz;

        // Caso tenha que promover
        if((retorno = inserir(index, bTree, raiz, chave, Pr, &chavePromovida, 
        &PrPromovida, &filhoDPromovido)) == PROMOVE){
            // Cria uma nova pagina
            no = criarPagina();
            // Le a pagina da esquerda
            leftpage = lerPagina(raiz, index);
            // Escreve as informacoes da primeira 
            no->C[0] = chavePromovida;
            no->Pr[0] = PrPromovida;
            no->P[0] = raiz;
            no->P[1] = filhoDPromovido;
            no->n++;
            no->nivel= leftpage->nivel + 1;
            // Atualiza as informacoes do cabecalho da subarvore
            raiz = bTree->proxRRN;
            bTree->noRaiz = raiz;
            bTree->nroChaves++;
            bTree->nroNiveis++;
            bTree->proxRRN++;
            // Salva a pagina
            salvarPagina(raiz, index, no);
            // Libera as variaveis
            free(no);
            free(leftpage);
        // Caso tenha ocorrido algum erro, retorna uma mensagem que pode ser tratado, caso necessario    
        } else if(retorno == ERRO){
            return ERRO;  
        // Senao, correu tudo bem  
        } else;
        
    }
    return SUCESSO;
}

// Recebe um arquivo (seguindo as especificacoes do trab 1) preenche o arquivo com as informacoes existentes no arquivo
int criarIndiceArvoreB(FILE* dataset, FILE* indexBtree){
    CABECALHO *header;
    ARVOREB *bTree;
    int i;
    REGISTRO aux;

    // Ler cabecalho da arvore 
    header = lerCabecalhoBin(dataset);

    // Caso o arquivo esteja inconsistente, finaliza a operacao
    if(header->status == '0'){
        free(header);
        return DEURUIM;
    }

    // Cria um cabecalho auxiliar para receber as informacoes
    bTree = criarCabecalhoArvoreB();

    // Percorrer todos os registros do arquivo base para trata-los e escreve-los no novo arquivo de indice
    for(i = 0; i < (header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos) ; i++){ 
        // Caso o arquivo nao tenha sido removido
        if(encontrarRegistroBin(dataset, i, &aux) != regDeletado){
            // Insere a chave
            inserirChave(bTree, aux.idNascimento, i, indexBtree);
        }
        // Liberar variaveis auxiliares
        free(aux.cidadeMae);
        free(aux.cidadeBebe);
    }

    // Escrever informacoes no cabecalho
    salvarCabecalhoArvoreB(indexBtree, bTree);

    // Liberar memoria
    free(header);
    free(bTree);
    return SUCESSO;
}

// Percorre o arquivo de indice buscando uma chave
int buscaPagina(int RRN, FILE* index, int chave, int *acessos){
    PAGINA *page;
    int POS, ret;

    // Caso nao exista nada nesta posicao (folha), finaliza a operacao
    if(RRN == VAZIO) return DEURUIM;

    // Ler a pagina da posicao atual
    page = lerPagina(RRN, index);

    // Conta quantos acessos a pagina foram feitos durante a busca
    (*acessos)++;

    // Procurar no arquivo de indices a chave, recebe o sinal que dentro do vetor do no existe a chave
    if((POS = buscaInterna(page->C, page->n, chave)) == ENCONTROU){
        // Sabendo que a chave existe nesse vetor, rercorre o este ate achar a chave
        for(int i = 0; i < page->n; i++){
            if(page->C[i] == chave){ 
                ret = page->Pr[i];
            }
        }
        // Liberea a variavel
        free(page);
        // Retorna a referencia da chave
        return ret;
    } 
    else{
        // Caso nao esteja neste no, recursivamente continua percorrendo
        ret = buscaPagina(page->P[POS], index, chave, acessos);
        // Libera informacoes
        free(page);
        return ret;
    } 

    return -7;
}

// Recebe um id nascimento e conta quantos acessos a paginas de disco foram feitos durante a busca 
int buscaArvoreB(FILE* dataset, FILE* indexBtree, int chave){
    // Le as informacoes da arvore e do arquivo de registro
    ARVOREB *bTree = lerCabecalhoArvoreB(indexBtree);
    CABECALHO *header = lerCabecalhoBin(dataset);
    REGISTRO aux;
    int acessos = 0;
    
    // Caso qualquer um dos dois for inconsistente, finaliza a operaca
    if(header->status == '0' || bTree->status == '0'){
        free(header);
        free(bTree);
        return DEURUIM;
    }

    // Percorre o aquivo de registro ate a chave de busca
    int RRNRegistro = buscaPagina(bTree->noRaiz, indexBtree, chave, &acessos);

    // Caso tenha sido removido, ou nao inserido, retorna erro e finaliza a operacao
    if(RRNRegistro == DEURUIM) printf("Registro inexistente.\n");
    else{
    // Caso encontrou a chave, imprime as informacoes
        encontrarRegistroBin(dataset, RRNRegistro, &aux);
        imprimirRegistro(aux);
        printf("Quantidade de paginas da arvore-B acessadas: %d\n", acessos);
    } 

    // Libera a memoria
    free(header);
    free(bTree);
    return SUCESSO;
}

// Insere conforme a funcionalidade 6 do trabalho pratico, atualiza o arquivo da arvore B
int inserirRegistroArvoreB(FILE* dataset, FILE* indexBtree){
    int idNascimento, Pr;
    ARVOREB *bTree;

    // Tenta inserir o registro no arquivo de registros, testa se eh consistente, se ja existe a chave
    if(inserirRegistro(dataset, &idNascimento, &Pr) == ERRO) return ERRO;

    // Le o cabecalho da arvore B
    bTree = lerCabecalhoArvoreB(indexBtree);

    // Verifica se o arquivo da arvore eh inconsistente
    if(bTree->status == '0') return ERRO;

    // Apos as verificaoes escreve a chave

    inserirChave(bTree, idNascimento, Pr, indexBtree);

    salvarCabecalhoArvoreB(indexBtree, bTree);

    // Libera a memoria
    free(bTree);

    return SUCESSO;
}
