#include <iostream>
#include <vector>
#include<algorithm>
using namespace std;
vector<int> CF[1111];
vector<int> Dominator[15];

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

void Dom(int node) {
    Dominator[node].push_back(node);
    for (int i = 0;i < CF[node].size();i++) {
        int connected = CF[node][i];

        vector<int> old = Dominator[connected];
        //cout << "Dominator[" << connected << "]는" << connected << "와 " << node << "의 교집합이 되어야 함\n";
        if (!Dominator[connected].empty()) 
            Dominator[connected] = find_disjoint(Dominator[connected], Dominator[node]);
        else
            Dominator[connected] = Dominator[node];

        if (old != Dominator[connected]) 
            Dom(connected);
    }
}
int main() {
    int entry = 1;
    
    CF[1].push_back(2);
    CF[1].push_back(14);
    CF[2].push_back(3);
    CF[2].push_back(4);
    CF[3].push_back(5);
    CF[3].push_back(6);
    CF[3].push_back(7);
    CF[4].push_back(8);
    CF[4].push_back(9);
    CF[4].push_back(10);
    CF[5].push_back(11);
    CF[6].push_back(11);
    CF[7].push_back(11);
    CF[8].push_back(12);
    CF[9].push_back(12);
    CF[10].push_back(12);
    CF[11].push_back(13);
    CF[12].push_back(13);
    CF[14].push_back(13);
    
    /*
    CF[1].push_back(2);
    CF[2].push_back(3);
    CF[2].push_back(4);
    CF[3].push_back(5);
    CF[4].push_back(5);
    CF[5].push_back(6);
    CF[5].push_back(7);
    CF[6].push_back(2);
    */

    /*
    CF[1].push_back(2);
    CF[2].push_back(3);
    CF[3].push_back(4);
    CF[4].push_back(5);
    */

    
    //CF[1].push_back(2);
    //CF[1].push_back(3);
    //CF[2].push_back(7);
    //CF[3].push_back(4);
    //CF[3].push_back(5);
    //CF[4].push_back(6);
    //CF[5].push_back(6);
    //CF[6].push_back(7);
    

    /*
    CF[1].push_back(2);
    CF[2].push_back(3);
    CF[2].push_back(4);
    CF[2].push_back(6);
    CF[3].push_back(5);
    CF[4].push_back(5);
    CF[5].push_back(2);
    */
    // Dominator 트리 계산
    Dom(entry);  // 시작 노드를 1로 가정

    // Dominator 결과 출력
    for (int i = 1; i <= 14; i++) {
        std::cout << "Dominator of node [" << i << "]: ";
        for (int j : Dominator[i])
            std::cout << j << " ";
        std::cout << endl;
    }
    return 0;
}
