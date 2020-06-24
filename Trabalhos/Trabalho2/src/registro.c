/* Criado por Mateus de Souza Santos 11366913
    e Wellington Matos Amaral 11315054
*/

#include <stdio.h>
#include <stdlib.h>
#include <registro.h>
#include <string.h>
#include <binarionatela.h>

#define maior(a, b) (a >= b ? a : b)

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


CABECALHO* lerCabecalhoBin(FILE* bin){ // Ler de um arquivo binário o cabeçalho e o retorna 
    CABECALHO *aux = calloc(1, sizeof(CABECALHO)); 
    
    if(aux == NULL) return aux;
    
    fread(&(aux->status), sizeof(unsigned char), 1, bin);
    fread(&(aux->RRNproxRegistro), sizeof(int), 1, bin);
    fread(&(aux->numeroRegistrosInseridos), sizeof(int), 1, bin);
    fread(&(aux->numeroRegistrosRemovidos), sizeof(int), 1, bin);
    fread(&(aux->numeroRegistrosAtualizados), sizeof(int), 1, bin);
    fread(&(aux->lixo), sizeof(char), 111, bin);

    return aux;
}

int criaBinario(FILE *src, FILE* dest){
    CABECALHO *header;
    REGISTRO regLido;
    char entradaCabecalhoCSV[140];
    int lastID = -1;
    
    if((header = calloc(1, sizeof(CABECALHO))) == NULL) return ERRO;
 
    fseek(src, 0, SEEK_END);     // Ponteiro vai pro final do arquivo csv

    if(ftell(src) == 0){         // Verifica se o arquivo tem 0 byte
        criarCabecalhobin(header, dest, '1');
        free(header);
        return SUCESSO;
    }

    fseek(src, 0, SEEK_SET);     // Volta o ponteiro para o inicio do arquivo

    fgets(entradaCabecalhoCSV, 140, src); //Pula o cabeçalho do csv
    
    regLido = lerRegistro(src); //Ler o primeiro registro

    while(lastID != regLido.idNascimento){
        
        // Byteoffset do prox registro no binario 
        fseek(dest, (header->RRNproxRegistro * 128) + 128, SEEK_SET);

        // Armazena os dados
        armazenarRegistrobin(regLido, dest, 0);

        free(regLido.cidadeMae);
        free(regLido.cidadeBebe);

        //atualiza o header
        header->numeroRegistrosInseridos++;
        header->RRNproxRegistro ++;       
         
        lastID = regLido.idNascimento;

        //vai pro próximo  registro
        regLido = lerRegistro(src);
 
    }


    free(regLido.cidadeMae);
    free(regLido.cidadeBebe);

    //armazenar o header
    criarCabecalhobin(header, dest, '1');
    free(header);
    
    return SUCESSO;
}

// Armazena um registro
int armazenarRegistrobin(REGISTRO regLido, FILE* dest, int dif){ 
    char lixo = '$';

    fwrite(&regLido.tam_CidadeMae, sizeof(int), 1, dest);
    fwrite(&regLido.tam_CidadeBebe, sizeof(int), 1, dest);
    fwrite(regLido.cidadeMae, sizeof(char),regLido.tam_CidadeMae, dest);
    fwrite(regLido.cidadeBebe, sizeof(char), regLido.tam_CidadeBebe, dest);

    fseek(dest, dif, SEEK_CUR);
    
    int ate = 105 - 8 - dif - regLido.tam_CidadeMae -regLido.tam_CidadeBebe;
    for (int i = 0; i < ate; i++){
        fwrite(&lixo, sizeof(char), 1, dest);
    } 
    fwrite(&regLido.idNascimento, sizeof(int), 1, dest);
    fwrite(&regLido.idadeMae, sizeof(int), 1, dest);
    fwrite(&regLido.dataNascimento, sizeof(char), 10, dest);
    fwrite(&regLido.sexoBebe, sizeof(char), 1, dest);
    fwrite(&regLido.estadoMae, sizeof(char), 2, dest);
    fwrite(&regLido.estadoBebe, sizeof(char), 2, dest);

    return SUCESSO;
}

int criarCabecalhobin(CABECALHO* header, FILE* dest, unsigned char status){
    char lixo = '$';

    header->status =  status;
    fseek(dest, 0, SEEK_SET);
    fwrite(&header->status, sizeof(char), 1, dest);
    fwrite(&header->RRNproxRegistro, sizeof(int), 1, dest);
    fwrite(&header->numeroRegistrosInseridos, sizeof(int),1, dest);
    fwrite(&header->numeroRegistrosRemovidos, sizeof(int), 1, dest);
    fwrite(&header->numeroRegistrosAtualizados, sizeof(int), 1, dest);
    for(int i = 0; i < 111; i++) fwrite(&lixo, sizeof(char), 1, dest);
    
    return SUCESSO;
}

