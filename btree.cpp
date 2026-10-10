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
        if(pgFolha->numeroElementos < ORDEM - 1){
            
         int i= pgFolha->numeroElementos - 1;

         for(;i<=0 && pgFolha-> chaves[i]> chave;i++){

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
    return -1;
}

#endif	/* _BTREE_CPP */
