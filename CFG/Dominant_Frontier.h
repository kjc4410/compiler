#ifndef __DOMINANT_FRONTIER__
#define __DOMINANT_FRONTIER__
#include "DomTree.h"

vector<int> Frontier[15];

int parent; //
bool checking[1111]; // CFG의 visit를 계산
void dfs(int node) {
    checking[node] = true;
    for (int i = 0;i < CF[node].size();i++) {
        int connected = CF[node][i];
        //cout << " 연결된 노드는: " << connected << endl;
        if (checking[connected] == true)
            return;
        if (visited[connected] == false) {
            //중복되는 원소는 제거. 이렇게 하는 이
            if (!Frontier[parent].empty() and Frontier[parent].back() == connected)
                continue;
            else
                Frontier[parent].push_back(connected);
        }
        else 
            dfs(connected);
    }
}



#endif __DOMINANT_FRONTIER