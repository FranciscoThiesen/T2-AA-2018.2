#include <bits/stdc++.h>

using namespace std;

constexpr long long inf = 0x3f3f3f3f3f3f3f3fLL;

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


struct aresta 
{
    int origem, destino, custo;
    aresta(int _x = 0, int _y = 0, int _z = 0) : origem(_x), destino(_y), custo(_z) {}
};

struct grafo 
{
    int numeroDeNos;
    deque<bool> verticesTerminais; // Usando deque, porque vector tem um overhead maior com bool do que deque
    
    vector< aresta > arestas;
    vector< set< pair<long long, int> > >  adjList; 
    grafo(int N) : numeroDeNos(N) 
    {
        verticesTerminais.resize(N);
        for(int i = 0; i < N; ++i) verticesTerminais[i] = false;
        adjList.resize(N);
    } 
    
    void definirNosTerminais(vector<int> indicesDosNosTerminais )
    {
        for(int i = 0; i < numeroDeNos; ++i) verticesTerminais[i] = false;
        for(const int& indice : indicesDosNosTerminais) verticesTerminais[indice] = true;
    }
    
    void adicionaAresta( int orig, int dest, long long c) { 
        // checar se orig e dest estao 0-indexed
        arestas.push_back( aresta( orig, dest, c) ); 
        adjList[ orig ].emplace( c, dest);
        adjList[ dest ].emplace( c, orig);
    }
    
    void ordenaArestas() {
        sort( arestas.begin(), arestas.end(), [] (const aresta& a, const aresta& b) { 
                return a.custo < b.custo; } );
    }
    
    long long bestGlobalAnswer = inf;
    // Removendo a maior aresta pra garantir que é uma relaxacao
    long long relaxation(const deque<bool> verticesAtivos ) {
        long long COST = 0LL;
        long long largestEdge = 0LL;
        for(int v = 0; v < numeroDeNos; ++v) {
            if( ! verticesAtivos[v] ) continue;
            long long bestEdgeFromV = inf; 
            for(const auto& x : adjList[v] ) {
                if( verticesAtivos[x.second] ) {
                    bestEdgeFromV = x.first;
                    break;
                }
            }
            COST += bestEdgeFromV;
            largestEdge = max(largestEdge, bestEdgeFromV);
        }
        return COST - largestEdge;
    }
    
    long long doit()
    {
        ordenaArestas();
        branch_and_bound(verticesTerminais);
        return bestGlobalAnswer;
    }
    
    void branch_and_bound( deque<bool>& verticesAtivos ) {
        long long V = relaxation( verticesAtivos );
        // Se entrar no if, eh porque temos esperancas de melhorar a resposta atual
        if( V < bestGlobalAnswer) {
            bestGlobalAnswer  = min( bestGlobalAnswer, MST(verticesAtivos) );
        }
        for(int i = 0; i < numeroDeNos; ++i)
        {
            if( verticesAtivos[i] == false)
            {
                verticesAtivos[i] = true;
                branch_and_bound( verticesAtivos );
                verticesAtivos[i] = false;
            }
        }
    }
    
    // Acha a MST de um conjunto, se existir
    long long MST(deque<bool>& verticesAtivos)
    {
        unionFind UF(numeroDeNos);
        int quantidadeDeArestasDaSolucao = 0;
        long long custoDaSolucao = 0LL;
        long long melhorResposta = inf;
        // Nesse loop, iteramos por todas as arestas e realizamos as operacoes do Kruskal
        for(const aresta& x : arestas )
        {
                // vamos checar se essa aresta une dois vertices que estao no nosso conjunto
            bool contemOrigem = verticesAtivos[x.origem];
            bool contemDestino = verticesAtivos[x.destino];
            if( contemOrigem && contemDestino )
            {
                if( UF.unite( x.origem, x.destino )  == true)
                {
                    quantidadeDeArestasDaSolucao += 1;
                    custoDaSolucao += x.custo;
                }
            }
        }
        int numeroDeVerticesNoConjunto = 0;
        for(int i = 0; i < numeroDeNos; ++i) if( verticesAtivos[i]) numeroDeVerticesNoConjunto++; 
        if( quantidadeDeArestasDaSolucao + 1 == numeroDeVerticesNoConjunto) melhorResposta = min( melhorResposta, custoDaSolucao );
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

    cout << g.doit() << endl;

    return 0;
}