REGISTRO lerRegistro(FILE *csv){
    REGISTRO reg;

    int k = 0, j = 0;

    char entrada[140], str[8][128];

    fgets(entrada, 140, csv);

    for(int i = 0; i < 8; i++){
        j = 0;
        while(entrada[k] != '\n' && entrada[k] != ',' && entrada[k] != 10 ){
            str[i][j] = entrada[k];
            k++, j++;
        } 
        

        if(entrada[k] == ',' || entrada[k] != '\n' ||  entrada[k] == 10) str[i][j] = '\0', k++;
        trim(str[i]);
    }

    reg.tam_CidadeMae = strlen(str[0]);
    reg.cidadeMae = calloc(sizeof(char), reg.tam_CidadeMae);
    strncpy(reg.cidadeMae, str[0], reg.tam_CidadeMae);
    
    reg.tam_CidadeBebe = strlen(str[1]);
    reg.cidadeBebe = calloc(sizeof(char), reg.tam_CidadeBebe);
    strncpy(reg.cidadeBebe, str[1], reg.tam_CidadeBebe);
    reg.idNascimento = atoi(str[2]);
    reg.idadeMae = atoi(str[3]);
    if(reg.idadeMae == 0) reg.idadeMae = -1;
    strncpy(reg.dataNascimento, str[4], 10);
    if(reg.dataNascimento[0]=='\0'){
        for(int i = 1; i < 10; i++){
            reg.dataNascimento[i] = '$';
        }       
    }
    if(str[5][0] == '\0') reg.sexoBebe = '0';
    else reg.sexoBebe = str[5][0];
    strncpy(reg.estadoMae, str[6], 2); 
    if(reg.estadoMae[0] == '\0') reg.estadoMae[1] = '$';
    strncpy(reg.estadoBebe, str[7], 2);
    if(reg.estadoBebe[0] == '\0') reg.estadoBebe[1] = '$';

    return reg;
}

int lerBinario(FILE *bin){
    CABECALHO *header;
    REGISTRO reg;
    
    int i;

    if((header = lerCabecalhoBin(bin)) == NULL || header->status == '0'){
        free(header);
        return ERRO;
    }


    if(header->numeroRegistrosInseridos == 0){
        printf("Registro inexistente.\n");
        return SUCESSO;
    }
    
    fseek(bin, 128, SEEK_SET);

    for(i = 0; i < (header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos); i++){   
        if(encontrarRegistroBin(bin, i, &reg)) printf("Registro inexistente.\n");
        else imprimirRegistro(reg);

    }
    
    free(header);

    return SUCESSO;
}

// Busca por RRN
int buscaRRN(FILE * src, int RRN){
    CABECALHO *header;
    REGISTRO reg;

    fseek(src, 0, SEEK_SET);

    if((header = lerCabecalhoBin(src)) == NULL || header->status == '0'){
        free(header);
        return ERRO;
    }

    /*  Percorrer o arquivo ate encontrar o RRN, caso ele exista, imprimir as informacoes deste
    * caso nao exista, retornar erro, caso ja tenha sido deletado, retornar erro
    */

    if(header->numeroRegistrosInseridos == 0 || header->numeroRegistrosInseridos < RRN || encontrarRegistroBin(src, RRN, &reg) == regDeletado){
        printf("Registro inexistente.\n");
    } else{
        imprimirRegistro(reg);
    }
    
    free(header);

    return SUCESSO;

}

