// 11315054 Wellington Matos Amaral
//          Mateus Gostoso
#include <stdio.h>

int main(void){
    int opc;
    scanf("%d", &opc);

    // inicialmente o status vale `1` (arquivo consistente) e deve ser alterado para `0`
    // isso sinaliza que o arquivo foi finalizado adequadamente(arquivo inconsistente)

    // Deve abrir o arquivo em "w+b"

    switch (opc){

        // Leitura de registros obtidos a partir de um arquivo de entrada 
        // e a gravação desses registros em um arquivo de dados  de saída. 
        case 1:
            // arquivo .csv e arquivo .bin para gerar saida
            // arquivoEntrada.csv  arquivoGerado.bin
            if(sucesso) binarioNaTela;
            else printf("Falha no carregamento do arquivo.\n");

            break;

        // Recuperação dos dados, de todos os registros, armazenados no arquivo de dados
        case 2:
            if(sucesso) imprimir;
            else printf("Falha no processamento do arquivo.\n");
            break;
    }






    // antes de finalizar a operacao, mudar o status para `1` 
}