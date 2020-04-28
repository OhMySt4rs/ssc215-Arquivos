// 11315054 Wellington Matos Amaral
// 11366913 Mateus de Souza Santos

#include <stdio.h>
#include <stdlib.h>
#include <binarionatela.h>
#include <registro.h>

int main(void){

    int opc;

    char arqDataset[10], arqBin[15];

    FILE *dataset, *bin;


    scanf("%d", &opc);

    // inicialmente o status vale `1` (arquivo consistente) e deve ser alterado para `0`
    // isso sinaliza que o arquivo foi finalizado adequadamente(arquivo inconsistente)

    // Deve abrir o arquivo em "w+b"

    switch (opc){

        // Leitura de registros obtidos a partir de um arquivo de entrada 
        // e a gravação desses registros em um arquivo de dados  de saída. 
        case 1:

            scanf("%s %s", arqDataset, arqBin);

            if((dataset = fopen(arqDataset, "r")) == NULL){
                printf("Erro na abertura do arquivo csv");
                return 0;
            }

            if((bin = fopen(arqBin, "w+b")) == NULL){
                printf("Erro na abertura do arquivo bin");
                return 0;
            }

            binarioNaTela(arqBin);

            break;

        // Recuperação dos dados, de todos os registros, armazenados no arquivo de dados
        case 2:

            scanf("%s", arqBin);

            if((bin = fopen(arqBin, "r+b")) == NULL){
                printf("Falha no processamento do arquivo.\n");
                return 0;
            }
            
            break;
    }

    // antes de finalizar a operacao, mudar o status para `1` 
}