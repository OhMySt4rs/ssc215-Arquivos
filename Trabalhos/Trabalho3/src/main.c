// 11315054 Wellington Matos Amaral
// 11366913 Mateus de Souza Santos

#include <stdio.h>
#include <stdlib.h>
#include <binarionatela.h>
#include <registro.h> 
#include <arvoreB.h>

int main(void){

    int opc, RRN, n, i, idNascimento, Pr;

    char arqDataset[10], arqBin[30], arqIndice[30], campo[30];

    FILE *dataset, *bin, *indice;

    scanf("%d", &opc);

    /* inicialmente o status vale `1` (arquivo consistente) e deve ser alterado para `0`
     * todas as vezes que existam alteracoes no arquivo, ao fim da operacao deve retornar a status 1
     * sinalizando que a operacao foi concluida e o programa finalizado adequadamente 
     */ 


    switch (opc){

        // Leitura de registros obtidos a partir de um arquivo de entrada 
        // e a gravação desses registros em um arquivo de dados  de saída. 
        case 1:

            // Leirura do nome do arquivo binario e csv
            scanf("%s %s", arqDataset, arqBin);

            // Abertura do arquivo csv
            if((dataset = fopen(arqDataset, "r")) == NULL){
                printf("Falha no processamento do arquivo.\n");
                return 0;
            }

            // Abertura do arquivo binario
            if((bin = fopen(arqBin, "w+b")) == NULL){
                printf("Falha no processamento do arquivo.\n");
                return 0;
            }

            criaBinario(dataset, bin);

            // Fechar o arquivo binario
            fclose(bin);

            // Imprimir a saida
            binarioNaTela(arqBin);

            // Fechar o arquivo csv
            fclose(dataset);

            break;

        // Recuperação dos dados, de todos os registros, armazenados no arquivo de dados
        case 2:

            // Leirura do nome do arquivo binario
            scanf("%s", arqBin);

            // Abertura do arquivo
            if((bin = fopen(arqBin, "r+b")) == NULL) printf("Falha no processamento do arquivo.\n");

            // Retorna erro caso o arquivo esteja vazio
            else if(lerBinario(bin) == ERRO){
                printf("Falha no processamento do arquivo.\n");  
            }

            // Fechar o arquivo
            fclose(bin);
            break;

        // Pesquisa por itens gravados no arquivo
        case 3:

            // Leirura do nome do arquivo binario
            scanf("%s", arqBin);

            // Erro caso tenha problemas na abertura do arquivo ou caso hajam problemas na alocacao do registro
            if((bin = fopen(arqBin, "r+b")) == NULL || (buscaCombinadaRegistro(bin) == ERRO)) 
                printf("Falha no processamento do arquivo.\n");

            // Fechar o arquivo
            fclose(bin);
            break;

        // Busca por RRN
        case 4:

            // Leirura do nome do arquivo binario
            scanf("%s %d",arqBin, &RRN);

            // Caso ocorram problemas na abertura do arquivo ou o RRN nao seja valido
            if((bin = fopen(arqBin, "r+b")) == NULL || buscaRRN(bin, RRN) != SUCESSO){
                printf("Falha no processamento do arquivo.");
            }

            // Fechar o arquivo
            fclose(bin);
            break; 

        // Remocao logica de registro
        case 5:

            // Leirura do nome do arquivo binario
            scanf("%s %d", arqBin, &n);

            // Caso ocorram problemas na abertura do arquivo 
            if((bin = fopen(arqBin, "r+b")) == NULL){
                printf("Falha no processamento do arquivo.");
            } else{
                // Ele pode remover n arquivos a cada vez
                for(i = 0; i < n; i++){
                    // Caso existam problemas de incosistencia, arquivo vazio ou registro nao encontrado
                    if(removerRegistroBin(bin)){
                        printf("Falha no processamento do arquivo.");
                        return 0;
                    }
                }

                // Fechar o aquivo
                fclose(bin);

                binarioNaTela(arqBin);
            }
            break; 
            
        // Iserir elementos adicionais     
        case 6:
            // Leirura do nome do arquivo binario
            scanf("%s %d", arqBin, &n);

            // Caso ocorram problemas na abertura do arquivo 
            if((bin = fopen(arqBin, "r+b")) == NULL){
                printf("Falha no processamento do arquivo.");
            } else{
                // Ele pode inserir n arquivos a cada vez
                for(i = 0; i < n; i++){
                    // Caso exista inconsistencia no arquivo
                    if(inserirRegistro(bin, &idNascimento, &Pr)){
                        printf("Falha no processamento do arquivo.");
                        return 0;
                    }
                }

                // Fechar o arquivo
                fclose(bin);

                binarioNaTela(arqBin);
            }
            break;

        // Atualizar registro
        case 7:
            // Leirura do nome do arquivo binario
            scanf("%s %d", arqBin, &n);

            // Caso ocorram problemas na abertura do arquivo
            if((bin = fopen(arqBin, "r+b")) == NULL){
                printf("Falha no processamento do arquivo.");
            } else{
                // Ele pode atualizar n arquivos a cada vez
                for(i = 0; i < n; i++){
                    scanf("%d", &RRN);
                    // Caso existam incosistencias no arquivo ou o RRN seja invalido
                    if(atualizarRegistroBin(bin, RRN)){
                        printf("Falha no processamento do arquivo.");
                        return 0;
                    }
                }

                // Fechar o arquivo
                fclose(bin);

                binarioNaTela(arqBin);
            }
            break;
        
        case 8:
            // Leitura e abertura dos arquivos de dados e de indice
            scanf(" %s %s", arqBin, arqIndice);

            if((bin = fopen(arqBin, "r + b")) == NULL || (indice = fopen(arqIndice, "w + b")) == NULL
            || criarIndiceArvoreB(bin, indice) == DEURUIM){
                printf("Falha no processamento do arquivo.\n");
            }
            else{
                // Fechar os arquivos
                fclose(bin);
                fclose(indice);
                
                binarioNaTela(arqIndice);
            }
                        
            break;

        case 9:
            // Leitura e abertura dos arquivos de dados e de indice
            scanf(" %s %s", arqBin, arqIndice);
            scanf("%s %d", campo, &idNascimento);
            if((bin = fopen(arqBin, "rb")) == NULL || (indice = fopen(arqIndice, "rb")) == NULL || buscaArvoreB(bin, indice, idNascimento) == DEURUIM){
                printf("Falha no processamento do arquivo.\n");
            }else{
                // Fechar os arquivos
                fclose(bin);
                fclose(indice);           
            }
            break;

        case 10:
            // Leirura do nome do arquivo binario
            scanf("%s %s %d", arqBin, arqIndice, &n);

            // Caso ocorram problemas na abertura do arquivo 
            if((bin = fopen(arqBin, "r + b")) == NULL || (indice = fopen(arqIndice, "r + b")) == NULL){
                printf("Falha no processamento do arquivo.\n");
            } else{
                // Ele pode inserir n arquivos a cada vez
                for(i = 0; i < n; i++){
                    // Caso exista inconsistencia no arquivo
                    if(inserirRegistroArvoreB(bin, indice)){
                        printf("Falha no processamento do arquivo.");
                        return 0;
                    }
                }

                // Fechar o arquivo
                fclose(bin);
                fclose(indice);

                binarioNaTela(arqIndice);
            }
            break;
    }
    return 0;
} 