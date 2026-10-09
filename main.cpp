/*
 * ALUNO: COLOQUE SEU NOME AQUI
 * File:   main.cpp
 */

#include <stdlib.h>
#include <time.h>

#include "btree.h"

int main(int argc, char** argv) {

    // iniciar a semente aleatoria
    srand ( time(NULL) );

    remove("arvoreb.dat"); // durante o desenvolvimento a cada execução o arquivo é apagado

    // criar arvore b
    btree *arvore = new btree();

    printf("Use essas chaves como exemplos de consultas que devem ser encontradas: ");

    // inserir numeros aleatorios na arvore
    for (int i = 0; i < 10000; i++) {
        int chave = rand() % 1000000 + 1;
		int valor = chave + 1; // representa a localização de um registro no arquivo de dados
        arvore->insereChave(chave,valor);
        if (i % 1000 == 0)
            printf("%d, ",chave);
    }

    printf("\n\nEstatisticas:\n");
    printf("Numero de elementos: %d\n", arvore->getNumeroElementos());
    printf("Altura da arvore: %d\n", arvore->getAlturaArvore());
    printf("Taxa de ocupacao: %d%%\n", arvore->computarTaxaOcupacao());

    int opcao = 0;
    while (opcao != 5) {
        printf("\n\nMenu: 1-inserir 2-remover 3-consultar 4-depurar 5-sair: ");
        scanf("%d",&opcao);
        switch(opcao) {
            int chave, valor;
            case 1:
                printf("\nChave: ");
                scanf("%d",&chave);
                printf("\nValor: ");
                scanf("%d",&valor);
                arvore->insereChave(chave,valor);
                break;
            case 2:
                printf("\nRemove chave: ");
                scanf("%d",&chave);
                arvore->removeChave(chave);
                break;
            case 3:
                printf("\nConsulta chave: ");
                scanf("%d",&chave);
                valor = arvore->buscaChave(chave);
                if (valor == -1)
                    printf("\nChave %d nao encontrada.\n",chave);
                else
                    printf("\nChave %d encontrada e valor=%d.\n",chave,valor);
                break;
            case 4:
                arvore->depuracao();
                break;
            case 5:
                // exit :-)
                break;
            default:
                opcao = 0;
                break;
        }
    }

    delete arvore;

    return (EXIT_SUCCESS);
}
