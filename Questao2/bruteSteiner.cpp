#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <numeric>

using namespace std;

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
        iota( parent.begin(), parent.end() , 0); 
    }
    
    int findParent(int v)
    {
        if( parent[v] != v) parent[v] = findParent( parent[v] );
        return parent[v];
    }

    bool unite(int a, int b)
    {
        a = findParent(a), b = findParent(b);
        if(a == b) return false;
        if( rank[a] < rank[b]) parent[a] = b;
        else parent[b] = a;
        rank[a] += ( rank[a] == rank[b]);
        return true;
    }
};


// Esse codigo so funciona para N <= 32
// Como a complexidade é O( M * 2^(N) ), não faz muito sentido resolver com bruteforce instancias maiores que 32
struct aresta 
{
    int origem, destino, custo;
    aresta(int _x = 0, int _y = 0, int _z = 0) : origem(_x), destino(_y), custo(_z) {}
};

struct grafo 
{
    int numeroDeNos;
    long long mascaraTerminal = 0;
    vector< aresta > arestas;

    grafo(int N) : numeroDeNos(N) {} 
    
    void definirNosTerminais( std::vector<int> indicesDosNosTerminais )
    {
        mascaraTerminal = 0LL; // Resetando a mascara de nos
        for(const int& indice : indicesDosNosTerminais)
        {
            if( indice >= 31)
            {
                cout << "Essa instancia é grande demais para ser rodada com backtrack" << std::endl;
                exit(-1);
            }
            else mascaraTerminal |= (1LL << indice );
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
        sort( arestas.begin(), arestas.end(), [] (const aresta& a, const aresta& b) { 
                return a.custo < b.custo; } );
    }
    
    // Acha o custo da melhor solução que contem os nos terminais definidos a priori
    long long bruteforce()
    {
        ordenaArestas();
        
        cout << "mascaraTerminais = " << mascaraTerminal << std::endl;
        
        long long mascaraMaxima = (1LL << numeroDeNos) - 1; // 0x11111111, um 1 para cada no
        
        long long melhorResposta = 0x3f3f3f3f3f3f3f3fLL; // Constante grande inicial -> "infinito"
        
        
        // Agora, para todo conjunto que contiver todos os nós terminais, vamos
        // tentar achar uma árvore geradora minima. Se conseguirmos, vamos melhorar a resposta
        // do problema
        for(long long conjunto = 0LL; conjunto <= mascaraMaxima; ++conjunto)
        {
            // Para entrar no if, o subconjunto de nós sendo testado precisa necessariamente conter todos os nós terminais
            if( (conjunto & mascaraTerminal) == mascaraTerminal)
            {
                // entao temos um conjunto valido
                unionFind UF(numeroDeNos);
                int quantidadeDeArestasDaSolucao = 0;
                long long custoDaSolucao = 0LL;

                // Nesse loop, iteramos por todas as arestas e realizamos as operacoes do Kruskal
                for(const aresta& x : arestas )
                {
                    // vamos checar se essa aresta une dois vertices que estao no nosso conjunto
                    bool contemOrigem = ( conjunto & (1LL << x.origem) );
                    bool contemDestino = ( conjunto & (1LL << x.destino ) );
                    
                    if( contemOrigem && contemDestino )
                    {
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
                int numeroDeVerticesNoConjunto = __builtin_popcountll( conjunto ); // Essa instrucao conta os 1's de um long long
            
                if( quantidadeDeArestasDaSolucao + 1 == numeroDeVerticesNoConjunto) melhorResposta = min( melhorResposta, custoDaSolucao );
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
