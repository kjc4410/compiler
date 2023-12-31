#include<stdio.h>
#include<string.h>
#include<ctype.h>
#define NO_KEYWORD 14 // 단어 심벌의 개수
#define ID_LENGTH 12  // 유효한 명칭의 길이를 정의하는 상수
FILE* sourceFile;

int hexValue(char ch);
int getNumber(char firstCharacter);
int superLetterOrDigit(char ch);
int superLetter(char ch);
void lexicalError(int n);
void printToken(struct tokenType token);
struct tokenType scanner();

struct tokenType {
    int number; //토큰 번호를 갖는 필드
    int line; //각 라인을 정의
    union {
        char id[ID_LENGTH]; // 토큰 값을 갖는 명칭
        int num;               // 토큰 값을 갖는 상수
        /*   parser가 토큰을 요구할 때 scanner는 이 구조체의 값을 return   */
    } value;
};

enum tsymbol { // 49개
    tnull = -1,
    tnot,       tnotequ,    tremainder,     tremAssign,     tident,     tnumber,
    /* 0         1              2              3              4           5        */
    tand,       tlparen,      trparen,        tmul,        tmulAssign,    tplus,
    /* 6         7             8              9              10           11            */
    tinc,      taddAssign,   tcomma,        tminus,         tdec,       tsubAssign,
    /* 12       13            14             15             16            17            */
    tdiv,      tdivAssign,     tcolon,       tsemicolon,      tless,       tlesse,    
    /* 18       19            20             21             22            23            */
    tassign,    tequal,       tgreat,       tgreate,        tlbracket,     trbracket, 
    /* 24       25            26             27             28           29           */
    teof,
    /* 30                                                                             */

    //   ...........    word symbols ................................. //
    tbreak,     tcase,       tconst,      tcontinue,      tdefault,
    /* 31        32           33           34              35                          */
    tdo,         telse,        tfor,          tif,            tint,         treturn,
    /* 36        37           38           39              40            41            */
    tswitch,     tvoid,       twhile,      tlbrace,           tor,          trbrace
    /* 42         43          44           45              46             47            */
};
const char* tokenName[] = { // 48개
    "!",             "!=",        "%",       "%=",      "%ident",    "%number",
    /* 0            1          2          3           4             5     */
    "&&",           "(",         ")",         "*",         "*=",           "+",
    /* 6            7          8          9          10            11       */
    "++",           "+=",        ",",         "-",         "--",	        "-=",
    /* 12          13         14        15           16           17        */
    "/",            "/=",        ":",         "; ",          "<",          "<= ",
    /* 18          19         20         21          22           23       */
    " = ",          "==",        ">",        ">=",         "[",           "]",
    /* 24          25         26         27          28           29      */

    //   ...........    word symbols ................................. //
    "eof",         "break",     "case",     "const",    "continue",   "default",
    /* 30          31         32         33         34            35   */
    "do",          "else",       "for",       "if",        "int",       "return",
    /* 36          37         38         39         40            41     */
    "switch",      "void",      "while",      "{",         "||",            "}"
    /* 42          43         44         45         46           47      */
};  
const char* keyword[NO_KEYWORD] = { "break",  "case",  "const",  "continue",  "default",
                                           "do",     "else",    "for",       "if",      "int",
                                          "return",  "switch",  "void",   "while" };
enum tsymbol tnum[NO_KEYWORD] = {
      tbreak, tcase, tconst, tcontinue, tdefault,
      tdo, telse,  tfor,  tif,  tint,   treturn,
      tswitch, tvoid,  twhile
};

int main() {
    sourceFile = fopen("test.mc", "r");
    scanner();
    fclose(sourceFile);
    return 0;
}

struct tokenType scanner()
{
    struct tokenType token;
    int i, index;
    char ch, id[ID_LENGTH];

