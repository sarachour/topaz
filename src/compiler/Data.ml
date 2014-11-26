(*variable information*)
type vartype = VarFloat | VarDouble | VarChar | VarInt
type varexpr = VarExpr of string | VarNilExpr
type varinfo = ScalarVarInfo of vartype*string*varexpr | ArrayVarInfo of vartype*string*varexpr*varexpr |
				ConstScalarVarInfo of vartype*string*varexpr | ConstArrayVarInfo of vartype*string*varexpr*varexpr;;
type rangeinfo = RangeInfo of string*varexpr*varexpr*varexpr;;

(*task information*)
type exp = TaskSetExp of string*rangeinfo*(taskblock list) | ErrorExp of string | NilExp | ParenExp of exp | BraceExp of exp | ComposeExp of exp list | TokenExp of string | Endline

and taskblock = Compute of (varinfo list)*(varinfo list)*(exp list) |
				 Combine of exp list |
				 Transform of (varinfo list)*(varinfo list)*(exp list) |
				 Before of exp list 
				  
