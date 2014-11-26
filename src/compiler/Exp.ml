open TaskSet
open Util
open Data

let rec print tree = 
	let rec _print tree depth =
		let rec print_tabs count = match count with
			0 -> ""
			| n ->  "\t"^(print_tabs (n-1))
		in
		let rec print_list lst depth = match lst with
			TokenExp(e)::t -> (_print (TokenExp e) depth)^" "^(print_list t depth)
			| h::t -> (_print h depth)^(print_list t depth)
			|[] -> ""
		in
		match tree with
			ErrorExp s -> "Error: "^s
			| NilExp -> ""
			| ParenExp e -> "("^(_print e depth)^")"
			| BraceExp e -> 
				let (body, newline) = 
					match e with
						ComposeExp(h::t) -> (
							match (get_back (h::t)) with
								Endline -> (ComposeExp(pop_back (h::t)), Endline)
								|_ -> (ComposeExp(h::t), NilExp)
							)
						| ComposeExp([]) -> (NilExp, NilExp)
					| Endline -> (NilExp, Endline)
					| _ -> (e, NilExp)
				in
					"{"^(_print body (depth+1))^(_print newline (depth))^"}"
			| ComposeExp elst -> print_list elst depth
			| TokenExp s -> s
			| Endline -> "\n"^(print_tabs depth)
			| TaskSetExp (name, rinfo, blocks) -> 
				let print_vexpr expr = match expr with
						VarExpr(e) -> (e)
						| VarNilExpr -> "NULL"
				in
				let newline = "\n"^(print_tabs depth) in
				let print_type typ = match typ with
					VarFloat -> "float" | VarDouble -> "double" | VarInt -> "int" | VarChar -> "char"
				in
				let rec _print_vlist lst = match lst with
					ScalarVarInfo(typ, name, e)::t -> 
						((print_type typ)^" "^name^":="^(print_vexpr e)^newline)^(_print_vlist t)
					|ArrayVarInfo(typ, name, siz, e)::t -> 
						((print_type typ)^"*"^" "^name^"["^(print_vexpr siz)^"] :="^(print_vexpr e)^newline)^(_print_vlist t)
					|ConstScalarVarInfo(typ, name, e)::t -> 
						("const "^(print_type typ)^" "^name^":="^(print_vexpr e)^newline)^(_print_vlist t)
					|ConstArrayVarInfo(typ, name, siz, e)::t -> 
						("const "^(print_type typ)^"*"^" "^name^"["^(print_vexpr siz)^"] :="^(print_vexpr e)^newline)^(_print_vlist t)
					|[] -> "<END OF LIST>"
				in
				let print_block blk = match blk with
					Compute(inp, outp, blk) -> 
						let in_str = _print_vlist inp in
						let out_str = _print_vlist outp in
						"COMPUTE!:"^newline^"INPUTS:"^newline^in_str^newline^"OUTPUTS:"^newline^out_str^newline^(_print (ComposeExp blk) (depth+1))
					| Combine(blk) -> "COMBINE:"^newline^(_print (ComposeExp blk) (depth+1))
					| Transform(vin,vout,blk) -> "TRANSFORM:"^newline^
						(_print_vlist vin)^newline^
						(_print_vlist vout)^newline^
						(_print (ComposeExp blk) (depth+1))
					| Before(blk) -> "BEFORE:"^newline^(_print (ComposeExp blk) (depth+1))
				in
				let rec print_blocks blks = match blks with
					h::t -> (print_block h)^newline^(print_blocks t)
					|[] -> newline
				in
				let print_range rinfo = match rinfo with
					RangeInfo(idx, min, max, step) ->
						"range["^idx^"]: min:"^(print_vexpr min)^" max:"^(print_vexpr max)^" step:"^(print_vexpr step)
				in
				"<taskset "^name^">"^newline^(print_range rinfo)^newline^newline^
				(print_blocks blocks)
	in
		_print tree 0


let rec join e1 e2 = match (e1, e2) with
	(ComposeExp(l1), ComposeExp(l2)) -> ComposeExp(l1 @ l2)
	|(ComposeExp(l1), n) -> ComposeExp(l1 @ [n])
	|(n, ComposeExp(l2)) -> ComposeExp(n::l2)
	|(n1, n2) -> ComposeExp(n1::n2::[])
