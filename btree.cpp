#include <sys/types.h>
#include <sys/stat.h>

#include "btree.h"

#ifndef _BTREE_CPP
#define	_BTREE_CPP

bool fileExists(const char *filename) { struct stat statBuf; if (stat(filename,&statBuf) < 0) return false; return S_ISREG(statBuf.st_mode); }

btree::btree() {
    char nomearquivo[20] = "arvoreb.dat";

    // se arquivo ja existir, abrir e carregar cabecalho
    if (fileExists(nomearquivo)) {
        // abre arquivo
        arquivo = fopen(nomearquivo,"r+");
        leCabecalho();
    }
    // senao, criar novo arquivo e salvar o cabecalho
    else {
        // cria arquivo
        arquivo = fopen(nomearquivo,"w+");

        // atualiza cabecalho
        cabecalhoArvore.numeroElementos = 0;
        cabecalhoArvore.paginaRaiz = -1;
        salvaCabecalho();
    }
}

btree::~btree() {
    // fechar arquivo
    fclose(arquivo);
}

int btree::computarTaxaOcupacao() {
    return 0;
}

void btree::insereChave(int chave, int valor) {

    if (cabecalhoArvore.paginaRaiz == -1) {//arvore vazia
        int idNovaRaiz;
        paginaB *pgRaiz = novaPagina(&idNovaRaiz);

        pgRaiz->chaves[0] = chave;
        pgRaiz->valores[0] = valor;
        pgRaiz->numeroElementos = 1;
        salvaPagina(idNovaRaiz, pgRaiz);

        cabecalhoArvore.paginaRaiz = idNovaRaiz;
        cabecalhoArvore.numeroElementos = 1;
        cabecalhoArvore.alturaArvore = 1;
        salvaCabecalho();

        delete pgRaiz;
        return;
    }

    int chavePromovida, idPagNova;
    bool temSplit = insereRecursivo(chave, valor, cabecalhoArvore.paginaRaiz, 1, chavePromovida, idPagNova);

    if (temSplit){
        int idNovaRaiz;
        paginaB *novaRaiz = novaPagina(&idNovaRaiz);

        novaRaiz->chaves[0] = chavePromovida;
        novaRaiz->valores[0] = cabecalhoArvore.paginaRaiz; 
        novaRaiz->valores[1] = idPagNova;               
        novaRaiz->numeroElementos = 1;
        salvaPagina(idNovaRaiz, novaRaiz);

        cabecalhoArvore.paginaRaiz = idNovaRaiz;
        cabecalhoArvore.alturaArvore++;
        salvaCabecalho();

        delete novaRaiz;
    }

}

