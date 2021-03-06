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
    
    inline void ordenaArestas() {
        sort( arestas.begin(), arestas.end(), [] (const aresta& a, const aresta& b) { 
                return a.custo < b.custo; } );
    }
    
    long long bestGlobalAnswer = inf;
    // Removendo a maior aresta pra garantir que é uma relaxacao
    inline long long relaxation(const deque<bool> verticesAtivos ) {
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
    
    inline long long doit()
    {
        ordenaArestas();
        branch_and_bound(verticesTerminais);
        return bestGlobalAnswer;
    }
    
    inline void branch_and_bound( deque<bool>& verticesAtivos ) {
        long long V = relaxation( verticesAtivos );
        // Se entrar no if, eh porque temos esperancas de melhorar a resposta atual
        if( V < bestGlobalAnswer) {
            long long val = MST(verticesAtivos);
            if(val < bestGlobalAnswer) 
            {
                bestGlobalAnswer = val;
                for(int i = 0; i < numeroDeNos; ++i)
                {
                    if(verticesAtivos[i]) cerr << i << " ";
                }
                cerr << endl;
                cerr << "novaAns = " << val << endl;
            }
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
    inline long long MST(deque<bool>& verticesAtivos)
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
    ifstream in("B/b02.stp");
    int nodes, edges;
    in >> nodes >> edges;
    grafo g(nodes);
    cout << nodes << " " << edges << endl;
    for(int i = 0; i < edges; ++i)
    {
        int a, b; long long c;
        in >> a >> b >> c;
        --a;--b;
        g.adicionaAresta(a, b, c);
    }
    vector<int> indicesTerminais;
    int qnt;
    in >> qnt;
    for(int i = 0; i < qnt; ++i)
    {
        int indiceTerminal;
        in >> indiceTerminal;
        indicesTerminais.push_back(indiceTerminal - 1);
    }
    g.definirNosTerminais( indicesTerminais );
    cout << g.doit() << endl;

    return 0;
}
