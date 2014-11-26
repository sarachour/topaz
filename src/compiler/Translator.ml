open Exp
open Data
(*ISSUE: Array size 1 no longer interpereted as array. Should be.*)

let counter = 
	let count = ref (-1) in
	fun () -> incr count; !count
	
	
let translate_task name range blocks =
	let intToken = TokenExp("int") in
	let voidToken = TokenExp("void") in
	let boolToken = TokenExp("bool") in
	let eqToken = TokenExp("=") in
	let lteToken = TokenExp("<=") in
	let scToken = TokenExp(";") in
	let peqToken = TokenExp("+=") in
	let forToken = TokenExp("for") in
	let cToken = TokenExp(",") in
	let ifToken = TokenExp("if") in
	let returnToken = TokenExp("return") in
	let sendFxn = TokenExp("Topaz::topaz->send") in
	let receiveFxn = TokenExp("Topaz::topaz->receive") in
	let unpackInFxn = TokenExp("Topaz::topaz->unpack_inputs") in
	let unpackOutFxn = TokenExp("Topaz::topaz->unpack_outputs") in
	let packOutFxn = TokenExp("Topaz::topaz->pack_outputs") in
	let taskAddFxn = TokenExp("Topaz::topaz->add") in
	let taskCompFxn = TokenExp(name^"_COMPUTE_TPZ") in
	let taskTransFxn = TokenExp(name^"_TRANS_TPZ") in
	let taskId = TokenExp(string_of_int (counter())) in
	let refr expr = match expr with 
		TokenExp(e) -> TokenExp("&"^e) 
		| _ -> raise (Failure "Unknown Expression we're taken reference of") 
	in
	let typ t = match t with
		VarFloat -> TokenExp("float") | VarDouble -> TokenExp("double") | VarInt -> TokenExp("int") | VarChar -> TokenExp("char")
	in
	let ltyp t = match t with
		VarFloat -> TokenExp("FLOAT") | VarDouble -> TokenExp("DOUBLE") | VarInt -> TokenExp("INT") | VarChar -> TokenExp("CHAR")
	in
	let ptr t = match (typ t) with
		TokenExp(s) -> TokenExp(s^"*")
		| _ -> raise (Failure "Impossible, since typ fxn only outputs tokens.") 
	in
	let bracket e = ComposeExp([TokenExp("["); e; TokenExp("]")]) in
	let extract blocks =
		let rec _extract blks ctx =
			match (blks, ctx) with
			(h::t, (vin, vout, bef, cmp, red, tin, tout, trans)) -> 
				let currctx = match h with
					Before(bef) ->
						(vin, vout, bef, cmp, red, tin, tout, trans)
					| Combine(red) ->
						(vin, vout, bef, cmp, red, tin, tout, trans)
					| Transform(tin,tout,trans) ->
						(vin, vout, bef, cmp, red, tin, tout, trans)
					| Compute(vin, vout, cmp) ->
						(vin, vout, bef, cmp, red, tin, tout, trans)
				in
					(_extract t currctx)
			|([], c) -> c
		in
			_extract blocks ([],[],[],[],[],[],[],[])
	in
	let (vin, vout, bef, cmp, red, tin, tout, trans) = extract blocks in
	let (rrank, rmin, rmax, rstep) = match range with 
		RangeInfo(a, VarExpr(b), VarExpr(c), VarExpr(d)) -> (TokenExp(a),TokenExp(b),TokenExp(c),TokenExp(d))
		| _ -> raise (Failure "range must have well defined min, max, step")
	in
	let checker_header = [Endline;(typ VarInt); rrank;scToken;Endline] @ (* Extract inputs, outputs *)
			(List.map (function v -> 
				match v with 
					ScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
					| ArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([(ptr t); TokenExp(n); scToken; Endline])
					| ConstScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
					| ConstArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([(ptr t); TokenExp(n); scToken; Endline])
					| _ -> raise (Failure "every array must have a well defined size")
				) (vin@vout@tout)
			) @
			[unpackInFxn;
			ParenExp(ComposeExp(
				[TokenExp("__INPUT"); cToken; taskId; cToken; refr rrank] @
				(List.map (function inp -> 
					match inp with 
						ScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
						| ArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
						| ConstScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
						| ConstArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
					) 
				vin)
			)); scToken; Endline;] @
			[unpackOutFxn;
			ParenExp(ComposeExp(
				[TokenExp("__OUTPUT"); cToken; taskId; cToken; refr rrank] @
				(List.map (function inp -> 
					match inp with 
						ScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
						| ArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
						| ConstScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
						| ConstArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
					) 
				vout)
			)); scToken; Endline]
	in
	let trans_defn =  ComposeExp([
		Endline; boolToken; taskTransFxn; 
		ParenExp(ComposeExp([
				TokenExp("Task*"); TokenExp("__INPUT"); cToken; TokenExp("Task*"); TokenExp("__OUTPUT")
			])); 
		BraceExp(ComposeExp(
			[Endline] @
			checker_header @
			trans @
			[returnToken; TokenExp("Topaz::topaz->check");
			ParenExp(ComposeExp(
				[taskId; cToken; rrank] @
				  (List.map (function inp -> match inp with
					ScalarVarInfo(_,n,_) -> ComposeExp([cToken; (TokenExp n)])
					|_ -> raise (Failure "no mapping outputs can be arrays.")
				  ) tout)
			));scToken; Endline]
		))
	])
	in
	let comp_defn = 
		ComposeExp([
			Endline; voidToken; taskCompFxn; 
			ParenExp(ComposeExp([
				TokenExp("Task*"); TokenExp("__INPUT"); cToken; TokenExp("Task*"); TokenExp("__OUTPUT")
			])); 
			Endline;
			BraceExp(ComposeExp(
				[Endline; intToken; rrank; scToken; Endline] @
				(List.map (function v -> 
					match v with 
						ScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
						| ArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([(ptr t); TokenExp(n); scToken; Endline])
						| ConstScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
						| ConstArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([(ptr t); TokenExp(n); scToken; Endline])
						| _ -> raise (Failure "every array must have a well defined size")
					) (vin)
				) @
				(List.map (function v -> 
					match v with 
						ScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
						| ArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([typ t; TokenExp(n); (bracket (TokenExp siz)); scToken; Endline])
						| ConstScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
						| ConstArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([typ t; TokenExp(n); (bracket (TokenExp siz)); scToken; Endline])
						| _ -> raise (Failure "every array must have a well defined size")
					) (vout)
				) @
				[unpackInFxn;
				ParenExp(ComposeExp(
					[taskId; cToken; refr rrank] @
					(List.map (function inp -> 
						match inp with 
							ScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
							| ArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
							| ConstScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
							| ConstArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
						) 
					vin)
				));
				scToken; Endline] @
				[(TokenExp "pin_start_inject_errors"); ParenExp(ComposeExp([])); scToken; Endline] @
				cmp @
				[(TokenExp "pin_stop_inject_errors"); ParenExp(ComposeExp([])); scToken; Endline] @
				(*pack code*)
				[packOutFxn;
				ParenExp(ComposeExp(
					[taskId; cToken; rrank] @
					(List.map (function inp -> 
						match inp with 
							ScalarVarInfo(_,n,_) -> ComposeExp([cToken; (TokenExp n)]) 
							| ArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; (TokenExp n)]) 
							| ConstScalarVarInfo(_,n,_) -> ComposeExp([cToken; (TokenExp n)]) 
							| ConstArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; (TokenExp n)]) 
						) 
					vout)
				));
				scToken; Endline] 
			))
		])
	in
	let init_defn = ComposeExp([
		Endline; taskAddFxn; 
		ParenExp(
			ComposeExp(
				[taskId; cToken; taskCompFxn; cToken; taskTransFxn; cToken; 
				TokenExp(string_of_int (List.length vin)); cToken;
				TokenExp(string_of_int (List.length vout)); cToken;
				TokenExp(string_of_int (List.length tout))] @
				(List.map (function inp -> 
					match inp with 
						ScalarVarInfo(t,_,_) -> 
							ComposeExp([cToken; Endline; TokenExp("\t"); TokenExp("TASK_ARG_SCALAR"); cToken; (ltyp t); cToken; TokenExp("1")]) 
						| ArrayVarInfo(t,_,VarExpr(e),_) -> 
							ComposeExp([cToken; Endline; TokenExp("\t"); TokenExp("TASK_ARG_ARRAY"); cToken; (ltyp t); cToken; TokenExp(e)]) 
						| ConstScalarVarInfo(t,_,_) -> 
							ComposeExp([cToken; Endline; TokenExp("\t"); TokenExp("TASK_ARG_CONST_SCALAR"); cToken; (ltyp t); cToken; TokenExp("1")]) 
						| ConstArrayVarInfo(t,_,VarExpr(e),_) -> 
							ComposeExp([cToken; Endline; TokenExp("\t"); TokenExp("TASK_ARG_CONST_ARRAY"); cToken; (ltyp t); cToken; TokenExp(e)]) 
						| ArrayVarInfo(_,_,VarNilExpr,_) -> raise (Failure "every array must have a well defined size")
						| ConstArrayVarInfo(_,_,VarNilExpr,_) -> raise (Failure "every array must have a well defined size")
					) 
				(vin@vout))
			)
		);
		scToken; Endline
	])
	in
	let main_loop = ComposeExp(
		(List.map (function v -> 
			match v with 
				ScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
				| ArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([(ptr t); TokenExp(n); scToken; Endline])
				| ConstScalarVarInfo(t,n,_) -> ComposeExp([(typ t); TokenExp(n); scToken; Endline])
				| ConstArrayVarInfo(t,n,VarExpr(siz),_) -> ComposeExp([(ptr t); TokenExp(n); scToken; Endline])
				| ArrayVarInfo(_,_,VarNilExpr,_) -> raise (Failure "every array must have a well defined size")
				| ConstArrayVarInfo(_,_,VarNilExpr,_) -> raise (Failure "every array must have a well defined size")
			) (vin@vout)
		) @
		[
		    TokenExp("Topaz::topaz->start"); ParenExp(ComposeExp([taskId])); scToken; Endline;
			forToken; 
			ParenExp(ComposeExp([intToken; rrank; eqToken; rmin; scToken; rrank; lteToken; rmax; scToken; rrank; peqToken; rstep]));
			Endline;
			BraceExp(ComposeExp(
				bef @
				(*invocation*)
				[
					Endline; sendFxn; 
					ParenExp(ComposeExp([taskId; cToken; rrank] @ 
					(List.map (function inp -> 
						match inp with 
							ScalarVarInfo(_,_,VarExpr(e)) -> ComposeExp([cToken; TokenExp(e)]) 
							| ArrayVarInfo(_,_,_,VarExpr(e)) -> ComposeExp([cToken; TokenExp(e)]) 
							| ConstScalarVarInfo(_,_,VarExpr(e)) -> ComposeExp([cToken; TokenExp(e)]) 
							| ConstArrayVarInfo(_,_,_,VarExpr(e)) -> ComposeExp([cToken; TokenExp(e)]) 
							| _ -> raise (Failure "every input must have an assigned value")
						) vin)
					)); 
					scToken;
					Endline
				] @
				[
					unpackInFxn;
					ParenExp(ComposeExp(
						[taskId; cToken; refr rrank] @
						(List.map (function inp -> 
							match inp with 
								ScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
								| ArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
								| ConstScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
								| ConstArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
							) 
						vin)
					)); scToken; Endline;
					ifToken; 
					ParenExp(ComposeExp([
						receiveFxn;
						ParenExp(ComposeExp(
							[taskId; cToken; (refr rrank)] @
							(List.map (function outp -> 
								match outp with 
									ScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
									| ArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
									| ConstScalarVarInfo(_,n,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
									| ConstArrayVarInfo(_,n,_,_) -> ComposeExp([cToken; refr (TokenExp n)]) 
								) vout)
						)); 
							
					]));
					BraceExp(ComposeExp(
						red
					))
				] @
				[Endline]
			))
		]
	) 
	in
		(main_loop, [comp_defn; trans_defn], [init_defn]);;
		
		
let rec translate tree = 
	let rec _translate tree = 
		match tree with
			NilExp -> (NilExp, [],[],[])
			| Endline -> (Endline, [],[],[])
			| BraceExp(b) -> 
				let (ne, nx, ni, nf) = _translate b in
				(BraceExp(ne), nx, ni, nf)
			| ParenExp(e) -> (ParenExp(e),[],[],[])
			| TaskSetExp(name, range, blocks) -> 
				let 
					(ne, nx, ni) = translate_task name range blocks
				in
					(ne, nx, ni, [])
			| ComposeExp(lst) ->
				let rec translate_list lst= 
					match lst with 
						h::t -> 
						let (ne, nx, ni, nf) = (_translate h) in
						let (te, tx, ti, tf) = translate_list t in
							(ne::te, nx@tx, ni@ti, nf@tf)
						|[] -> ([],[],[],[])
				in
					let (ne, nx, ni, nf) = translate_list lst
					in
						(ComposeExp(ne), nx, ni, nf)
			| ErrorExp(e) ->  (ErrorExp(e),[],[],[])
			| TokenExp(e) -> (TokenExp(e),[],[],[])
	in
		let 
			(ne, nx, ni, nf) =  _translate tree 
		in
		let
			init_fxn = ComposeExp(
				[Endline; TokenExp("int"); TokenExp("topaz_init");
					ParenExp(ComposeExp([
							TokenExp("int"); TokenExp("argv"); TokenExp(","); 
							TokenExp("char**"); TokenExp("argc")
						]));
					BraceExp(ComposeExp(ni@[Endline]))
				]
			)
		in
		let 
			finalize_fxn = ComposeExp(
				[Endline; TokenExp("int"); TokenExp("topaz_finish");
					ParenExp(ComposeExp([
							TokenExp("int"); TokenExp("argv"); TokenExp(","); 
							TokenExp("char**"); TokenExp("argc")
						]));
					BraceExp(ComposeExp(nf@[Endline]))
				]
			)
		in
		let 
			worker_fxn = ComposeExp(
				[Endline; TokenExp("int"); TokenExp("topaz_worker");
					ParenExp(ComposeExp([
							TokenExp("int"); TokenExp("argv"); TokenExp(","); 
							TokenExp("char**"); TokenExp("argc")
						]));
					BraceExp(ComposeExp(nf@[Endline]))
				]
			)
		in
		let
			addnl_inc = ComposeExp([
				TokenExp("#include"); TokenExp("<topaz.h>"); Endline;
				TokenExp("#include"); TokenExp("<task.h>");  Endline;
				TokenExp("#include"); TokenExp("<checker.h>"); Endline;
			])
		in
			ComposeExp(
				[addnl_inc; ne]@nx@[init_fxn; finalize_fxn; worker_fxn]
			)
