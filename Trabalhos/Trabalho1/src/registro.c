#include <stdio.h>
#include <stdlib.h>
#include <registro.h>
#include <string.h>


/* Registro Cabeçalho (header record) 
 * Em geral, é interessante manter algumas informações sobre o arquivo
 * para uso futuro 
 * Essas informações podem ser mantidas em um cabeçalho no início do arquivo
 * A existência de um registro cabeçalho torna um arquivo um objeto auto-descrito
 * o software pode acessar arquivos de forma mais flexível, porém o software tem 
 * que ser mais elaborado 
 */ 

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

// No arquivo .csv, o separador de campos é vírgula `,`
/* Em armazenar registro devemos seguir a ordem
 * tamanho do campo cidadeMae
 * tamanho do campo cidadeBebe
 * cidadeMae
 * cidadeBebe
 * idNascimento
 * idadeMae
 * dataNascimento
 * sexoBebe
 * estadoMae
 * estadoBebe
 */ 

// Para campos de tamanho fixo
/* se o campo é inteiro ou de dupla precisão, então armazena-se o valor -1
 * se o campo é do tipo string, então armazena-se ‘\0$$$$$$$$$$$’
 */
// Para campos de tamanho variavel
// deve ser armazenado apenas o indicador de tamanho do campo, o qual deve possuir ovalor 0.

// Em remover registros, eles serao removidos logicamente
/* No campo 1 no inicio do registro, "tamanho do campo cidadeMae" caso seja armazenado -1, 
 * o campo deve ser considerado logicamente removido
 */

// Em imprimir lixo e logicamente removidos devem ser tratados para nao aparecer na tela, se for nulo deve ser representado por `-`
// Caso sucesso
// "Nasceu em " cidadeBebe "/" estadoBebe ", em " dataNascimento ", um bebe de sexo " sexoBebe "." 
// Em sexo do bebe deve imprimir: "IGNORADO" caso sexoBebe  = ‘0’,  "MASCULINO"  caso  sexoBebe  = ‘1’e  "FEMININO"  caso sexoBebe  = ‘2’.  
// Caso registro inexistente
// "Registro inexistente."

CABECALHO* lerCabecalhoBin(FILE* bin){
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

int lerBinario(FILE*bin){
    CABECALHO *header;
    REGISTRO aux;
    
    int i;

    if((header = lerCabecalhoBin(bin)) == NULL || header->status == 0) return ERRO;


    if(header->numeroRegistrosInseridos == 0){
        printf("Registro inexistente.\n");
        return SUCESSO;
    }
    
    fseek(bin, 128, SEEK_SET);

    for(i = 0; i < header->numeroRegistrosInseridos; i++){
        fseek(bin, 128 + (128 * i), SEEK_SET);
        imprimirRegistroBin(bin);
    }
    
    free(header);
}

int imprimirRegistroBin(FILE *bin){
    REGISTRO aux;
    char sexo[10];

    fread(&(aux.tam_CidadeMae), sizeof(int), 1, bin);

    if(aux.tam_CidadeMae == -1) return regDeletado;

    fread(&(aux.tam_CidadeBebe), sizeof(int), 1, bin);
    
    aux.cidadeMae = calloc(aux.tam_CidadeMae + 1, sizeof(char));
    aux.cidadeBebe = calloc(aux.tam_CidadeBebe + 2, sizeof(char));

    fread(aux.cidadeMae, sizeof(char), aux.tam_CidadeMae, bin);
    aux.cidadeMae[aux.tam_CidadeMae] = '\0';
    fread(aux.cidadeBebe, sizeof(char), aux.tam_CidadeBebe, bin);
    aux.cidadeBebe[aux.tam_CidadeBebe] = '\0';
    if(aux.tam_CidadeBebe == 0) strcpy(aux.cidadeBebe, "-");

    fseek(bin, 105 - ( 8 + aux.tam_CidadeMae + aux.tam_CidadeBebe ), SEEK_CUR);

    fread(&(aux.idNascimento), sizeof(int), 1, bin);
    fread(&(aux.idadeMae), sizeof(int), 1, bin);
    fread(&(aux.dataNascimento), sizeof(char), 10, bin);
    aux.dataNascimento[10] = '\0';
    if(strlen(aux.dataNascimento) == 0) strcpy(aux.dataNascimento, "-");
    fread(&(aux.sexoBebe), sizeof(char), 1, bin);
    fread(&(aux.estadoMae), sizeof(char), 2, bin);
    aux.estadoMae[2] = '\0';
    fread(&(aux.estadoBebe), sizeof(char), 2, bin);
    aux.estadoBebe[2] = '\0';
    if(strlen(aux.estadoBebe) == 0) strcpy(aux.estadoBebe, "-");
    

    if(aux.sexoBebe == '0') strcpy(sexo, "IGNORADO");
    if(aux.sexoBebe == '1') strcpy(sexo, "MASCULINO"); 
    if(aux.sexoBebe == '2') strcpy(sexo, "FEMININO"); 

    printf("Nasceu em %s/%s, em %s, um bebe de sexo %s.\n", 
    aux.cidadeBebe, aux.estadoBebe, aux.dataNascimento, sexo);

    free(aux.cidadeMae);
    free(aux.cidadeBebe);
}
