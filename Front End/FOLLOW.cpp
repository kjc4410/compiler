#include<iostream>
#include<string>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<set>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<chrono>
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
	Nonterminal = 0,
	Terminal,
	Operator,
};
bool isOperator(char ch);
void FIRST(string s);
void FOLLOW(string s);
void Processing(string s);

string arrow = "->";
string starting;
using Head = string;
using Body = string;
using rhsTokens = vector<pair<string, Tokens>>;


typedef struct RuleInfo{
	string head;
	int RuleNum;
}RuleInfo;

vector<pair<RuleInfo, rhsTokens>> Production;
// lhs에서 body를 토큰으로 구분, 중복허용
// head는 생성규칙의 lhs 

set<string> NontermTable;
unordered_set<string> TermTable;

unordered_map<string, vector<string>> FIRST_SET;          //FIRST 저장
unordered_map<string, vector<string>> FOLLOW_SET;         //FOLLOW 저장

// rhsTokens = vector<pair<string, Tokens>>
void FIRST(string nonterminal){

	for(auto &i : Production){
		if(nonterminal == i.first.head){
			string head = i.first.head;
			rhsTokens body = i.second;

			for(int i=0;i<body.size();i++){

				if(body[i].second == Terminal){
					if(find(FIRST_SET[nonterminal].begin(), FIRST_SET[nonterminal].end(), body[0].first)==FIRST_SET[nonterminal].end())
						FIRST_SET[head].push_back(body[i].first);
					break;
				}
				else if(body[i].second == Nonterminal){
					// 해당 Nonterminal의 규칙으로 가서 first 탐색
					FIRST(body[i].first);
					bool isEpsilon = false;
					
					//해당 Nonterminal의 FIRST_SET 다 추가
					for(auto &t : FIRST_SET[body[i].first]){
						if(find(FIRST_SET[nonterminal].begin(), FIRST_SET[nonterminal].end(), t) == FIRST_SET[nonterminal].end())
							FIRST_SET[nonterminal].push_back(t);
						
						if(t == "epsilon")
							isEpsilon=true;
					}
					if(isEpsilon == false) break;
				}
			}
		}
	}
}
void FOLLOW() {
	for (auto& nonterm : NontermTable){
		for (auto& i : Production) {
			rhsTokens rule_body = i.second;
			string rule_name = i.first.head;
			
			for (int j = 0; j < rule_body.size(); j++) {

				if (rule_body[j].first == nonterm && j + 1 != rule_body.size()) {
					int cur = j;

					while(cur < rule_body.size()-1){
						
						//다음 토큰이 terminal이라면
						if (rule_body[cur + 1].second == Terminal){
							if (find(FOLLOW_SET[nonterm].begin(), FOLLOW_SET[nonterm].end(), rule_body[cur + 1].first) == FOLLOW_SET[nonterm].end())
								FOLLOW_SET[nonterm].push_back(rule_body[cur + 1].first);
							break;
						}
						// 다음 토큰이 nonterminal일 때
						else if(rule_body[cur+1].second == Nonterminal){
							bool isEpsilon = false;

							for(auto &k : FIRST_SET[rule_body[cur+1].first]){
								if(k == "epsilon")
									isEpsilon = true;

								else if(k != "epsilon")
									if (find(FOLLOW_SET[nonterm].begin(), FOLLOW_SET[nonterm].end(), k) == FOLLOW_SET[nonterm].end())
										FOLLOW_SET[nonterm].push_back(k);	
							}
							if(isEpsilon) cur++;
							else break;
						}
						else if(cur >= rule_body.size()) break;
					}
				}
			}
		}
	}

	bool updated = true;
	while (updated) {
		updated = false;
		for (auto& nonterm : NontermTable) {
			for (auto& i : Production) {
				rhsTokens rule_body = i.second;
				string rule_name = i.first.head;
				for (int j = 0; j < rule_body.size(); j++) {

					if (rule_body[j].first == nonterm && j + 1 == rule_body.size()) { //규칙에서 nonterminal 찾기
						string cur = rule_body[j].first;

						for (auto& k : FOLLOW_SET[rule_name]) {

							if (find(FOLLOW_SET[cur].begin(), FOLLOW_SET[cur].end(), k) == FOLLOW_SET[cur].end()) {
								FOLLOW_SET[cur].push_back(k);
								updated = true;
							}
						}

						// 맨 뒤의 noterminal들의 first를 확인하고 nullable할 때 이전 토큰이 nonterminal인지 확인하고 
						// 맞다면 이전 nonterminal에도 head의 follow 추가하고 다시 이전 nonterminal의 first 검사 <--- 이를 반복 
						int cnt = 0;
						while (cnt < rule_body.size()) {
							bool isEpsilon = false;
							string curToken = rule_body[j-cnt].first;
							for (auto& k : FIRST_SET[curToken])
								if (k == "epsilon") {

									isEpsilon = true;
									cnt++;
									Tokens KindOfBackToken = rule_body[j - cnt].second;
									if (KindOfBackToken == Nonterminal) {

										string back = rule_body[j - cnt].first;
										for (auto& k : FOLLOW_SET[rule_name]) {
											if (find(FOLLOW_SET[back].begin(), FOLLOW_SET[back].end(), k) == FOLLOW_SET[back].end()) {
												FOLLOW_SET[back].push_back(k);
												updated = true;
											}
										}
									}
									else
										break;
								}
							if (isEpsilon == false)
								break;
						}
					}
				}
			}
		}
	}
}