    token.number = tnull;
    do {
         ch = fgetc(sourceFile);
        if (ch == ' ' || ch == '\t' || ch == '\r')
            continue; // 공백 문자는 토큰으로 처리하지 않고 건너뜀
        if (ch == '\n') { // 엔터키(개행 문자)를 만났을 때
            token.line++; // 라인 수 증가
            continue; // 개행 문자는 토큰으로 처리하지 않고 건너뜀
        }
        if (superLetter(ch)) { // 알파벳 or _인지 체크 && 처음엔 숫자가 나오면 안되기에

            i = 0;
            do {
                if (i < ID_LENGTH)
                    id[i++] = ch; //id값에 문자 저장

                ch = fgetc(sourceFile); //문자 하나 읽어옴

            } while (superLetterOrDigit(ch)); //알파벳 혹은 숫자인지 체크

            if (i >= ID_LENGTH) // 초과시 에러
                lexicalError(1); // 식별자 길이는 12보다 작아야 한다.
            id[i] = '\0'; //입력한 문자열의 마지막에 \0 입력
            ungetc(ch, sourceFile);  //  retract

            // 키워드 테이블에서 identifial 찾기
            for (index = 0; index < NO_KEYWORD; index++)
                if (!strcmp(id, keyword[index])) // id와 keyword 비교
                    break;

            if (index < NO_KEYWORD)    // found, keyword exit
                token.number = tnum[index]; //keyword 결정
            else {                     // not found, identifier exit
                token.number = tident;
                strcpy(token.value.id, id); // 식별자 결정
            }
        }  // keyword인지 식별자인지 확인하는 과정

        else if (isdigit(ch)) {  // number인지 확인
            token.number = tnumber;
            token.value.num = getNumber(ch); //숫자이면 값으로 판단
        }
        else switch (ch) {  // special character
        case '/':
            ch = fgetc(sourceFile);
            if (ch == '*')			// text comment : /* */인지 확인
                do {
                    while (ch != '*')
                        ch = fgetc(sourceFile);
                    ch = fgetc(sourceFile);
                } while (ch != '/');
            else if (ch == '/')		// line comment: //인지 확인
                while (fgetc(sourceFile) != '\n');
            else if (ch == '=')     //            "/=인지 확인"
                token.number = tdivAssign;
            else {
                token.number = tdiv;   //        "/인지 확인"
                ungetc(ch, sourceFile); // retract
            }
            break;
        case '!':
            ch = fgetc(sourceFile);
            if (ch == '=')  token.number = tnotequ; //     "!="인지 확인
            else {
                token.number = tnot;               //      "!"인지 확인
                ungetc(ch, sourceFile); // retract
            }
            break;
        case '%':
            ch = fgetc(sourceFile);
            if (ch == '=') {
                token.number = tremAssign;       //        "%="인지 확인
            }
            else {
                token.number = tremainder;       //            "%"인지 확인
                ungetc(ch, sourceFile);
            }
            break;
        case '&':
            ch = fgetc(sourceFile);
            if (ch == '&')
                token.number = tand;          //       "&"인지 확인
            else {
                lexicalError(2);
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '*':
            ch = fgetc(sourceFile);
            if (ch == '=')
                token.number = tmulAssign;     //   "*="인지 확인
            else {
                token.number = tmul;            //    "*"
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '+':
            ch = fgetc(sourceFile);
            if (ch == '+')
                token.number = tinc;
            else if (ch == '=')
                token.number = taddAssign;
            else {
                token.number = tplus;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '-':
            ch = fgetc(sourceFile);
            if (ch == '-')
                token.number = tdec;
            else if (ch == '=')
                token.number = tsubAssign;
            else {
                token.number = tminus;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '<':
            ch = fgetc(sourceFile);
            if (ch == '=')
                token.number = tlesse;
            else {
                token.number = tless;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '=':
            ch = fgetc(sourceFile);
            if (ch == '=')
                token.number = tequal;
            else {
                token.number = tassign;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '>':
            ch = fgetc(sourceFile);
            if (ch == '=')
                token.number = tgreate;
            else {
                token.number = tgreat;
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '|':
            ch = fgetc(sourceFile);
            if (ch == '|')
                token.number = tor;
            else {
                lexicalError(3);
                ungetc(ch, sourceFile);  // retract
            }
            break;
        case '(': token.number = tlparen;         break;
        case ')': token.number = trparen;         break;
        case ',': token.number = tcomma;          break;
        case ';': token.number = tsemicolon;      break;
        case '[': token.number = tlbracket;       break;
        case ']': token.number = trbracket;       break;
        case '{': token.number = tlbrace;         break;
        case '}': token.number = trbrace;         break;
        case EOF: token.number = teof;            break;
        default: {
            printf("Current character : %c", ch);
            lexicalError(4);
            break;
        }
        } // switch end
    } while (token.number != teof);
    //printf("%d\n", token.number);
    return token;
} // end of scanner

void lexicalError(int n){
    printf(" *** Lexical Error : ");
    switch (n) {
    case 1: printf("an identifier length must be less than 12.\n");
        break;
    case 2: printf("next character must be &\n");
        break;
    case 3: printf("next character must be |\n");
        break;
    case 4: printf("invalid character\n");
        break;
    }
}
int superLetter(char ch){ //알파벳 혹은_인지 체크 : 숫자가 제일 먼저 나올 수 없기에
    if (isalpha(ch) || ch == '_') return 1; /*  isalpha함수 파라미터에 넣어서 알파벳인지 판별*/
    else return 0;
}
int superLetterOrDigit(char ch){
    if (isalnum(ch) || ch == '_') return 1; /* c가 알파벳이거나 숫자인지 판별
                                           알파벳 대문자: 1, 알파벳 소문자: 2, 숫자: 4 else: 0 */
    else return 0;
}
int getNumber(char firstCharacter){
    int num = 0;
    int value;
    char ch;

    if (firstCharacter == '0') { //첫 번째 문자가 0이면
        ch = fgetc(sourceFile);
        if ((ch == 'X') || (ch == 'x')) {		// hexa decimal
            while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
                num = 16 * num + value;
        }
        else if ((ch >= '0') && (ch <= '7'))	// octal
            do {
                num = 8 * num + (int)(ch - '0');
                ch = fgetc(sourceFile);
            } while ((ch >= '0') && (ch <= '7'));
        else num = 0;						// zero
    }
    else {									// decimal
        ch = firstCharacter;
        do {
            num = 10 * num + (int)(ch - '0');
            ch = fgetc(sourceFile);
        } while (isdigit(ch));
    }
    ungetc(ch, sourceFile);  /*  ch는 다시 넣을 문자, sourceFile은 문자를
                                            다시 넣을 파일 성공 시 ch리턴  */
    return num;
}
int hexValue(char ch){ //16진수 변환
    switch (ch) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return (ch - '0');
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return (ch - 'A' + 10);
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return (ch - 'a' + 10);
    default: return -1;
    }
}
void printToken(struct tokenType token){
    if (token.number == tident)
        printf("Token ID = \'%s\' \n", token.value.id);
    else if (token.number == tnumber)
        printf("Token num = \'%d\'\n", token.value.num);
    else
        printf("Token Symbol = \'%s\'\n", tokenName[token.number]);
}
