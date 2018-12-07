#include <bits/stdc++.h>

/* Implementacao para o T2 de AA-2018.2
 * Francisco & Lauro
 */

#define sz(x) (int) (x).size()

using namespace std;

constexpr double eps = 1e-8;

template < typename T = double > 
inline int cmp_double(const T a, const T b, double TOL = eps )
{
    if( a + TOL > b) {
        if(b + TOL > a) return 0;
        return 1;
    }
    return -1;
}

// Nos serao numerados de 0, N - 1. Se por acaso nao estiver convergindo,
// checar se temos que adicionar tolerancia por conta capacidade = double
template< typename CAP = double>
struct Graph {
    
    int nodes, edges, S, T;
    vector< int > dist, dest;
    vector< vector< int> > outgoing_edges;
    vector< CAP > capacities, flow, excess; 
    
    CAP residual_cap(int idx) { return capacities[idx] - flow[idx]; }
    inline int inv( int a ) { return a ^ 1; }
    
    Graph(int n, int source, int sink) : nodes(n), S(source), T(sink) 
    {
        excess.assign(nodes, 0);
        dist.assign(nodes, 0);
        outgoing_edges.resize(nodes);
    }
    
    void addArc(int u, int v, CAP c)
    {
        outgoing_edges[ u ].push_back( sz(dest));
        outgoing_edges[ v ].push_back( sz(dest) + 1 );
        
        dest.push_back( v );
        dest.push_back( u );
        
        capacities.push_back( c );
        capacities.push_back( 0 );
    
        flow.push_back( 0 );
        flow.push_back( 0 );
    }
    
    void find_distances(int root)
    {
        queue<int> q;
        q.push(root);
        while(!q.empty())
        {
            int fst = q.front();
            q.pop();
            for(const int idx : outgoing_edges[fst] )
            {
                int adj = dest[ idx ];
                if( dist[adj] == -1 )
                {
                    dist[adj] = dist[fst] + 1;
                    q.push( adj ); 
                }
            }
        }
    }

    void preprocess() 
    {
        fill( dist.begin(), dist.end(), 0);
        fill( excess.begin(), excess.end(), 0);
        fill( flow.begin(), flow.end(), 0);
        dist[T] = 0;
        find_distances( T );
        dist[S] = nodes;
    }
   
    // Pegar qualquer no ativo. O(n^2 * m)
    CAP generic_preflow_push()
    {
        preprocess(); 
        
        stack<int> active_nodes;
        // Saturando arestas que saem de S
        for(const int idx : outgoing_edges[S])
        {
            int adj = dest[idx]; 
            CAP c = capacities[idx];          
            flow[idx] += c;
            flow[inv(idx)] -= c;
            excess[ adj ] += c;
            if(adj != T) active_nodes.push( adj );
        }
    
        auto pushRelabel = [&] (int node)
        {
            bool found_admissible_arc = false;
            for(const int idx : outgoing_edges[node])
            {
                int adj = dest[idx];
                if( dist[node] != dist[adj] + 1 ) continue;
                CAP send = min( excess[node], residual_cap(idx) );
                if( cmp_double( send, 0.0 ) == 1 )
                {
                    flow[idx] += send;
                    flow[inv(idx)] -= send;
                    excess[ node ] -= send;
                    excess[ adj ] += send;
                    if( cmp_double<double>( excess[adj], send ) == 0 && (adj != S && adj != T) ) active_nodes.push( adj );
                    if( cmp_double<double>( excess[node], 0 ) == 1) active_nodes.push( node ); 
                    found_admissible_arc = true;
                    break;
                }
            }
            
            if( found_admissible_arc == false ) 
            {
                dist[node] += 1;
                if( cmp_double<double>( excess[node], 0) == 1 ) active_nodes.push(node);
            }
        };
        
        while( !active_nodes.empty() )
        {
            int fst = active_nodes.top();
            active_nodes.pop();
            pushRelabel(fst);     
        }
        return excess[T];
    }

};

Graph<double> parseGraph(const string filename)
{
    ifstream in(filename);
    int nodes, edges, S, T;
    in >> nodes >> edges;
    in >> S >> T;
    --S; --T; // indexing from 0
    Graph<double> G(nodes, S, T);
    for(int i = 0; i < edges; ++i)
    {
        int u, v;
        double c;
        in >> u >> v >> c;
        G.addArc(u - 1, v - 1, c);
        G.addArc(v - 1, u - 1, c);
    }
    in.close();
    return G;
}

int main()
{
    string filename = "elist1440.rmf";
    auto testGraph = parseGraph( filename );
    auto x = testGraph.generic_preflow_push();
    cout << " flow = " << x << endl;

    return 0;
}
