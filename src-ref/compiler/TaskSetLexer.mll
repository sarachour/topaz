
{
open TaskSetParser

}

let ws_ = [' ' '\n' '\t']+
let nonws_ = [^' ' '\n' '\t' '[' ']' '=' ';']+

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
	| "="						{ SET }			 
	| "int"						{ TYPE("int") }	
	| "float"					{ TYPE("float") }	
	| "double"					{TYPE("double") }	
	| "char"					{ TYPE("char") }	
	| ";" 				   		{ SEMICOLON }
	| ws_						{ varlist lexbuf }
	| nonws_  as c				{ WORD(c) }
	| eof						{ EOF } 
