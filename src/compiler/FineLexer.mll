
{
open FineParser
open Data
}

let ws_ = [' ' '\n' '\t']+
let nonws_ = [^' ' '\n' '\t' '[' ']' '=' ',' ';' '(' ')']+

rule range = parse
     "=="				   { EQ }
	| "<="				   { LTEQ }
	| ">="				   { GTEQ }
	| "+="				   { PLUSEQ }
	| "="				   { SET }
	| ">"		   		   { GT }
	| "<"				   { LT }
	| ";" 				   { SEMICOLON }
	| ws_		 		   { range lexbuf }
	|  _  as c	   		   { TOK(c) }
	| eof				   { EOF } 
	

and varlist = parse
	"["							{ OPENBRACKET }
	|"]"						{ CLOSEBRACKET }
	|")"						{ CLOSEBRACE }
	|"("						{ OPENBRACE }
	| "="						{ SET }			 
	| "int"						{ TYPE(VarInt) }	
	| "float"					{ TYPE(VarFloat) }	
	| "double"					{TYPE(VarDouble) }	
	| "char"					{ TYPE(VarChar) }	
	| "const"					{ CONST }
	| "," 				   		{ COMMA }
	| ";" 				   		{ SEMICOLON }
	| ws_						{ varlist lexbuf }
	| nonws_  as c				{ WORD(c) }
	| eof						{ EOF } 
