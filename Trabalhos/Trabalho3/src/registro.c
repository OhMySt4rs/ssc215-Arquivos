/* Criado por Mateus de Souza Santos 11366913
    e Wellington Matos Amaral 11315054
*/

#include <stdio.h>
#include <stdlib.h>
#include <registro.h>
#include <string.h>
#include <binarionatela.h>

#define maior(a, b) (a >= b ? a : b)

CABECALHO* lerCabecalhoBin(FILE* bin){ // Ler de um arquivo binário o cabeçalho e o retorna 
    CABECALHO *aux = calloc(1, sizeof(CABECALHO)); 
    
    // Caso haja falha na alocacao, nao insere-se nada no cabecalho
    if(aux == NULL) return aux;
    
    // Inicializacao de todos os campos do cabecalho
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
    
    // Caso existam problemas na alocacao do cabecalho retorna erro na criacao
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

    // Liberar campos dinamicos do registrador
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

    // Escreve os campos de tamanho variavel
    fwrite(&regLido.tam_CidadeMae, sizeof(int), 1, dest);
    fwrite(&regLido.tam_CidadeBebe, sizeof(int), 1, dest);
    fwrite(regLido.cidadeMae, sizeof(char),regLido.tam_CidadeMae, dest);
    fwrite(regLido.cidadeBebe, sizeof(char), regLido.tam_CidadeBebe, dest);

    // Ajusta o ponteiro para o inicio do espaco que sobram do campo de tamanho variavel
    fseek(dest, dif, SEEK_CUR);
    
    // Preenche todos os bytes com "lixo"
    int ate = 105 - 8 - dif - regLido.tam_CidadeMae -regLido.tam_CidadeBebe;
    for (int i = 0; i < ate; i++){
        fwrite(&lixo, sizeof(char), 1, dest);
    } 

    // Escreve os campos de tamanho fixo
    fwrite(&regLido.idNascimento, sizeof(int), 1, dest);
    fwrite(&regLido.idadeMae, sizeof(int), 1, dest);
    fwrite(&regLido.dataNascimento, sizeof(char), 10, dest);
    fwrite(&regLido.sexoBebe, sizeof(char), 1, dest);
    fwrite(&regLido.estadoMae, sizeof(char), 2, dest);
    fwrite(&regLido.estadoBebe, sizeof(char), 2, dest);

    return SUCESSO;
}

// Inicializa um cabecalho
int criarCabecalhobin(CABECALHO* header, FILE* dest, unsigned char status){
    char lixo = '$';

    // Ajusta o estado (consistente ou nao) do arquivo
    header->status =  status;
    // Ajusta o ponteiro para o inicio do arquivo
    fseek(dest, 0, SEEK_SET);
    // Escreve as informacoes armazenadas no cabecalho auxiliar no arquivo
    fwrite(&header->status, sizeof(char), 1, dest);
    fwrite(&header->RRNproxRegistro, sizeof(int), 1, dest);
    fwrite(&header->numeroRegistrosInseridos, sizeof(int),1, dest);
    fwrite(&header->numeroRegistrosRemovidos, sizeof(int), 1, dest);
    fwrite(&header->numeroRegistrosAtualizados, sizeof(int), 1, dest);
    for(int i = 0; i < 111; i++) fwrite(&lixo, sizeof(char), 1, dest);
    
    return SUCESSO;
}

