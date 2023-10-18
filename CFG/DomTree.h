#ifndef __DOMTREE__
#define __DOMTREE__
#include "Dominator.h"
vector<int> DT[15];
bool visited[1111];

//Dominant Tree 계산
void DomTree() {
    int cnt = 0;
    for (auto& i : Dominator) {
        if (i.size() == 1) continue;
        for (size_t j = 0; j < i.size();j++) {
            if (j + 1 == i.size()) continue;
            int current = i[j], next = i[j + 1];

            //DT가 비어있지 않고, Dominator[i][j+1]가 DT의 맨 앞이나 뒤가 같다면 무시
            if (!DT[current].empty() && (DT[current].back() == next || DT[current].front() == next))
                continue;
            DT[current].push_back(next);
        }
    }
}

void DomTreeCheck(int node) {
    for (int i = 0;i < DT[node].size();i++) {
        int connected = DT[node][i];
        if (!visited[connected]) {
            visited[connected] = true;
            DomTreeCheck(connected);
        }
    }
    return;
}

#endif __DOMTREE__