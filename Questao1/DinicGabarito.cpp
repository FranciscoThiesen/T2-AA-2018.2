#include <bits/stdc++.h>
using namespace std;

#define all(x) (x).begin(), (x).end()

#define pb push_back
#define sz(x) ((int)(x).size())
#define TRACE(x) x
#define WATCH(x) TRACE(cout << #x" = " << x << endl)
#define PRINT(x...) TRACE(printf(x))
#define WATCHR(a, b) TRACE(for (auto c=a; c!=b;) cout << *(c++) << " "; cout << endl)
#define WATCHC(V) TRACE({cout << #V" = "; WATCHR(V.begin(), V.end());})

#define FU(i, a, b) for (auto i = a; i < b; ++i)
#define fu(i, b) FU(i, 0, b)
#define FD(i, a, b) for (auto i = (b) - 1; i >= a; --i)
#define fd(i, b) FD(i, 0, b)

using ll = long long;
using vi = vector<int>;
using vvi = vector<vi>;
using vll = vector<ll>;
using vvll = vector<vll>;
using vd = vector<double>;
using vb = vector<bool>;
using pii = pair<int, int>;
using pll = pair<ll, ll>;

ll mod(ll a, ll b) {
  return ((a%b)+b)%b;
}

int cmp(double x, double y = 0, double tol = 1.e-7) {
  return (x <= y + tol) ? (x + tol < y) ? -1 : 0 : 1;
}


template<typename CAP = double> struct graph {
  //////////////////////////////////////////////////////////////////////////////
  // Shared part. Also known as: You will need this!
  //
  vi dest;  // use sz(dest) as number of arcs
  vvi adj;  // use sz(adj) as number of vertices
  int inv(int a) { return a ^ 0x1; }
  graph(int n = 0) {
    adj.resize(n);
  }
  // Adds an arc to the graph. u is capacity, c is cost.
  // u is only needed on flows, and c only on min-cost-flow
  int arc(int i, int j, CAP u) {
    dest.pb(j);
    adj[i].pb(sz(dest)-1);
    dest.pb(i);
    adj[j].pb(sz(dest)-1);
    cap.pb(u); // For both flows
    cap.pb(0);
    return sz(dest)-2;
  }
  //////////////////////////////////////////////////////////////////////////////
  // For both flows!!
  //

  vector<CAP> cap, flow;

  int orig(int a) { return dest[inv(a)]; }
  CAP capres(int a) { return cap[a] - flow[a]; }

  //////////////////////////////////////////////////////////////////////////////
  // Max Flow! - Dinic O(n^2 * m)
  // don't call maxflow with ini == end
  //

  vi curAdj, d;

  bool MFbfs(int s, int t) {
    d.assign(sz(adj), INT_MAX/2);
    curAdj = vi(sz(adj));
    d[s] = 0;
    queue<int> Q; Q.push(s);
    while (!Q.empty()) {
      int u = Q.front(); Q.pop();
      for (auto i : adj[u]) {
        int v = dest[i];
        if (capres(i) > 0 && d[v] == INT_MAX/2) {
          d[v] = d[u] + 1; Q.push(v);
        }
      }
    }
    return d[t] != INT_MAX/2;
  }

  CAP MFdfs(int u, int t, CAP f) {
    if (u == t) return f;
    for(int &i = curAdj[u]; i < adj[u].size(); ++i) {
      int ar = adj[u][i], v = dest[ar];
      if (d[v] != d[u]+1 || capres(ar) == 0) continue;
      ll tmpF = MFdfs(v, t, min(f, capres(ar)));
      if (tmpF) {
        flow[ar] += tmpF;
        flow[inv(ar)] -= tmpF;
        return tmpF;
      }
    }
    return 0;
  }

  CAP maxflow(int ini, int end) {
    flow.assign(sz(dest), 0);
    while (MFbfs(ini, end))
      while (MFdfs(ini, end, LLONG_MAX/2));
    CAP F = 0;
    for (int a : adj[ini]) F += flow[a];
    return F;
  }

};

double doit(const string filename)
{
    ifstream in(filename);
    int nodes, edges, S, T;
    in >> nodes >> edges;
    in >> S >> T;
    --S; --T; // indexing from 0
    graph<double> G(nodes);
    cout << nodes << " " << edges << " " << S << " " << T << endl;
    for(int i = 0; i < edges; ++i)
    {
        int u, v;
        double c;
        in >> u >> v >> c;
        G.arc(u - 1, v - 1, c);
        G.arc(v - 1, u - 1, c);
    }
    in.close();
    return G.maxflow(S, T);
}

int main()
{
    string filename = "elist96.rmf";
    cout << doit(filename) << endl;
    return 0;
}