// Ler registro
REGISTRO lerRegistro(FILE *csv){
    REGISTRO reg;

    int k = 0, j = 0;

    char entrada[140], str[8][128];

    // Legenda da tabela que nao sera util
    fgets(entrada, 140, csv);

    // Leitura de cada um dos oito campos da tabela 
    for(int i = 0; i < 8; i++){
        j = 0;

        // Os campos sao separados por ',' ou ate o fim do da linha
        while(entrada[k] != '\n' && entrada[k] != ',' && entrada[k] != 10 ){
            str[i][j] = entrada[k];
            k++, j++;
        } 
        
        // Ajustar as strings para um formato adequado para posterior manipulacao
        if(entrada[k] == ',' || entrada[k] != '\n' ||  entrada[k] == 10) str[i][j] = '\0', k++;
        trim(str[i]);
    }

    // Escrever em um registro auxiliar asa informacoes adquiridas anteriormente
    // Campos de tamanho variavel
    reg.tam_CidadeMae = strlen(str[0]);
    reg.cidadeMae = calloc(sizeof(char), reg.tam_CidadeMae);
    strncpy(reg.cidadeMae, str[0], reg.tam_CidadeMae);
    
    // Campos de tamanho fixo
    reg.tam_CidadeBebe = strlen(str[1]);
    reg.cidadeBebe = calloc(sizeof(char), reg.tam_CidadeBebe);
    strncpy(reg.cidadeBebe, str[1], reg.tam_CidadeBebe);
    reg.idNascimento = atoi(str[2]);
    reg.idadeMae = atoi(str[3]);

    // Algumas verificacoes de valores invalidos e o seu devido tratamento segundo as especificacoes
    if(reg.idadeMae == 0) reg.idadeMae = -1;
    strncpy(reg.dataNascimento, str[4], 10);
    if(reg.dataNascimento[0]=='\0'){
        for(int i = 1; i < 10; i++){
            reg.dataNascimento[i] = '$';
        }       
    }

    // Outras verificacoes caso hajam campos com informacoes invalidas
    if(str[5][0] == '\0') reg.sexoBebe = '0';
    else reg.sexoBebe = str[5][0];
    strncpy(reg.estadoMae, str[6], 2); 
    if(reg.estadoMae[0] == '\0') reg.estadoMae[1] = '$';
    strncpy(reg.estadoBebe, str[7], 2);
    if(reg.estadoBebe[0] == '\0') reg.estadoBebe[1] = '$';

    return reg;
}

// Leirua do arquivo binario
int lerBinario(FILE *bin){
    CABECALHO *header;
    REGISTRO reg;
    
    int i;

    // Casp p cabecalho estaja vazio ou inconsistente, termina-se a operacao
    if((header = lerCabecalhoBin(bin)) == NULL || header->status == '0'){
        free(header);
        return ERRO;
    }

    // Caso nao hajam registros inseridos, nao ha operacoes a fazer
    if(header->numeroRegistrosInseridos == 0){
        printf("Registro Inexistente.\n");
        return SUCESSO;
    }
    
    // Ajusta o ponteiro para apos o cabecalho, inicio do primeiro registro
    fseek(bin, 128, SEEK_SET);

    // Leitura de todos os registros existentes
    for(i = 0; i < (header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos); i++){   
        // Caso seja um registro invalido, retorna um erro
        if(encontrarRegistroBin(bin, i, &reg)) printf("Registro Inexistente.\n");
        // Caso contrario, imprime todas as informacoes pertinentes ao registro
        else imprimirRegistro(reg);

    }
    
    // Liberar a variavel auxiliar
    free(header);

    return SUCESSO;
}

// Busca por RRN
int buscaRRN(FILE * src, int RRN){
    CABECALHO *header;
    REGISTRO reg;

    // Ajusta o ponteiro no inicio do cabecalho
    fseek(src, 0, SEEK_SET);

    // Verificar a consistencia do arquivo
    if((header = lerCabecalhoBin(src)) == NULL || header->status == '0'){
        free(header);
        return ERRO;
    }

    /*  Percorrer o arquivo ate encontrar o RRN, caso ele exista, imprimir as informacoes deste
    * caso nao exista, retornar erro, caso ja tenha sido deletado, retornar erro
    */

    if(header->numeroRegistrosInseridos == 0 || header->numeroRegistrosInseridos < RRN || encontrarRegistroBin(src, RRN, &reg) == regDeletado){
        printf("Registro Inexistente.\n");
    } else{
        imprimirRegistro(reg);
    }
    
    // Liberar variavel auxiliar
    free(header);

    return SUCESSO;

}

