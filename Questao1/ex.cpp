#include <bits/stdc++.h>

using namespace std;

template <typename T>
T soma( T a, T b) {
    return a + b;
}

int main()
{
    cout << soma(10, 20) << endl;
    cout << soma(10.5, 20.0) << endl;
    string a = "maria ", b = "leticia";
    cout << soma(a, b) << endl;
    return 0;
}
