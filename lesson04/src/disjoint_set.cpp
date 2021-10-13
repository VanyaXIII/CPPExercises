#include "disjoint_set.h"
#include <algorithm>

#define ROOT -1


DisjointSet::DisjointSet(long size) {
    parents = std::vector<int>(size);
    ranks = std::vector<int>(size, 1);
    sizes = std::vector<int>(size, 1);
    for (int i = 0; i < size; ++i)
    {
        parents[i] = i;
    }
}

int DisjointSet::get_set(int element) {
    if (element == parents[element])
        return element;
    return parents[element] = get_set(parents[element]);
}

int DisjointSet::count_differents() {
    int count = 0;
    for (size_t i = 0; i < this->parents.size(); i++) {
        if (get_set(i) == i)
            count++;
    }

    return count;
}

int DisjointSet::get_set_size(int element) {
    return sizes[get_set(element)];
}

int  DisjointSet::union_sets(int element0, int element1)
{
    int root0 = get_set(element0),
            root1 = get_set(element1),
            size0 = sizes[root0],
            size1 = sizes[root1],
            rank0 = ranks[root0],
            rank1 = ranks[root1];

    if(root1 == root0)
        return root0;

    if(rank0 == rank1){
        parents[root1] = root0;
        sizes[root0] += size1;
        ranks[root0]++;
        return root0;
    }else if(rank0 > rank1){
        parents[root1] = root0;
        sizes[root0] += size1;
        return root0;
    }else{
        parents[root0] = root1;
        sizes[root1] += size0;
        return root1;
    }
}