int encontrarRegistroBin(FILE *bin, int posReg, REGISTRO *aux){

    fseek(bin, 128 + (128 * posReg), SEEK_SET);


    fread(&(aux->tam_CidadeMae), sizeof(int), 1, bin);

    if(aux->tam_CidadeMae == -1) return regDeletado;

    fread(&(aux->tam_CidadeBebe), sizeof(int), 1, bin);
    
    aux->cidadeMae = calloc(aux->tam_CidadeMae + 1, sizeof(char));   
    aux->cidadeBebe = calloc(aux->tam_CidadeBebe + 2, sizeof(char));

    fread(aux->cidadeMae, sizeof(char), aux->tam_CidadeMae, bin);
    aux->cidadeMae[aux->tam_CidadeMae] = '\0';
    fread(aux->cidadeBebe, sizeof(char), aux->tam_CidadeBebe, bin);
    aux->cidadeBebe[aux->tam_CidadeBebe] = '\0';
    if(aux->tam_CidadeBebe == 0) strcpy(aux->cidadeBebe, "-");

    fseek(bin, 105 - ( 8 + aux->tam_CidadeMae + aux->tam_CidadeBebe ), SEEK_CUR);

    fread(&(aux->idNascimento), sizeof(int), 1, bin);
    fread(&(aux->idadeMae), sizeof(int), 1, bin);
    fread(&(aux->dataNascimento), sizeof(char), 10, bin);
    aux->dataNascimento[10] = '\0';
    if(strlen(aux->dataNascimento) == 0) strcpy(aux->dataNascimento, "-");
    fread(&(aux->sexoBebe), sizeof(char), 1, bin);
    fread(&(aux->estadoMae), sizeof(char), 2, bin);
    aux->estadoMae[2] = '\0';
    fread(&(aux->estadoBebe), sizeof(char), 2, bin);
    aux->estadoBebe[2] = '\0';
    if(strlen(aux->estadoBebe) == 0) strcpy(aux->estadoBebe, "-");


    return SUCESSO;
}

int imprimirRegistro(REGISTRO aux){

    char sexo[10];

    if(aux.sexoBebe == '0') strcpy(sexo, "IGNORADO");
    if(aux.sexoBebe == '1') strcpy(sexo, "MASCULINO"); 
    if(aux.sexoBebe == '2') strcpy(sexo, "FEMININO"); 

    printf("Nasceu em %s/%s, em %s, um bebê de sexo %s.\n", 
    aux.cidadeBebe, aux.estadoBebe, aux.dataNascimento, sexo);


    free(aux.cidadeMae);
    free(aux.cidadeBebe);

    return SUCESSO;
}  

int removerRegistroBin(FILE *src){
    int campos, i;
    REGISTRO *aux, reg;
    CABECALHO *header;

    fseek(src, 0, SEEK_SET);

    //mudar para inconsistente

    header = lerCabecalhoBin(src);

    if(header->status == '0'){
        free(header);
        return ERRO;
    }

    criarCabecalhobin(header, src, '0');

    scanf("%d", &campos);

    aux = definirCriteriosBusca(campos);
    
    for( i = 0; i < header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos; i++){ 
        // Caso o arquivo ja tenha sido removido, nao deve fazer nada
        if(encontrarRegistroBin(src, i, &reg) != regDeletado){
            // Deve-se mudar o valor do primeiro campo para -1 neste caso, TamCidadeMae, caso exista aquele registro
            // Todos os outros campos continuam sem alteracoes
            if(compararRegistro(&reg, aux)){
                free(reg.cidadeMae);
                free(reg.cidadeBebe);  
                continue;
            }

            // Muda para -1 no tamCidadeMae (sinal de remocao logica, combinado na documentacao)
            reg.tam_CidadeMae = -1;
            
            header->numeroRegistrosRemovidos ++; // Não passa no caso de teste se atualizar
            header->numeroRegistrosInseridos --; // Sugestao do grupo da sala, nao concordo
            

            fseek(src, 128 + (128*i), SEEK_SET);

            fwrite(&reg.tam_CidadeMae, sizeof(int), 1, src);
            
            free(reg.cidadeMae);
            free(reg.cidadeBebe);  
        }
    }

    criarCabecalhobin(header, src, '1');

    free(aux->cidadeMae);
    free(aux->cidadeBebe);
    free(aux);

    free(header);
    // Voltar para consistente
    return SUCESSO;
} 

int buscaCombinadaRegistro(FILE* src){
    int campos, i, flag = 0;
    REGISTRO *aux, reg;
    CABECALHO *header;

    header = lerCabecalhoBin(src);

    scanf("%d", &campos);

    aux = definirCriteriosBusca(campos);

    /*printf("id: %d\ncidade bb: %s\nestado: %s \ncidade mae: %s\n estado: %s\ndata nascimento: %s\n idade mae: %d\n sexo bebe: %c\n",
    aux->idNascimento, aux->cidadeBebe, aux->estadoBebe, aux->cidadeMae, aux->estadoMae,aux->dataNascimento, aux->idadeMae, aux->sexoBebe);*/

    for(i = 0; i < header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos ; i++){ // Ele vai ler todos registros
        if(!encontrarRegistroBin(src, i, &reg)){
            if(compararRegistro(&reg, aux)){
                if(reg.cidadeMae){
                    free(reg.cidadeMae);
                    reg.cidadeMae = NULL;
                }
                if(reg.cidadeBebe){
                    free(reg.cidadeBebe);
                    reg.cidadeBebe = NULL;
                }
                continue;
            } else{
                flag++;
                imprimirRegistro(reg);
            }
        }

    }

    free(header);
    free(aux->cidadeMae);
    free(aux->cidadeBebe);
    free(aux);

    if(!flag) printf("Registro inexistente.\n");

    return SUCESSO;
}

