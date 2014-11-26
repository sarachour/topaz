%{
open Data
%}

%token<char> TOK
%token<string> WORD

%token SEMICOLON
%token EQ
%token GT
%token GTEQ
%token LT
%token LTEQ
%token SET
%token PLUSEQ
%token EOF


%token OPENBRACKET
%token CLOSEBRACKET
%token <string> TYPE

%start range
%start varlist
%type <Data.range> range
%type <Data.var list> varlist
%type <Data.var> var
%type <string> word
%type <string> expr
%%

range: 
	word SET expr SEMICOLON word LT expr SEMICOLON word PLUSEQ expr {{rangeRank=$1; rangeMin=$3; rangeMax=$7; rangeStep = $11}}
;

var:
	TYPE WORD SET expr 								   	 {{varType=$1; varName=$2; varSize="1"; varValue=$4}}
	| TYPE WORD OPENBRACKET expr CLOSEBRACKET SET expr   {{varType=$1; varName=$2; varSize=$4; varValue=$7}}
	| TYPE WORD  										 {{varType=$1; varName=$2; varSize="1"; varValue=""}}
	| TYPE WORD OPENBRACKET expr CLOSEBRACKET            {{varType=$1; varName=$2; varSize=$4; varValue=""}}
;
varlist:
	var						{[$1]}
	| varlist SEMICOLON var {$1 @ [$3]}
;
word:
	TOK {Char.escaped $1}
	| word TOK  {$1 ^ (Char.escaped $2)}
;

expr:
	WORD {$1}
	|TOK  {Char.escaped $1}
	| expr WORD  {$1 ^ ($2)}
	| expr TOK  {$1 ^ (Char.escaped $2)}
	| expr EQ {$1 ^ "=="}
	| expr GT {$1 ^ ">"}
	| expr GTEQ {$1 ^ ">="}
	| expr LT {$1 ^ "<"}
	| expr LTEQ {$1 ^ "<="}
	| expr TYPE {$1^$2}
	| expr OPENBRACKET {$1^"["}
	| expr CLOSEBRACKET {$1^"]"}
