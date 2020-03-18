#include <utility>
#include <unordered_map>

using namespace std;

struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const pair<T1, T2>& p) const
    { 
        auto hash1 = hash<T1>{}(p.first); 
        auto hash2 = hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
};

int main ()
{
    unordered_map<pair<string, unsigned int>, int, hash_pair> tmp;
    pair<string, unsigned int> tmp2;
    tmp2.first = "SDfsd";
    tmp2.second = 123;
    tmp[tmp2] = 666;
    return 1;
}