// Busca por registro utilizando posicao
int encontrarRegistroBin(FILE *bin, int posReg, REGISTRO *aux){

    // Ajusta o ponteiro na posicao desejada
    fseek(bin, 128 + (128 * posReg), SEEK_SET);

    // Le o campo tamCidadeMae
    fread(&(aux->tam_CidadeMae), sizeof(int), 1, bin);

    // Caso esteja sinalizado como logicamente deletado, retorna
    if(aux->tam_CidadeMae == -1) return regDeletado;

    // Leitura do proximo campo de tamanho varivavel
    fread(&(aux->tam_CidadeBebe), sizeof(int), 1, bin);
    
    // Alocacao de variaveis auxiliares para os campos de tamanho variavel
    aux->cidadeMae = calloc(aux->tam_CidadeMae + 1, sizeof(char));   
    aux->cidadeBebe = calloc(aux->tam_CidadeBebe + 2, sizeof(char));

    // Leitura dos campos de tamanho variavel
    fread(aux->cidadeMae, sizeof(char), aux->tam_CidadeMae, bin);
    aux->cidadeMae[aux->tam_CidadeMae] = '\0';
    fread(aux->cidadeBebe, sizeof(char), aux->tam_CidadeBebe, bin);
    aux->cidadeBebe[aux->tam_CidadeBebe] = '\0';
    // Teste para verificar se a informacao eh valida e seu devido tratamento
    if(aux->tam_CidadeBebe == 0) strcpy(aux->cidadeBebe, "-");

    // Ajuste do ponteiro para os campos de tamanho fixo
    fseek(bin, 105 - ( 8 + aux->tam_CidadeMae + aux->tam_CidadeBebe ), SEEK_CUR);

    // Leitura dos campos de tamanho fixo
    fread(&(aux->idNascimento), sizeof(int), 1, bin);
    fread(&(aux->idadeMae), sizeof(int), 1, bin);
    fread(&(aux->dataNascimento), sizeof(char), 10, bin);
    aux->dataNascimento[10] = '\0';
    // Verifica se as informacoes sao validas e o seu devido tratamento
    if(strlen(aux->dataNascimento) == 0) strcpy(aux->dataNascimento, "-");
    fread(&(aux->sexoBebe), sizeof(char), 1, bin);
    fread(&(aux->estadoMae), sizeof(char), 2, bin);
    aux->estadoMae[2] = '\0';
    fread(&(aux->estadoBebe), sizeof(char), 2, bin);
    aux->estadoBebe[2] = '\0';
    // Verifica se as informacoes sao validas e o seu devido tratamento
    if(strlen(aux->estadoBebe) == 0) strcpy(aux->estadoBebe, "-");

    return SUCESSO;
}

// Imprimir registro
int imprimirRegistro(REGISTRO aux){

    char sexo[10];

    // Decifrar informacoes salvas sobre o sexo do bebe
    if(aux.sexoBebe == '0') strcpy(sexo, "IGNORADO");
    if(aux.sexoBebe == '1') strcpy(sexo, "MASCULINO"); 
    if(aux.sexoBebe == '2') strcpy(sexo, "FEMININO"); 

    // Formatacao presente na especificacao
    printf("Nasceu em %s/%s, em %s, um bebê de sexo %s.\n", 
    aux.cidadeBebe, aux.estadoBebe, aux.dataNascimento, sexo);

    // Liberar variaveis auxiliares
    free(aux.cidadeMae);
    free(aux.cidadeBebe);

    return SUCESSO;
}  

// Remover registro
int removerRegistroBin(FILE *src){
    int campos, i;
    REGISTRO *aux, reg;
    CABECALHO *header;

    // Ajusta o ponteiro para o inicio do cabecalho
    fseek(src, 0, SEEK_SET);

    
    header = lerCabecalhoBin(src);

    // Verificar se o arquivo eh consistente
    if(header->status == '0'){
        free(header);
        return ERRO;
    }

    // Mudar para inconsistente
    criarCabecalhobin(header, src, '0');

    // Ler quantidade de campos que serao pesquisaods
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
            
            //Seta o ponteiro para o registro  especifíco
            fseek(src, 128 + (128*i), SEEK_SET);

            fwrite(&reg.tam_CidadeMae, sizeof(int), 1, src);
            
            // Liberar variaveis auxiliares
            free(reg.cidadeMae);
            free(reg.cidadeBebe);  
        }
    }

    // Salvar como consistente
    criarCabecalhobin(header, src, '1');

    // Liberar variaveis auxiliares
    free(aux->cidadeMae);
    free(aux->cidadeBebe);
    free(aux);
    free(header);

    return SUCESSO;
} 

// Busca com campos do registro
int buscaCombinadaRegistro(FILE* src){
    int campos, i, flag = 0;
    REGISTRO *aux, reg;
    CABECALHO *header;

    header = lerCabecalhoBin(src);

    // Verificar se o arquivo eh consistente
    if(header->status == '0'){
        free(header);
        return ERRO;
    }

    // Quantidade de campos 
    scanf("%d", &campos);

    aux = definirCriteriosBusca(campos);

    // Ele vai ler todos registros
    for(i = 0; i < header->numeroRegistrosInseridos + header->numeroRegistrosRemovidos ; i++){ 
        // Salva no reg um registro existente
        if(!encontrarRegistroBin(src, i, &reg)){    
            // Teste de registro
            if(compararRegistro(&reg, aux)){        
                // Caso o campo seja diferente exista, libera-se o espaco alocado
                if(reg.cidadeMae){
                    free(reg.cidadeMae);
                    reg.cidadeMae = NULL;
                }
                // Caso o campo seja diferente exista, libera-se o espaco alocado
                if(reg.cidadeBebe){
                    free(reg.cidadeBebe);
                    reg.cidadeBebe = NULL;
                }
                continue;
            } 
            // Se nao atender os criterios vai para o próximo registro caso contrário o imprime
            else{
                flag++;
                imprimirRegistro(reg);
            }
        }

    }

    // Liberar variaveis auxiliares
    free(header);
    free(aux->cidadeMae);
    free(aux->cidadeBebe);
    free(aux);

    // Caso nada tenha sido imprimido anteriormente, uma mensagem de erro eh emitida
    if(!flag) printf("Registro Inexistente.\n");

    return SUCESSO;
}

