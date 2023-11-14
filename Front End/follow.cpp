#include<iostream>
#include<string>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<set>
#include<vector>
#include<unordered_map>
using namespace std;

/*
    Grammar
    1. Nonterminal은 대문자로 작성
    2. Terminal은 소문자로 작성
    3. 유도는 ->로 나타냄
    4. 문법의 lhs가 같으면 | 기호를 통해 문법 규칙을 나열할 수 있음
*/

vector<string> Grammar;
enum Tokens {
	Nonterminal=0,
	Terminal,
	Operator,
};
bool isOperator(char ch);
void FIRST(string s);
void FOLLOW(string s);
void Processing(string s);

string arrow = "->";
using Head = string;
using Body = string;
using BodyToken = vector<pair<string, Tokens>>;

unordered_multimap<Head, Body> Rule;
vector<pair<string, BodyToken>> OnlyBody;

// lhs에서 body를 토큰화하기 위해 사용
// head는 생성규칙의 lhs 
unordered_multimap<Head, BodyToken> Rules;

set<string> NontermTable;
set<string> TermTable;

unordered_map<string, vector<string>> FIRST_SET;
unordered_map<string, vector<string>> FOLLOW_SET; 
//unordered_map<string, string> First1;
//vector< pair<string, vector<string>> > First2;

//FIRST를 구하는 함수
void FIRST(string nonterminal) {
	auto range = Rules.equal_range(nonterminal);
	vector<string> v; //FIRST의 원소를 저장하는 벡터

	//unordered_multimap에서 중복되는 값들을 찾는 과정
	for (auto it = range.first; it != range.second; it++) {

		// vector<pair<string, Tokens>>에 접근
		BodyToken vec = it->second;

		// 생성규칙의 첫 번째가 terminal이면 이를 추가
		if (vec[0].second == Terminal)
			v.push_back(vec[0].first);

		else{ // nonterminal이면 해당 nonterminal의 first를 구함
			FIRST(vec[0].first); //재귀를 통해 first를 구하고
			if(!FIRST_SET[vec[0].first].empty()) 
				for(auto &i : FIRST_SET[vec[0].first]) 
					v.push_back(i);
		}
	}
	FIRST_SET.insert({nonterminal,v});
}

unordered_map<string,bool> visited;

void visited_init(){
	for(auto &i: NontermTable)
		visited[i]=false;
}

void FOLLOW(string nonterminal){
	vector<string> vv;
	for(auto& i : OnlyBody){
		BodyToken rule_body = i.second;
		string rule_name = i.first;
		for(int j=0;j<rule_body.size();j++){
			
			if(rule_body[j].first == nonterminal){ //규칙의 rhs의 토큰이라면
				string cur_token = rule_body[j].first;
				string next_token = rule_body[j+1].first;
				//찾은 rhs의 다음 토큰이 없으면 현재 FOLLOW에 lhs의 terminal을 넣음
				if( j+1 == rule_body.size()){
					
					if(rule_name == cur_token)
						continue;

					//FOLLOW(rule_name);

					if(!FOLLOW_SET[rule_name].empty())
						for(auto &k : FOLLOW_SET[rule_name])
							vv.push_back(k);
					continue;
				}
				if(rule_body[j+1].second ==Terminal){ // 그 다음 토큰이 terminal이면
					
					//if(find(FOLLOW_SET[next_token].begin(), FOLLOW_SET[next_token].end(),   )
					
					vv.push_back(next_token); //벡터에 삽입
				}
				else if(rule_body[j+1].second == Nonterminal){ // 그 다음 토큰이 nonterminal이라면
					//if(FIRST_SET[rule_body[j+1].first] == FOLLOW_SET[nonterminal])


					// 1. 만약 규칙들 중 같은 Nonterminal이 반복적으로 있으면 굳이 일일이 하나씩 검증할 필요 없이 방문을 통해 중복 부분 탐색 x
					// 2. 아니면 중복되는 것을 모두 실행하여 벡터에 넣고 벡터의 중복 원소 제거 방법으로 다 없애기 
					if(visited[next_token] ==true)
						continue;


					for(auto &k : FIRST_SET[next_token]){ // 그 nonterminal의 first를 구함
						if(k != "epsilon")
							vv.push_back(k);
					}
					visited[next_token]=true;
					
				}
			}
		}
	}

	//아래는 벡터 중복 제거 원소 
	//sort(vv.begin(), vv.end());
	//vv.erase(unique(vv.begin(), vv.end()), vv.end());
	//FOLLOW_SET.insert(make_pair(s, vv)); <-----이거는 벡터에 적용이 안됨
	FOLLOW_SET[nonterminal]=vv;
	return;
}


void Processing() {
	for (auto& line : Grammar) {
		int index = line.find(arrow);

		string LHS;
		for (int i = 0;i < index;i++)
			if (line[i] != ' ')
				LHS += line[i];
		NontermTable.insert(LHS);
		//NontermTable.push_back(LHS);
	
		string RHS = line.substr(index + 2, line.size() - index + 2);

		istringstream rhs(RHS);
		string p;
		while (getline(rhs, p, '|')) {
			Rule.insert({ LHS,p });
			int i = 0;
			BodyToken body;
			while (i < p.size()) {
				string token;
				Tokens kind;
				if (p[i] == ' ') {
					i++;
					continue;
				}
				else if (isupper(p[i])) {
					while (isupper(p[i]))
						token += p[i++];
					kind = Nonterminal;
					NontermTable.insert(token);
				}
				else if (islower(p[i])) {
					while (islower(p[i]))
						token += p[i++];
					kind = Terminal;
					TermTable.insert(token);
				}
				else {
					while (isOperator(p[i]))
						token += p[i++];
					kind = Terminal;
					TermTable.insert(token);
				}
				body.push_back(make_pair(token, kind));
			}
			Rules.insert({LHS, body});
			OnlyBody.push_back({LHS ,body});
		}
	}
	std::cout << "--Rule-- \n";
	for (auto& i : Rule)
		std::cout << i.first << ": " << i.second << endl;
	std::cout << endl;



	std::cout << "Terminal 종류: \n";
	for (auto& i : TermTable)
		std::cout <<i<<" " << endl;
	std::cout << endl;
	
	std::cout << "Nonterminal 종류: \n";
	for (auto& i : NontermTable)
		std::cout << i << " " << endl;
	std::cout << endl;
}

int main() {
	string GrammarFile = "grammar.txt";
	ifstream file(GrammarFile);

	if (file.is_open()) {
		string line;
		while (getline(file, line))
			Grammar.push_back(line);
		file.close();
	}
	else
		std::cout << "error";

	Processing();

	cout<<"\n------FIST------\n";
	for (auto& i : NontermTable) {
		std::cout << "FIRST(" << i << ")= { ";
		FIRST(i);
		for(auto &j : FIRST_SET[i])
			cout<<j<<" ";	
		std::cout << "}\n";
	}
	
	cout<<"\n-----FOLLOW-----\n";
    for (auto& i : NontermTable){
        std::cout << "FOLLOW(" << i <<")= { ";
        FOLLOW(i);
		for(auto &j : FOLLOW_SET[i])
			cout << j << " ";
        std::cout<<"}\n";
    }
	
	return 0;
}

bool isOperator(char ch) {
	switch(ch){
	case '+': case '*': case '-': case '/':
	case '%': case'(': case ')': case'#':
		return true;
	default:
		return false;
	}
}
