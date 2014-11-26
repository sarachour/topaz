%{
open Data
%}

%token EOF
%token<string> TOKEN
%token ENDLINE
%token OPARAN CPARAN
%token OBRACE CBRACE
%token TASKSET

%start main
%type <Data.exp> main
%type <Data.exp> single
%type <Data.exp> brace
%type <Data.exp> compose
%%

main: 
	compose	{$1}
	| EOF	{NilExp}
;
single:
	ENDLINE {Endline}
	|TOKEN {
		let escaped1 = Str.global_replace (Str.regexp "\\") "\\\\\\\\" $1 in
		let escaped2 = Str.global_replace (Str.regexp "\"") "\\\"" escaped1 in
			TokenExp(escaped2)
	}
;
brace:
	OBRACE compose CBRACE {BraceExp($2)}
	| OBRACE CBRACE {BraceExp(NilExp)}
	| OPARAN compose CPARAN {ParenExp($2)}
	| OPARAN CPARAN {ParenExp(NilExp)}
;
compose:
	single {ComposeExp([$1])}
	|brace {ComposeExp([$1])}
	| compose single {match $1 with ComposeExp(e) -> ComposeExp(e@[$2]) | _-> ErrorExp("compose exp expected")}
	| compose brace {match $1 with ComposeExp(e) -> ComposeExp(e@[$2]) | _-> ErrorExp("compose exp expected")}
;
