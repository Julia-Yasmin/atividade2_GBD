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

    insereRecursivo(chave, valor, cabecalhoArvore.paginaRaiz, 1);
}

void btree::insereRecursivo(int chave, int valor, int pagina, int nivel) {

    // verificar se raiz existe!
    if (cabecalhoArvore.paginaRaiz == -1) {
        int idNovaRaiz;
        paginaB *pgRaiz = novaPagina(&idNovaRaiz);

        pgRaiz->chaves[0] =chave;
        pgRaiz->valores[0] =valor;
        pgRaiz->numeroElementos=1;
        
        salvaPagina(idNovaRaiz, pgRaiz);

        cabecalhoArvore.paginaRaiz = idNovaRaiz;
        cabecalhoArvore.numeroElementos =1;
        cabecalhoArvore.alturaArvore =1;
        salvaCabecalho();
        delete pgRaiz;

        return;
    }

    if(nivel< cabecalhoArvore.alturaArvore){

     paginaB *pgAtual = lePagina(pagina);
     int i;

     for(; i< pgAtual->numeroElementos && chave>= pgAtual->chaves[i]; i++){
     }
        
     int idProximaPagina = pgAtual->valores[i];

     delete pgAtual; 
     insereRecursivo(chave, valor, idProximaPagina, nivel +1);
     
     return;
    }

    if( nivel == cabecalhoArvore.alturaArvore){
        paginaB *pgFolha = lePagina(pagina);
        if(pgFolha->numeroElementos < ORDEM - 1){//se a pagina nao ta cheia
            
         int i= pgFolha->numeroElementos - 1;

         for(;i >= 0 && pgFolha-> chaves[i]> chave;i--){//vai percorrer do final ate o inicio

            pgFolha->chaves[i + 1] = pgFolha->chaves[i];
            pgFolha->valores[i + 1] = pgFolha->valores[i];
         }

         pgFolha->chaves[i + 1] = chave;
         pgFolha->valores[i + 1] = valor;
         pgFolha->numeroElementos++;

         cabecalhoArvore.numeroElementos++;
         salvaCabecalho();

         salvaPagina(pagina, pgFolha);
         delete pgFolha;
         return;


        } else { //pagina cheia
            int inserirChave[ORDEM], inserirValor[ORDEM]; //temporarios apenas para a insercao
            for (int i = 0; i < ORDEM - 1; i++) {
                inserirChave[i] = pgFolha->chaves[i];
                inserirValor[i] = pgFolha->valores[i];
            }

            int x = ORDEM - 2;
            while (x >= 0 && inserirChave[x] > chave) {
                inserirChave[x+1] = inserirChave[x];
                inserirValor[x+1] = inserirValor[x];
                x--;
            }

            inserirChave[x + 1] = chave;
            inserirValor[x + 1] = valor;
            //esses vetores tem que ser divididos entre as duas paginas e ja estao ordenados e com a insercao feita

            int meio = ORDEM / 2;
            pgFolha->numeroElementos = meio;
            for (int i = 0; i < ORDEM / 2; i++) {
                pgFolha->chaves[i] = inserirChave[i];
                pgFolha->valores[i] = inserirValor[i];
            }

            int idNovo;
            paginaB *nova = novaPagina(&idNovo);
            
            for (int i = meio; i < ORDEM; i++) {
                nova->chaves[i - meio] = inserirChave[i];
                nova->valores[i - meio] = inserirValor[i];
                nova->numeroElementos++;
            }

            cabecalhoArvore.numeroElementos++;
            salvaCabecalho();

            salvaPagina(pgFolha->numeroPagina, pgFolha);
            salvaPagina(nova->numeroPagina, nova);   
            delete(pgFolha);
            delete(nova);                    
        }
    }

    

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
    cabecalhoArvore.numeroElementos++;
    salvaCabecalho();
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
