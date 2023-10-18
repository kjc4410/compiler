#include "Dominator.h"

int main() {
    int entry = 1,cnt=1;

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


//----------- Dominator 트리 계산---------//
    Dom(entry);  // 시작 노드를 1로 가정


//-------- Dominator 결과 출력-------------//
    cout << "------Dominant------\n";
    for (auto &i : Dominator) {
        if (i.size() == 0)continue;
        std::cout <<"Dominator of node [" << cnt++ << "]: ";
        for (int j : i)
            std::cout << j << " ";
        std::cout << endl;
    }


    //---------Dominant Tree 계산----------//
    DomTree();
    node_size = cnt-1;
    cout << endl<<endl;


    //---------Dominant Tree 출력----------//
    cout << "------Dominant Tree------\n";
    cnt = 0;
    for (auto& i : DT) {
        if (i.size() == 0) { ++cnt;continue; }
        std::cout << "DT [" << cnt++ << "]: ";
        for (auto& j : i)
            cout << j << " ";
        cout << endl;
    }
    int DT_size = cnt;
    cout << endl<<endl;


//--------Dominant Frontier 출력---------//
//DT에서 각 노드를 방문을 통해 확인
    cout << "---Dominant Tree에서 방문 확인---\n";
    for (int i = 1;i <= node_size;i++){
        DomTreeCheck(i);
        cout << i << "번 노드는: ";
        for (int j = 1;j <= node_size;j++)
            cout << visited[j] << " ";

        parent = i;
        dfs(i);
        if (Frontier[i].empty())
            Frontier[i].push_back(-1);//공집한은 -1로 표현
        memset(visited,false, node_size+1);
        memset(checking, false, node_size + 1);
        cout << "을 방문\n";
    }

    cout << endl << endl << "-----Dominant Frontier-----\n";
    for (int i = 1;i <= node_size;i++) {
        cout << "DF(" << i << "): ";
        for (int j = 0;j < Frontier[i].size();j++) {
            if (Frontier[i][j] == -1)
                cout << "x"; // 공집합
            else
                cout << Frontier[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}