// Pesquisa por itens gravados no arquivo
REGISTRO* definirCriteriosBusca(int campos){
    
    char campo[16];
    char entrada[100];
    REGISTRO *aux = calloc(1, sizeof(REGISTRO));

    aux->idNascimento = -1;

    aux->cidadeBebe = calloc(100, sizeof(char));
    aux->cidadeMae = calloc(100, sizeof(char));

    for(int i = 0; i < campos; i++){
        scanf("%s", campo);
        scan_quote_string(entrada);

        /* O usuario pode fazer qualquer combinacao de campos do registro,
        * receber diversos itens na chamada da funcao, elas sera usadas para criar um "fitro"
        * e retornar todos os itens que correspondem a ele
        */
        if(!strcmp(campo, "idNascimento")) aux->idNascimento = atoi(entrada);
        if(!strcmp(campo, "idadeMae")) aux->idadeMae = atoi(entrada);
        if(!strcmp(campo, "dataNascimento")) strcpy(aux->dataNascimento, entrada);
        if(!strcmp(campo, "sexoBebe")) aux->sexoBebe = entrada[0];
        if(!strcmp(campo, "estadoMae")) strcpy(aux->estadoMae, entrada);        
        if(!strcmp(campo, "estadoBebe")) strcpy(aux->estadoBebe, entrada);                    
        if(!strcmp(campo, "cidadeMae")) strcpy(aux->cidadeMae, entrada);  
        if(!strcmp(campo, "cidadeBebe")) strcpy(aux->cidadeBebe, entrada);
    }
    
    
    return aux;
}

int compararRegistro(REGISTRO *src, REGISTRO *aux){

    if(((aux->idNascimento) != -1)){ if(aux->idNascimento != src->idNascimento) return ERRO;}
    if((aux->idadeMae) && (aux->idadeMae != src->idadeMae)) return ERRO;
    if((strlen(aux->dataNascimento)) && (strcmp(aux->dataNascimento, src->dataNascimento))) return ERRO;
    if((aux->sexoBebe) && aux->sexoBebe != src->sexoBebe) return ERRO;
    if(strlen(aux->estadoMae) && (strcmp(aux->estadoMae, src->estadoMae))) return ERRO;
    if(strlen(aux->estadoBebe) && (strcmp(aux->estadoBebe, src->estadoBebe))) return ERRO;      
    if(strlen(aux->cidadeMae) && (strcmp(aux->cidadeMae, src->cidadeMae))) return ERRO;                  
    if(strlen(aux->cidadeBebe) && (strcmp(aux->cidadeBebe, src->cidadeBebe))) return ERRO;

    return SUCESSO;
}

int atualizarRegistroBin(FILE* src, int RRN){
    REGISTRO reg;
    CABECALHO *header;
    char campo[16];
    char entrada[100];
    int campos, ant, new, dif;

    fseek(src, 0, SEEK_SET);
    
    header = lerCabecalhoBin(src);

    if(header->status == '0'){
        free(header);
        return ERRO;
    }
    
    // Verificar a existencia do registro
    if(header->numeroRegistrosInseridos == 0 || 
        header->numeroRegistrosInseridos < RRN || encontrarRegistroBin(src, RRN, &reg) == regDeletado){
            free(reg.cidadeBebe);
            free(reg.cidadeMae);
            free(header);
            return SUCESSO;
        }

    // Define status como inconsistente
    criarCabecalhobin(header, src, '0');

    /******** Mudar a "definir criterio de busca" *************/
    
    scanf("%d", &campos);

    for(int i = 0; i < campos; i++){
        scanf("%s", campo);
        scan_quote_string(entrada);

        if(!strcmp(entrada,"NULO")) entrada[0] = '\0';

        /* O usuario pode fazer qualquer combinacao de campos do registro,
        * receber diversos itens na chamada da funcao, elas sera usadas para criar um "fitro"
        * e retornar todos os itens que correspondem a ele
        */
        if(!strcmp(campo, "idNascimento")) reg.idNascimento = atoi(entrada);
        if(!strcmp(campo, "idadeMae")) {
            if(entrada[0]  == '\0') reg.idadeMae = -1;
            else reg.idadeMae = atoi(entrada);}
        if(!strcmp(campo, "dataNascimento")) strcpy(reg.dataNascimento, entrada);
        if(!strcmp(campo, "sexoBebe")) reg.sexoBebe = entrada[0];
        if(!strcmp(campo, "estadoMae")) strcpy(reg.estadoMae, entrada);        
        if(!strcmp(campo, "estadoBebe")) strcpy(reg.estadoBebe, entrada);                    
        if(!strcmp(campo, "cidadeMae")) strcpy(reg.cidadeMae, entrada);  
        if(!strcmp(campo, "cidadeBebe")) strcpy(reg.cidadeBebe, entrada);
    }
    

    fseek(src, (RRN * 128) + 128, SEEK_SET);

    ant = (reg.tam_CidadeMae + reg.tam_CidadeBebe); // Esse é o tamanho antigo

    reg.tam_CidadeMae = strlen(reg.cidadeMae);
    reg.tam_CidadeBebe = strlen(reg.cidadeBebe);
    
    new = (reg.tam_CidadeMae + reg.tam_CidadeBebe);
    
    dif = ant - new;

    dif = maior(0, dif);

    armazenarRegistrobin(reg, src,  dif);

    header->numeroRegistrosAtualizados++;

    criarCabecalhobin(header, src, '1');

    free(reg.cidadeBebe);
    free(reg.cidadeMae);

    free(header);

    return SUCESSO;
    
}