bool btree::insereRecursivo(int chave, int valor, int idPagina, int nivel, int &chavePromovida, int &idNovaPagina) {
    if (nivel == cabecalhoArvore.alturaArvore) {
        pagina *pgFolha = lePagina(idPagina);

        // Folha não está cheia: inserção simples
        if (pgFolha->numeroElementos < ORDEM - 1) {
            int i = pgFolha->numeroElementos - 1;
            while (i >= 0 && pgFolha->chaves[i] > chave) {
                pgFolha->chaves[i + 1] = pgFolha->chaves[i];
                pgFolha->valores[i + 1] = pgFolha->valores[i];
                i--;
            }
            pgFolha->chaves[i + 1] = chave;
            pgFolha->valores[i + 1] = valor;
            pgFolha->numeroElementos++;

            salvaPagina(idPagina, pgFolha);
            delete pgFolha;

            cabecalhoArvore.numeroElementos++;
            salvaCabecalho();
            return false;
        }

        // Folha cheia: dividir
        int tempChaves[ORDEM];
        int tempValores[ORDEM];

        for (int i = 0; i < ORDEM - 1; i++) {
            tempChaves[i] = pgFolha->chaves[i];
            tempValores[i] = pgFolha->valores[i];
        }

        int x = ORDEM - 2;
        while (x >= 0 && tempChaves[x] > chave) {
            tempChaves[x + 1] = tempChaves[x];
            tempValores[x + 1] = tempValores[x];
            x--;
        }
        tempChaves[x + 1] = chave;
        tempValores[x + 1] = valor;

        int meio = ORDEM / 2;

        // Atualiza a página esquerda (a própria folha)
        pgFolha->numeroElementos = meio;
        for (int i = 0; i < meio; i++) {
            pgFolha->chaves[i] = tempChaves[i];
            pgFolha->valores[i] = tempValores[i];
        }

        // Cria a página direita
        int idNovo;
        pagina *nova = novaPagina(&idNovo);
        nova->numeroElementos = ORDEM - meio;
        for (int i = meio; i < ORDEM; i++) {
            nova->chaves[i - meio] = tempChaves[i];
            nova->valores[i - meio] = tempValores[i];
        }

        salvaPagina(pgFolha->numeroPagina, pgFolha);
        salvaPagina(nova->numeroPagina, nova);

        delete pgFolha;
        delete nova;

        cabecalhoArvore.numeroElementos++;
        salvaCabecalho();

        chavePromovida = tempChaves[meio];
        idNovaPagina = idNovo;
        return true;
    }

    // Nó interno: desce pela árvore
    pagina *pgAtual = lePagina(idPagina);
    int i = 0;
    while (i < pgAtual->numeroElementos && chave >= pgAtual->chaves[i]) {
        i++;
    }
    int idFilho = pgAtual->valores[i];
    delete pgAtual;

    int chavePromovidaFilho, idNovoFilho;
    bool split = insereRecursivo(chave, valor, idFilho, nivel + 1,
                                 chavePromovidaFilho, idNovoFilho);

    if (!split) {
        return false;
    }

    // O filho dividiu: inserir a chave promovida e o novo filho no nó atual
    pagina *pg = lePagina(idPagina);

    // Se o nó interno tem espaço
    if (pg->numeroElementos < ORDEM - 1) {
        // Desloca chaves para a direita
        for (int j = pg->numeroElementos; j > i; j--) {
            pg->chaves[j] = pg->chaves[j - 1];
        }
        // Desloca filhos para a direita
        for (int j = pg->numeroElementos; j > i; j--) {
            pg->valores[j + 1] = pg->valores[j];
        }

        pg->chaves[i] = chavePromovidaFilho;
        pg->valores[i + 1] = idNovoFilho;
        pg->numeroElementos++;

        salvaPagina(idPagina, pg);
        delete pg;
        return false;
    }

    // Nó interno cheio: dividir
    int tempChaves[ORDEM];
    int tempValores[ORDEM + 1];

    // Copia chaves e insere a promovida
    for (int j = 0; j < i; j++) {
        tempChaves[j] = pg->chaves[j];
    }
    tempChaves[i] = chavePromovidaFilho;
    for (int j = i; j < pg->numeroElementos; j++) {
        tempChaves[j + 1] = pg->chaves[j];
    }

    // Copia filhos e insere o novo filho
    for (int j = 0; j <= i; j++) {
        tempValores[j] = pg->valores[j];
    }
    tempValores[i + 1] = idNovoFilho;
    for (int j = i + 1; j <= pg->numeroElementos; j++) {
        tempValores[j + 1] = pg->valores[j];
    }

    int meio = ORDEM / 2;

    // Página esquerda
    pg->numeroElementos = meio;
    for (int j = 0; j < meio; j++) {
        pg->chaves[j] = tempChaves[j];
    }
    for (int j = 0; j <= meio; j++) {
        pg->valores[j] = tempValores[j];
    }

    // Página direita
    int idNovo;
    pagina *nova = novaPagina(&idNovo);
    nova->numeroElementos = ORDEM - meio - 1;
    for (int j = meio + 1; j < ORDEM; j++) {
        nova->chaves[j - (meio + 1)] = tempChaves[j];
    }
    for (int j = meio + 1; j <= ORDEM; j++) {
        nova->valores[j - (meio + 1)] = tempValores[j];
    }

    salvaPagina(pg->numeroPagina, pg);
    salvaPagina(nova->numeroPagina, nova);

    delete pg;
    delete nova;

    chavePromovida = tempChaves[meio];
    idNovaPagina = idNovo;
    return true;
    // chamada recursiva incrementando nível até chegar na folha (quando nivel == cabecalho.alturaArvore)

    // na volta da recursão, situações:
    // 1) chave/valor inserido na pagina sem alterar maior valor da página
    // 2) overflow, divisão da pagina, para a página existente atualizar a maior chave e para
    //    a nova pagína, armazenar o identificador da página e a maior chave

    // cabecalho contem o numero da pagina raiz

    // se (cabecalhoArvore.paginaRaiz == 1) entao raiz eh a unica pagina. Ler raiz, inserir e salvar. Senao...
            // Exemplo:
            // pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
            // adicionar <chave,valor> na pagina pg
            // salvar pagina: salvaPagina(pg->numeroPagina, pg);

    // senao...

    // ler pagina raiz: pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);

    // se inserir, atualizar cabecalho
}

void btree::removeChave(int chave) {
	// neste trabalho, não é necessário implementar a remoção!

    // se remover, atualizar cabecalho
    if (true) {
        cabecalhoArvore.numeroElementos--;
        salvaCabecalho();
    }
}

int btree::buscaChave(int chave) {
    // se encontrar chave, retornar valor, caso contrário, retornar -1

    // arvore vazia
    if (cabecalhoArvore.paginaRaiz == -1) {
        return -1;
    }

    int idPagina = cabecalhoArvore.paginaRaiz;
    int nivel = 1;

    // desce pelas paginas internas ate chegar no nivel da folha
    while (nivel < cabecalhoArvore.alturaArvore) {
        paginaB *pg = lePagina(idPagina);

        int i = 0;
        while (i < pg->numeroElementos && chave >= pg->chaves[i]) {
            i++;
        }

        idPagina = pg->valores[i];   // proxima pagina
        delete pg;
        nivel++;
    }

    // agora idPagina eh uma folha: procura a chave dentro dela
    paginaB *folha = lePagina(idPagina);
    int resultado = -1;

    for (int i = 0; i < folha->numeroElementos; i++) {
        if (folha->chaves[i] == chave) {
            resultado = folha->valores[i];
            break;
        }
    }

    delete folha;
    return resultado;
}

#endif	/* _BTREE_CPP */
