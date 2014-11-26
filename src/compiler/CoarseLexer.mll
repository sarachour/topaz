
{
open CoarseParser

}

let ws_ = [' ' '\t']+
let notws_ = [^' ' '\n' '\t' '{''}''('')']+
let notws_literal_ = ['"'][^'"']*['"']
let comment_ = "/*"([^'*']|"*"[^'/'])*"*/"
let comment2_ = "//"[^'\n']*"\n"
let preprocess_ = "#"[^'\n']*"\n"

rule main = parse
	comment_ as cmnt			{TOKEN(cmnt)}
	|comment2_ as cmnt			{TOKEN(cmnt)}
	| preprocess_ as ppc 		{TOKEN(ppc)}
	|"{" 						{OBRACE}
	|"}" 						{CBRACE}
	|"(" 						{OPARAN}
	|")" 						{CPARAN}
	|"\n" 						{ENDLINE}
	|notws_literal_ as fragment {TOKEN(fragment)}
	|notws_ as fragment 		{TOKEN(fragment)}
	|ws_ 						{main lexbuf}
	|eof 						{EOF}
