/* Registro Cabeçalho (header record) 
 * Em geral, é interessante manter algumas informações sobre o arquivo
 * para uso futuro 
 * Essas informações podem ser mantidas em um cabeçalho no início do arquivo
 * A existência de um registro cabeçalho torna um arquivo um objeto auto-descrito
 * o software pode acessar arquivos de forma mais flexível, porém o software tem 
 * que ser mais elaborado 
 */ 

typedef struct registroCabecalho{
    char status;                    // Sempre que uma modificacao inicia assume `0`, se concluir com sucesso assume `1`
    int RRNproxRegistro;            // Byteoffset do proximo registro
    int numeroRegistrosInseridos;   // Inicialmente vale 0 e incrementa sempre que existir um novo registro
    int numeroRegistrosRemovidos;   // Os registros ser'ao logicamente removidos, incrementam sempre que isso ocorrer
    int numeroRegistrosAtualizados; // Sempre que alguma informacao em um registro for atualizada, ele sera incrmentado
    char lixo[111];                 // Armazena apenas `$` para manter o padrao de tamanho de 128 bites
}CABECALHO;

/* Organizacao hibrida de registros, haverao campos de tamanho fixo, campo de tamanho variavel 
 * (indicando o tamanho do campo) e delimitador entre registros `#`
 */

typedef struct registro{

    // Campos de tamanho fixo: tamanho maximo de 23 bytes
    int idNascimento;               // código sequencial que identifica univocamente cada registro do arquivo de dados)
    int idadeMae;                   // idade da mãe do bebê
    char dataNascimento[10];        // formato AAAA-MM-DD; data de nascimento do bebê
    char sexoBebe;                  // sexo  do  bebê pode assumir os valores ‘0’(ignorado), ‘1’(masculino) e ‘2’(feminino)
    char estadoMae[2];              // sigla  do  estado  da cidade de residência da mãe
    char estadoBebe[2];             // sigla  do  estado  da cidade na qual o bebê nasceu
    
    // Campos  de  tamanho  variável: tamanho  máximo  de 105 bytes incluindo  os espaços reservados para os indicadores de tamanho
    char* cidadeMae;                // cidade de residência da mãe
    char* cidadeBebe;               // cidade na qual o bebê nasceu
}REGISTRO;

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