void LOOKAHEAD(){

}

void Processing() {
	int counting = 0;
	for (auto& line : Grammar) {
		int lhs_end_index = line.find(arrow);

		string LHS;
		for (int i = 0; i < lhs_end_index; i++)
			if (line[i] != ' ')
				LHS += line[i];

		NontermTable.insert(LHS);
		if (counting == 0)
			starting = LHS;

		// arrow size가 2
		string RHS = line.substr(lhs_end_index + 2, line.size() - lhs_end_index + 2);

		istringstream rhs(RHS);
		string alternative;
		while (getline(rhs, alternative, '|')) {

			int i = 0;
			rhsTokens body;
			RuleInfo ruleinfo;
			while (i < alternative.size()) {
				string token;
				Tokens kind;
				if (alternative[i] == ' ') {
					i++;
					continue;
				}
				else if (isupper(alternative[i])) {
					while (isupper(alternative[i]))
						token += alternative[i++];
					kind = Nonterminal;
					NontermTable.insert(token);
				}
				else if (islower(alternative[i])) {
					while (islower(alternative[i]))
						token += alternative[i++];
					kind = Terminal;
					TermTable.insert(token);
				}
				else {
					while (isOperator(alternative[i]))
						token += alternative[i++];
					kind = Terminal;
					TermTable.insert(token);
				}
				body.push_back({token, kind});
			}		
			ruleinfo.head=LHS;
			ruleinfo.RuleNum = counting;
			Production.push_back({ruleinfo, body});
		}
		counting++;	
	}
	std::cout << "--Rule-- \n";
	for (auto& i : Production){
        std::cout << i.first.head << "-> "; 
		for(auto& j: i.second)
            std::cout<< j.first<<" ";
	    std::cout << endl;
    }

	std::cout << "Terminal 종류: \n";
	for (auto& i : TermTable)
		std::cout << i << " " << endl;
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

	chrono::system_clock::time_point starts = chrono::system_clock::now();
	std::cout << "\n------FIRST------\n";
	for (auto& i : NontermTable) {
		std::cout << "FIRST(" << i << ")= { ";
		FIRST(i);
		for (auto& j : FIRST_SET[i])
			std::cout << j << " ";
		std::cout << "}\n";
	}
	chrono::system_clock::time_point ends = chrono::system_clock::now();
	chrono::nanoseconds nano = ends-starts;
	std::cout << "time: "<< nano.count() <<"seconds"<<endl;

	FOLLOW_SET[starting].push_back("$");
	FOLLOW();

	std::cout << "\n-----FOLLOW-----\n";
	for (auto& i : NontermTable) {
		std::cout << "FOLLOW(" << i << ")= { ";

		for (auto& j : FOLLOW_SET[i])
			std::cout << j << " ";
		std::cout << "}\n";

	}
	return 0;
}

bool isOperator(char ch) {
	switch (ch) {
	case '+': case '*': case '-': case '/':
	case '%': case'(': case ')': case'#':
		return true;
	default:
		return false;
	}
}
