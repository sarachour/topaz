%{
open Data
%}

%token<char> TOK
%token<string> WORD

%token COMMA
%token EQ
%token GT
%token GTEQ
%token LT
%token LTEQ
%token SET
%token PLUSEQ
%token EOF
%token SEMICOLON

%token OPENBRACKET
%token CLOSEBRACKET
%token OPENBRACE
%token CLOSEBRACE
%token <Data.vartype> TYPE
%token CONST

%start range
%start varlist
%type <Data.rangeinfo> range
%type <Data.varinfo list> varlist
%type <Data.varinfo> var
%type <string> word
%type <string> expr
%type <string> uexpr
%%

range: 
	word SET expr SEMICOLON word LT expr SEMICOLON word PLUSEQ expr {RangeInfo($1,VarExpr($3),VarExpr("("^$7^")-1"),VarExpr($11))}
	|word SET expr SEMICOLON word LTEQ expr SEMICOLON word PLUSEQ expr {RangeInfo($1,VarExpr($3),VarExpr($7),VarExpr($11))}
;

var:
	TYPE WORD SET expr 								   	 {ScalarVarInfo($1,$2,VarExpr($4))}
	| TYPE WORD OPENBRACKET expr CLOSEBRACKET SET expr   {ArrayVarInfo($1,$2,VarExpr($4),VarExpr($7))}
	| TYPE WORD  										 {ScalarVarInfo($1,$2,VarNilExpr)}
	| TYPE WORD OPENBRACKET expr CLOSEBRACKET            {ArrayVarInfo($1,$2,VarExpr($4),VarNilExpr)}
	| CONST TYPE WORD OPENBRACKET expr CLOSEBRACKET SET expr   {ConstArrayVarInfo($2,$3,VarExpr($5),VarExpr($8))}
	| CONST TYPE WORD OPENBRACKET expr CLOSEBRACKET            {ConstArrayVarInfo($2,$3,VarExpr($5),VarNilExpr)}
	| CONST TYPE WORD SET expr 								   	 {ConstScalarVarInfo($2,$3,VarExpr($5))}
	| CONST TYPE WORD  										 {ConstScalarVarInfo($2,$3,VarNilExpr)}
	
;
varlist:
	var						{[$1]}
	| varlist COMMA var {$1 @ [$3]}
	| var EOF {[$1]}
;
word:
	TOK {Char.escaped $1}
	| word TOK  {$1 ^ (Char.escaped $2)}
;


uexpr:
	expr {$1}
	| uexpr COMMA expr {$1^","^$3}
	
expr:
    | {""}
	| expr TOK  {$1 ^ (Char.escaped $2)}
	| expr WORD {$1 ^ $2}
	| expr EQ {$1 ^ "=="}
	| expr GT {$1 ^ ">"}
	| expr GTEQ {$1 ^ ">="}
	| expr LT {$1 ^ "<"}
	| expr LTEQ {$1 ^ "<="}
	| expr TYPE {
		match $2 with
			VarInt -> $1^"int"
			|VarFloat -> $1^"float"
			|VarDouble -> $1^"double"
			|VarChar -> $1^"char"
	}
	| expr OPENBRACKET uexpr CLOSEBRACKET {$1^"["^$3^"]"}
	| expr OPENBRACE uexpr CLOSEBRACE {$1^"("^$3^")"}


