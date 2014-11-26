open Data
open Exp
open TaskSet

open Printf


let parse stream =
	let coarse stream = 
		CoarseParser.main CoarseLexer.main stream
	in
	let fine ast =
		let rec _sanitize_list lst = 
			match lst with
				NilExp::t -> _sanitize_list t
				| Endline::t -> _sanitize_list t
				| h::t -> h::(_sanitize_list t)
				| [] -> []
		in
		let rec _parse_list lst = 
			match lst with
			TokenExp("combine")::BraceExp(ComposeExp(e))::rest -> Combine(e)::(_parse_list rest)
			|TokenExp("compute")::TokenExp("in")::ParenExp(input)::TokenExp("out")::ParenExp(output)::BraceExp(ComposeExp(b))::rest ->
				let 
					in_info = FineParser.varlist FineLexer.varlist (Lexing.from_string (print input)) 
				in
				let 
					out_info = FineParser.varlist FineLexer.varlist (Lexing.from_string (print output)) 
				in
				(
					Compute(in_info, out_info, b)::(_parse_list rest);
				)
			|TokenExp("transform")::TokenExp("in")::ParenExp(input)::TokenExp("out")::ParenExp(output)::BraceExp(ComposeExp(blk))::rest -> 
				let 
					out_info = FineParser.varlist FineLexer.varlist (Lexing.from_string (print output)) 
				in
				let 
					in_info = FineParser.varlist FineLexer.varlist (Lexing.from_string (print input)) 
				in
				Transform(in_info, out_info,blk)::(_parse_list rest)
			|TokenExp("before")::BraceExp(ComposeExp(e))::rest -> 
				Before(e)::(_parse_list rest)
			|h::rest -> (_parse_list rest)
			| [] -> []
		in
		let rec _parse_blocks blocks = 
			match blocks with
				ComposeExp(e) -> (_parse_list (_sanitize_list e))
				|BraceExp(e) -> _parse_blocks e
				|_ -> []
					
		in
		let rec _fine_taskset name range blocks = 
			let 
				range_info = FineParser.range FineLexer.range (Lexing.from_string (print range))
			in 
			let 
				block_info = _parse_blocks blocks;
			in
				TaskSetExp(name, range_info, block_info)
		in
		let rec _fine ast = 
			match ast with 
				BraceExp(e) -> BraceExp(_fine e)
				| ParenExp(e) -> ParenExp(e)
				| ComposeExp(TokenExp("taskset")::TokenExp(name)::ParenExp(range)::Endline::BraceExp(blocks)::t) ->
					join (_fine_taskset name range blocks) (_fine (ComposeExp t))
				| ComposeExp(TokenExp("taskset")::TokenExp(name)::ParenExp(range)::BraceExp(blocks)::t) ->
					join (_fine_taskset name range blocks) (_fine (ComposeExp t))
				| ComposeExp(h::t) ->
					join (_fine h) (_fine (ComposeExp t))
				| n -> n
			
		in
			_fine ast
		
	in
		let cast = coarse stream in
		let fast = fine cast in
		fast