int inserirRegistro(FILE* bin){
    CABECALHO *header;
    REGISTRO aux;
    char entrada[100];
    int pos, i;

    fseek(bin, 0, SEEK_SET);

    header = lerCabecalhoBin(bin);

    if(header->status == '0'){
        free(header);
        return ERRO;
    }

    criarCabecalhobin(header, bin, '0');
    
    if(header->numeroRegistrosRemovidos > 0){
        for(pos = 0; header->numeroRegistrosRemovidos + header->numeroRegistrosInseridos;pos++)
            if(encontrarRegistroBin(bin, pos, &aux) == regDeletado){
                header->numeroRegistrosRemovidos--;
                break;
            }
    } else{ 
        pos = header->RRNproxRegistro;
        header->RRNproxRegistro++;
    }

    aux.cidadeBebe = calloc(100, sizeof(char));
    aux.cidadeMae = calloc(100, sizeof(char));

    //cidade mae
    scan_quote_string(entrada);
    if(!(strcmp(entrada, "NULO"))) entrada[0] = '\0';
    strcpy(aux.cidadeMae, entrada);
    printf("entrada: %s\n", entrada);

    //cidade bebe
    scan_quote_string(entrada);
    if(!(strcmp(entrada, "NULO"))) entrada[0] = '\0';
    strcpy(aux.cidadeBebe, entrada);
    printf("entrada: %s\n", entrada);

    //id Nascimento
    scan_quote_string(entrada);
    aux.idNascimento = atoi(entrada);
    printf("entrada: %s\n", entrada);

    //idade mae
    scan_quote_string(entrada);
    if(!(strcmp(entrada, "NULO"))) aux.idadeMae = -1;
    else aux.idadeMae = atoi(entrada);

    
    printf("entrada: %s\n", entrada);

    //data nascimento
    scan_quote_string(entrada); 
    if(!(strcmp(entrada, "NULO"))) {
        printf("ENTRA AQUI?");
        aux.dataNascimento[0] = '\0';
        for( i = 1; i < 10; i++){
            aux.dataNascimento[i] = '$';
        }       
    } else strcpy(aux.dataNascimento, entrada);

    printf("entrada: %s <-%c->\n", entrada, aux.dataNascimento[1]);

    //sexo bebe
    scan_quote_string(entrada);
    if(!(strcmp(entrada, "NULO"))) entrada[0] = '0';
    aux.sexoBebe = entrada[0];

    //estado mae
    scan_quote_string(entrada); 
    if(!(strcmp(entrada, "NULO"))){
        entrada[0] = '\0';
        entrada[1] = '$';
    }
    else strcpy(aux.estadoMae, entrada);

    //estado bebe
    scan_quote_string(entrada); 
    if(!(strcmp(entrada, "NULO"))) {
        entrada[0] = '\0';
        entrada[1] = '$';
    }
    else strcpy(aux.estadoBebe, entrada);

    aux.tam_CidadeBebe = (int) strlen(aux.cidadeBebe);
    aux.tam_CidadeMae = (int) strlen(aux.cidadeMae);
    
    fseek(bin, 128 + (128 * pos), SEEK_SET); //posição

    armazenarRegistrobin(aux, bin, 0);
    
    header->numeroRegistrosInseridos++;
    
    criarCabecalhobin(header, bin, '1');

    free(header);
    free(aux.cidadeMae);
    free(aux.cidadeBebe);

    return SUCESSO;
}


