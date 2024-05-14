#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
vector<int> CF[1111];
vector<int> Dominator[15];
vector<int> DT[15];
vector<int> Frontier[15];
int node_size;
// 두 벡터의 교집합 계산
vector<int> find_disjoint(vector<int> &v1, vector<int> &v2) {
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
        if (old != Dominator[connected]){
            if(!old.empty()) //기존 Dominator[connected]에서 맨 뒤를 뺀 값이 교집합으로 구한 것과 같다면
                old.pop_back(); // 더 이상 수행할 필요 없음(사이클이 있다는 것)
            if(old == Dominator[connected]){
                Dominator[connected].push_back(connected);
                return; // 뺀 값을 다시 넣어주고 리턴
            }
            Dom(connected);
        }
    }
}

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

bool visited[1111];
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

//Iterated Dominace Frontier 계산
int IDFparent;
void IDF(int node){
    if(Frontier[node].size()==0)
        return;
    
    for(auto &i : Frontier[node]){

        int frontierNode = i;

        if(find(Frontier[IDFparent].begin(), Frontier[IDFparent].end(),frontierNode) == Frontier[IDFparent].end())
            Frontier[IDFparent].push_back(frontierNode);
        
        if(Frontier[frontierNode].size()==0) return;
        IDF(frontierNode);
    }
}

int main() {
    int entry = 1,cnt=1;
    //--1--
    //--2--
    //--3--
    //--4--
    //--5--
    //--6--

   CF[1].push_back(2);
   CF[1].push_back(3);
   CF[2].push_back(3);
   CF[3].push_back(4);
   CF[4].push_back(3);
   CF[4].push_back(5);
   CF[4].push_back(6);
   CF[5].push_back(7);
   CF[6].push_back(7);
   CF[7].push_back(4);
   CF[7].push_back(8);
   CF[8].push_back(9);
   CF[8].push_back(10);
   CF[8].push_back(3);
   CF[9].push_back(1);
   CF[10].push_back(7);
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

    for(int i=1;i<=node_size;i++){
        IDFparent=i;
        IDF(i);
    }
    
    cout<<endl<<endl<<"------Iterated Dominant Frontier---------\n";
    for(int i=1;i<=node_size;i++){
        cout<<"DF+("<<i<<"): ";
        for(int j=0;j<Frontier[i].size();j++){
            if(Frontier[i].empty())
                cout<<"x";
            else
                cout<<"Frontier[i][j]"<<" ";
        }
        cout<<endl;
    }
    return 0;
}
