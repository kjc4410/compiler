#include<iostream>
#include<vector>
#include<algorithm>
#include<stack>
using namespace std;
vector<int> CFG[11];
vector<vector<int>> SCC;
stack<int> s;
bool visited[11];
int ID[11], id;

/// SCC는 노드마다 번호를 부여하고 가장 작은 번호의 노드를 부모로 한다
/// 만약에 사이클이 발생한다면 서로 연결되어있다는 뜻이고 각 노드의 부모는 가장 작은 노드
/// 즉 사이클의 시작노드가 될 것이다.
/// @return 
int dfs(int node){
    ID[node] = ++id; //노드에 번호를 부여한다
    int parent = ID[node]; // 현재 노드의 부모는 일단 자기 자신이다.
    s.push(node); // 현재 노드를 스택에 삽입한다

    // 현재 노드와 연결된 그래프를 확인한다.
    for(int i=0; i < CFG[node].size();i++){
        int connected = CFG[node][i]; // 연결되 노드가 있고
        
        if(ID[connected] == 0) // 현재 연결된 노드가 아직 번호를 부여받지 않았으면
            // 이 노드의 부모를 찾아야 한다. scc의 정의에 따라 가장 작은 번호가 되어야 한다.
            // 만약 사이클이 생겨 사이클이 처음 시작되는 노드에 도착했다면 이 부분은 실행되지 않을 것이다
            parent = min(parent , dfs(connected)); 
        else if(visited[connected] == false) //번호는 부여받았지만 아직 방문처리가 되지 않았을때
            // 즉 사이클의 여부를 확인하고 parent가 모두 결정되었다면 
            //  가장 작은 노드 즉 사이클의 시작 노드가 부모노드가 될 것이다
            parent = min(parent, ID[connected]);
    }
    
    // 사이클의 마지막 노드는 당연히 부모노드가 가장 작은 노드일 것이고
    // 부모노드보다는 숫자가 당연히 높을 것이다. 따라서 사이클의 시작 노드를 제외하고는
    // 사이클의 속하는 나머지 노드들은 아래 if문에서 실행되지 않는다.
    // if문이 실행되면 현재 스택에는 사이클에 속하는 노드들이 삽입되어 있을 것이다.
    if(ID[node] == parent){
        vector<int> scc; //
        while(true){
            int t = s.top();
            s.pop();
            visited[t]=true; // 이 방문처리는 부모가 모두 결정되고, 발생된 사이클을 모두 알았다면 방문 처리
            scc.push_back(t);
            if(node == t) break;
        }
        SCC.push_back(scc);
    } 
    return parent;
}
