#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <numeric>



/* Implementacao pessoal de Union-Find para usar no Kruskal
 * uso ha um tempo essa implementacao para resolver problemas da maratona
 * e ela sempre se mostrou robusta
 */
struct unionFind 
{
    std::vector<int> parent, rank;

    unionFind(int n)
    {
        parent.resize(n);
        rank.assign(n, 0);
        std::iota( parent.begin(), parent.end() , 0); 
    }
    
    int findParent(int v)
    {
        if( parent[v] != v) parent[v] = findParent( parent[v] );
        return parent[v];
    }

    bool unite(int a, int b)
    {
        a = findParent(a), b = findParent(b);
        if(a == b) return false; // eles ja sao do mesmo cluster
        if( rank[a] < rank[b]) parent[a] = b;
        else parent[b] = a;
        rank[a] += ( rank[a] == rank[b]);
        return true;
    }
};


/* Codigo para dar uma ideia do approach brute-force / backtrack
 * Como esse programa só vai fazer sentido para grafos com <= 32 nós, vou usar
 * bitmask para alguns detalhes da implementacao 
 */

struct aresta 
{
    int origem, destino, custo;
    aresta(int _x = 0, int _y = 0, int _z = 0) : origem(_x), destino(_y), custo(_z) {}
};

struct grafo 
{
    int numeroDeNos;
    long long mascaraTerminal = 0;
    std::vector< aresta > arestas;

    grafo(int N) : numeroDeNos(N) {} 
    
    // Lembrar que os indices dos nós pertencem ao intervalo [0, numeroDeNos - 1]
    void definirNosTerminais( std::vector<int> indicesDosNosTerminais )
    {
        mascaraTerminal = 0LL; // Resetando a mascara de nos
        for(const int& indice : indicesDosNosTerminais)
        {
            if( indice > 31)
            {
                std::cout << "Essa instancia é grande demais para ser rodada com backtrack" << std::endl;
                exit(-1);
            }
            else
            {
                mascaraTerminal |= (1LL << indice );
                // Digamos que estamos adicionando o vertice de indice 0 do grafo na mascara
                // Ela vai ficar assim = 0x0000000000000000000....0000001
                // Vamos ter 0 para os vertices que nao pertencem ao conjunto de nos terminais
                // e 1 para os vertices que pertecem ao conjunto de nos terminais
            }
        }
    }
    
    void adicionaAresta( int orig, int dest, int c)
    {
        arestas.push_back( aresta( orig, dest, c) );
    }
    
    /* A funcao ordena arestas deve ser chamada antes de realizar o backtrack
     * Isso é necessário para que o kruskal realizado em cada step dê o resultado correto
     */
    void ordenaArestas()
    {
        std::sort( arestas.begin(), arestas.end(), [] (const aresta& a, const aresta& b) { 
                return a.custo < b.custo; } );
    }
    
    // Acha o custo da melhor solução que contem os nos terminais definidos a priori
    long long bruteforce()
    {
        ordenaArestas(); // esse procedimento so deve ser feito uma vez
        
        // Chamei forca bruta
        std::cout << "mascaraTerminais = " << mascaraTerminal << std::endl;
        
        long long mascaraMaxima = (1LL << numeroDeNos) - 1;
        
        long long melhorResposta = 0x3f3f3f3f3f3f3f3fLL; // Constante grande inicial
        // Mascara maxima equivale ao subconjunto que engloba todos os nós
        
        
        // Agora, para todo conjunto que contiver todos os nós terminais, vamos
        // tentar achar uma árvore geradora minima. Se conseguirmos, vamos melhorar a resposta
        // do problema
        for(long long conjunto = 0LL; conjunto <= mascaraMaxima; ++conjunto)
        {
            // Vamos testar se esse conjunto contem os vertices terminais
            // O que vamos fazer aqui é um and bitwise entre o conjunto e a mascara de nos terminais.
            //
            // Suponha por exemplo: conjunto = 0x00001011
            //               mascaraterminal = 0x00001010
            //
            //  conj & mask                  = 0x00001010
            //  Conclusão, se o and de um com o outro for igual a mascaraTerminal
            //  entao quer dizer que o conjunto contem todos os nós da máscaraTerminal 
            //  em outras palavras, é um conjunto válido e vamos tentar achar a Árvore geradora mínima pra ele.
            if( (conjunto & mascaraTerminal) == mascaraTerminal)
            {
                // entao temos um conjunto valido
                unionFind UF(numeroDeNos);
                int quantidadeDeArestasDaSolucao = 0;
                long long custoDaSolucao = 0LL;

                for(const aresta& x : arestas )
                {
                    // vamos checar se essa aresta une dois vertices que estao no nosso conjunto
                    bool contemOrigem = ( conjunto & (1LL << x.origem) );
                    bool contemDestino = ( conjunto & (1LL << x.destino ) );
                    
                    if( contemOrigem && contemDestino )
                    {
                        // Se consegui juntar os dois vertices, quer dizer que
                        // vale a pena colocar essa aresta na solucao
                        if( UF.unite( x.origem, x.destino )  == true)
                        {
                            quantidadeDeArestasDaSolucao += 1;
                            custoDaSolucao += x.custo;
                        }
                    
                    }
                }
                /* Se temos N vértices no conjunto, temos que nos certificar de que 
                 * nosso algoritmo selecionou exatamente N - 1 arestas. Pois pode 
                 * existir o caso que nao existe uma árvore contendo todos os vertices
                 * do conjunto e apenas eles.
                 */
                int numeroDeVerticesNoConjunto = __builtin_popcountll( conjunto );
                // esse popcnt basicamente conta o numero de bits iguais a 1 em um inteiro ou long long
            
                if( quantidadeDeArestasDaSolucao + 1 == numeroDeVerticesNoConjunto)
                {
                    // Entao temos uma solucao valida
                    melhorResposta = std::min( melhorResposta, custoDaSolucao );
                }
            }
        }
        return melhorResposta;
    }
};


int main()
{
    // Vou testar com um grafo pequeno pra garantir que esta funcionando
    // Vou testar um caso que sei a resposta
    // Basicamente vou botar todos os nos como nos terminais e a resposta
    // tem que ser igual ao custo da Arvore geradora minima
    grafo g(5);

    std::vector<int> terminais = {0, 1, 2, 3, 4};
    g.definirNosTerminais( terminais );

    g.adicionaAresta(0, 1, 10);
    g.adicionaAresta(0, 1, 20);
    g.adicionaAresta(1, 2, 10);
    g.adicionaAresta(2, 3, 10);
    g.adicionaAresta(3, 4, 10);
    
    // Claramente a árvore geradora minima tem que ter custo 40, e a arvore de Steiner encontrada deveria ter o mesmo valor

    long long resposta = g.bruteforce();

    std::cout << resposta << std::endl;

    return 0;
}
