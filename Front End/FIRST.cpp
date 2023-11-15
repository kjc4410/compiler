#include<iostream>
#include<string>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<vector>
#include<unordered_map>
using namespace std;
vector<string> Grammar;
enum Tokens {
	Nonterminal=0,
	Terminal,
	Operator,
};
bool isOperator(char ch);
void FIRST(string s);
void FOLLOW(string s);
void Processing();

string arrow = "->";
using Head = string;
using Body = string;
using lhs = vector<pair<string, Tokens>>;

unordered_map<Head, Body> Production;
unordered_multimap<Head, Body> Rule;

// lhs에서 body를 토큰화하기 위해 
unordered_multimap<string, lhs> Rules;

unordered_map<string, Tokens> TerminalTable;
unordered_map<string, Tokens> NonterminalTable;



void FIRST(string s) {
	if (isupper(s[0])) {
		auto range = Rules.equal_range(s);
		//unordered_multimap에서 중복되는 값들을 찾는 과정
		for (auto it = range.first; it != range.second; it++) {
			// vector<pair<string, Tokens>>에 접근
			lhs vec = it->second;
			if (vec[0].second == Terminal)
				cout <<vec[0].first<<", ";
			else
				FIRST(vec[0].first);
		}
	}
}
void Processing() {
	for (auto& line : Grammar) {
		int index = line.find(arrow);

		string LHS;
		for (int i = 0;i < index;i++)
			if (line[i] != ' ')
				LHS += line[i];
		//cout << "자른 LHS: " << LHS << "$\n";
		NonterminalTable.insert({ LHS, Nonterminal });

		string RHS = line.substr(index + 2, line.size() - index + 2);
		Production.insert({ LHS, RHS });

		istringstream rhs(RHS);
		string p;
		while (getline(rhs, p, '|')) {
			Rule.insert({ LHS,p });
			int i = 0;
			lhs body;
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
					NonterminalTable.insert({ token, kind });
				}
				else if (islower(p[i])) {
					while (islower(p[i]))
						token += p[i++];
					kind = Terminal;
					TerminalTable.insert({ token, Terminal });
				}
				else {
					while (isOperator(p[i]))
						token += p[i++];
					kind = Terminal;
					TerminalTable.insert({ token, Operator });
				}
				body.push_back(make_pair(token, kind));
			}
			Rules.insert({LHS, body});
		}
	}
	cout << "--Rule-- \n";
	for (auto& i : Rule)
		cout << i.first << ": " << i.second << endl;
	cout << endl;

	cout << "Terminal 종류: \n";
	for (auto& i : TerminalTable)
		cout <<i.first<<": "<< i.second << endl;
	cout << endl;
	
	cout << "Nonterminal 종류: \n";
	for (auto& i : NonterminalTable)
		cout << i.first << ": " << i.second << endl;
	cout << endl;
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
		cout << "error";
	Processing();

	for (auto& i : NonterminalTable) {
		cout << "FIRST(" << i.first << ")= { ";
		FIRST(i.first);
		cout << "}\n";
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
