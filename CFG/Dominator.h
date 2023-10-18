#ifndef __DOMINATOR_
#define __DOMINATOR_
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

vector<int> Dominator[15];
vector<int> CF[1111];
vector<int> Frontier[15];
int node_size;
// 두 벡터의 교집합 계산
vector<int> find_disjoint(vector<int> &v1, vector<int> &v2) {
    int last = v1.back();
    vector<int> result;
    set_intersection(
        v1.begin(), v1.end(),
        v2.begin(), v2.end(),
        back_inserter(result));
    return result;
}
//Dominant 계산
void Dom(int node) {
    Dominator[node].push_back(node); //자기 자신을 지배함
    for (int i = 0;i < CF[node].size();i++) { //CFG에서 연결된 노드 탐색
        int connected = CF[node][i]; // 연결된 노드

        vector<int> old = Dominator[connected]; // 벡터의 변화를 감지하기 위한 벡터
        //cout << "Dominator[" << connected << "]는" << connected << "와 " << node << "의 교집합이 되어야 함\n";
        if (!Dominator[connected].empty()) //비어있지 않으면, 즉 갱신이 필요한 벡터
            Dominator[connected] = find_disjoint(Dominator[connected], Dominator[node]);
        else
            Dominator[connected] = Dominator[node];

        //변화가 감지되었다면 다시 탐색
        if (old != Dominator[connected]) 
            Dom(connected);
    }
}


#endif __DOMINATOR_