// Pesquisa por itens gravados no arquivo
REGISTRO* definirCriteriosBusca(int campos){
    char campo[16];
    char entrada[100];
    REGISTRO *aux = calloc(1, sizeof(REGISTRO));

    // Pressets iniciais para ajuste da variavel auxuliar
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

// Confere se o registro src atende todos critérios do registro aux
int compararRegistro(REGISTRO *src, REGISTRO *aux){

    // A cada etapa eh feito um verificacao, caso falhe, ele retorna a que falhou
    if(((aux->idNascimento) != -1)){ if(aux->idNascimento != src->idNascimento) return ERRO;}
    if((aux->idadeMae) && (aux->idadeMae != src->idadeMae)) return ERRO;
    if((strlen(aux->dataNascimento)) && (strcmp(aux->dataNascimento, src->dataNascimento))) return ERRO;
    if((aux->sexoBebe) && aux->sexoBebe != src->sexoBebe) return ERRO;
    if(strlen(aux->estadoMae) && (strcmp(aux->estadoMae, src->estadoMae))) return ERRO;
    if(strlen(aux->estadoBebe) && (strcmp(aux->estadoBebe, src->estadoBebe))) return ERRO;      
    if(strlen(aux->cidadeMae) && (strcmp(aux->cidadeMae, src->cidadeMae))) return ERRO;                  
    if(strlen(aux->cidadeBebe) && (strcmp(aux->cidadeBebe, src->cidadeBebe))) return ERRO;

    // Caso tenha alcancado este ponto, entao todos os campos pesquisados sao iguais
    return SUCESSO;
}

// Atualizar registro existente
int atualizarRegistroBin(FILE* src, int RRN){
    REGISTRO reg;
    CABECALHO *header;
    char campo[16];
    char entrada[100];
    int campos, ant, new, dif;

    // Ajustar o ponteiro para o inicio do arquivo
    fseek(src, 0, SEEK_SET);
    
    // Verificar se o arquivos eh consistente
    header = lerCabecalhoBin(src);

    if(header->status == '0'){
        free(header);
        return ERRO;
    }
    
    // Verificar a existencia do registro
    if(header->numeroRegistrosInseridos == 0 || 
        header->numeroRegistrosInseridos < RRN || encontrarRegistroBin(src, RRN, &reg) == regDeletado){
            free(header);
            return SUCESSO;
        }

    // Define status como inconsistente
    criarCabecalhobin(header, src, '0');

    
    scanf("%d", &campos);

    for(int i = 0; i < campos; i++){
        scanf("%s", campo);
        scan_quote_string(entrada);

        if(!strcmp(entrada,"")) entrada[0] = '\0';

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
    

    // Ajusta o ponteiro para o local do RRN encontrado anteriormente
    fseek(src, (RRN * 128) + 128, SEEK_SET);

    // Esse é o tamanho antigo
    ant = (reg.tam_CidadeMae + reg.tam_CidadeBebe); 
    // Estes sao os novos tamanhos
    reg.tam_CidadeMae = strlen(reg.cidadeMae);
    reg.tam_CidadeBebe = strlen(reg.cidadeBebe);
    new = (reg.tam_CidadeMae + reg.tam_CidadeBebe);
    
    /* Caso o novo seja maior que o tamanho antigo, todas as informacoes anteriores serao cobertas
     * Caso contrario, algumas informacoes do ultimo registro ainda estarao presentres, porem a 
     * a nova informacao estara sinalizada onde sera o seu limite
     */
    dif = ant - new;
    dif = maior(0, dif);

    // Guarda no registro
    armazenarRegistrobin(reg, src,  dif);

    // Incrementa no cabecalho
    header->numeroRegistrosAtualizados++;

    // Atualiza o cabecalho
    criarCabecalhobin(header, src, '1');

    // Libera variaveis auxiliares
    free(reg.cidadeBebe);
    free(reg.cidadeMae);
    free(header);

    return SUCESSO;
}

// Insere novo registro
int inserirRegistro(FILE* bin, int *idNascimento, int *Pr){
    CABECALHO *header;
    REGISTRO aux;
    char entrada[100];
    int pos, i;

    // Ajusta o ponteiro para o inicio do arquivo
    fseek(bin, 0, SEEK_SET);

    header = lerCabecalhoBin(bin);

    // Verifica se o arquivo eh consistente
    if(header->status == '0'){
        free(header);
        return ERRO;
    }

    // Ajusta como inconsistente
    criarCabecalhobin(header, bin, '0');

    /* Caso alguem ja tenha sido removido, este espaco de memoria sera reutilizado, pos na 
     * funcao encontrar resgisto, ela emite um sinal caso o registro ja tenha sido removido
     * anteriormente
     */
    if(header->numeroRegistrosRemovidos > 0){
        for(pos = 0; header->numeroRegistrosRemovidos + header->numeroRegistrosInseridos;pos++)
            if(encontrarRegistroBin(bin, pos, &aux) == regDeletado){
                header->numeroRegistrosRemovidos--;
                break;
            }
    } 
    // Caso contrario, insere-se no fim do arquivo
    else{ 
        pos = header->RRNproxRegistro;
        header->RRNproxRegistro++;
    }

    *Pr = pos;

    // Alocacao das variaveis auxiliares
    aux.cidadeBebe = calloc(100, sizeof(char));
    aux.cidadeMae = calloc(100, sizeof(char));

    // Receber e guardar as informacoes de tamanho variaveis

    //cidade mae
    scan_quote_string(entrada);
    strcpy(aux.cidadeMae, entrada);

    //cidade bebe
    scan_quote_string(entrada);
    strcpy(aux.cidadeBebe, entrada);

    // Receber e guardar as informacoes de tamanho fixo

    //id Nascimento
    scan_quote_string(entrada);
    aux.idNascimento = atoi(entrada);
    *idNascimento = aux.idNascimento;

    //idade mae
    scan_quote_string(entrada);
    // Teste caso as informacoes recebidas sao validas e o seu tratamento
    if(!(strcmp(entrada, ""))) aux.idadeMae = -1;
    else aux.idadeMae = atoi(entrada);

    //data nascimento
    scan_quote_string(entrada); 
    // Teste caso as informacoes recebidas sao validas e o seu tratamento
    if(!(strcmp(entrada, ""))) {
        aux.dataNascimento[0] = '\0';
        for( i = 1; i < 10; i++){
            aux.dataNascimento[i] = '$';
        }       
    }else strcpy(aux.dataNascimento, entrada);

    //sexo bebe
    scan_quote_string(entrada);
    // Teste caso as informacoes recebidas sao validas e o seu tratamento
    if(!(strcmp(entrada, ""))) entrada[0] = '0';
    aux.sexoBebe = entrada[0];

    //estado mae
    scan_quote_string(entrada); 
    // Teste caso as informacoes recebidas sao validas e o seu tratamento
    if(!(strcmp(entrada, ""))){
        aux.estadoMae[0] = '\0';
        aux.estadoMae[1] = '$';
    }
    else strcpy(aux.estadoMae, entrada);

    //estado bebe
    scan_quote_string(entrada); 
    // Teste caso as informacoes recebidas sao validas e o seu tratamento
    if(!(strcmp(entrada, ""))) {
        aux.estadoBebe[0] = '\0';
        aux.estadoBebe[1] = '$';
    }
    else strcpy(aux.estadoBebe, entrada);

    // Atualizacao do tamanho dos campos variaveis
    aux.tam_CidadeBebe = (int) strlen(aux.cidadeBebe);
    aux.tam_CidadeMae = (int) strlen(aux.cidadeMae);
    
    // Ajuste do ponteiro para a posicao que sera armazenado (definido anteriormente)
    fseek(bin, 128 + (128 * pos), SEEK_SET); //posição

    // Armazenar no arquivo
    armazenarRegistrobin(aux, bin, 0);
    
    // Atualizar o cabecalho
    header->numeroRegistrosInseridos++;
    
    criarCabecalhobin(header, bin, '1');

    // Liberar informacoes auxiliares
    free(header);
    free(aux.cidadeMae);
    free(aux.cidadeBebe);

    return SUCESSO;
}


