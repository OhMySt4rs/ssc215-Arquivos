// 11315054 Wellington Matos Amaral
// 11366913 Mateus de Souza Santos

#include <stdio.h>
#include <stdlib.h>
#include <binarionatela.h>
#include <registro.h> 

int main(void){

    int opc, RRN, n, i;

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

            criaBinario(dataset, bin);

            fclose(bin);

            binarioNaTela(arqBin);

            fclose(dataset);

            break;

        // Recuperação dos dados, de todos os registros, armazenados no arquivo de dados
        case 2:

            scanf("%s", arqBin);

            if((bin = fopen(arqBin, "r+b")) == NULL) printf("Falha no processamento do arquivo.\n");

            else if(lerBinario(bin) == ERRO){
                printf("Falha no processamento do arquivo.\n");  
            }

            fclose(bin);
            break;

        // Pesquisa por itens gravados no arquivo
        case 3:

            scanf("%s", arqBin);

            if((bin = fopen(arqBin, "r+b")) == NULL || (buscaCombinadaRegistro(bin) == ERRO)) 
                printf("Falha no processamento do arquivo.\n");

            break;

        // Busca por RRN
        case 4:
            scanf("%s %d",arqBin, &RRN);

            if((bin = fopen(arqBin, "r+b")) == NULL || buscaRRN(bin, RRN) != SUCESSO){
                printf("Falha no processamento do arquivo.");
            }

            fclose(bin);

            break; 

        // Remocao logica de registro
        case 5:

            scanf("%s %d", arqBin, &n);

            if((bin = fopen(arqBin, "r+b")) == NULL){
                printf("Falha no processamento do arquivo.");
            } else{
                // Ele pode remover n arquivos a cada vez
                for(i = 0; i < n; i++){
                    removerRegistroBin(bin);
                }

                fclose(bin);

                binarioNaTela(arqBin);
            }
            
        // Iserir elementos adicionais     
        case 6:
            /* Pode ser inserido em locais onde ja foram removidos registros
             * nao deve ser tratado truncamento
             * 
             * ela pode ser executada n vezes
             * 
             * valores nulos na entrada, devem ser identificados como NULL
             * 
             * nao esquecer de alterar o status do arquivo durante a manipulacao
             * ao final, deve se usar a funcao binario na tela
             * 
             * caso retorne erro imprimir "Falha no processamento do arquivo"
             */ 
            
            break;
        

        // Atualizar registro
        case 7:
            /* O usuario pode atualizar as informacoes de um registro, ele ira fornecer o RRN do
             * item, nao deve tratar os caracteres antigos que estejam no ristro anteriormente
             * 
             * o usuario pode fazer n atualizacoes
             * 
             * caso o RRN nao exista, o programa deve continuar normalmente ate concluir as n 
             * modificacoes
             * 
             * ao final, deve usar a funcao binario na tela
             * 
             * nao esquecer de mudar o status do arquivo durante a manipulacao*/

            break;

    }

    return 0;
} 