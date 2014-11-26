open Data


 
let rec printExp_ exptree depth= 
	let rec makeindents num = 
		if num > 0 then "	"^(makeindents (num-1))
		else ""
	in
	match exptree with
	NilExp -> ""
	| ErrorExp(s) -> "\n\n[[[ ERROR:"^s^"]]]\n\n"
	| BraceExp(e) -> "{"^
					 (printExp_ e (depth+1))^
					 "\n"^(makeindents depth)^"}"
	| ParenExp(e) -> "("^(printExp_ e (depth))^")"
	| Endline -> "\n"
	| TokenExp(s) -> s
	| ComposeExp(elst) -> 
		let rec printlst elst = match elst with
			| Endline::t -> (printExp_ Endline depth)^(makeindents depth)^(printlst t)
			| h::t -> (printExp_ h depth)^" "^(printlst t)
			| [] -> ""
		in
			printlst elst
	| TaskSetExp(e) -> printExp_ (ErrorExp("taskset not expected")) depth
	
let printExp exptree = printExp_ exptree 0

let startContext = {
	ctxDecls=ComposeExp([TokenExp("#include \\\"topaz.h\\\""); Endline; 
						 TokenExp("topaz_info * __TOPAZ;"); Endline;
						 TokenExp("task_message __IN;"); Endline;
						 TokenExp("task_message __OUT;"); Endline;
						 TokenExp("task_data_accessor __DA;"); Endline]); 
	ctxTaskHdrs=NilExp; 
	ctxTasks=NilExp;
	ctxTaskAdds=NilExp;
	ctxChecker=NilExp;
	}


let rec findBody prefix lst = 
		let rec fExp exp = match exp with
			ParenExp(e) -> let (p, b) = fExp e in
				(p,b)
			| BraceExp(e) -> let (p, b) = fExp e in
				(p,b)
			| ComposeExp(e) -> let (p, b) = findBody prefix e in
				(p,b)
			| _ -> (NilExp, NilExp)
		in
		match lst with
		TokenExp(pfx)::ParenExp(par)::BraceExp(brace)::t
		|TokenExp(pfx)::Endline::ParenExp(par)::BraceExp(brace)::t
		|TokenExp(pfx)::ParenExp(par)::Endline::BraceExp(brace)::t
		|TokenExp(pfx)::Endline::ParenExp(par)::Endline::BraceExp(brace)::t ->
			if pfx = prefix then
				(par, brace)
			else
				findBody prefix t
		|TokenExp(pfx)::BraceExp(brace)::t
		|TokenExp(pfx)::Endline::BraceExp(brace)::t ->
			if pfx = prefix then
				(NilExp, brace)
			else
				findBody prefix t
		|TokenExp(pfx)::ParenExp(par)::t
		|TokenExp(pfx)::Endline::ParenExp(par)::t ->
			if pfx = prefix then
				(par, NilExp)
			else
				findBody prefix t
		| h::t -> 
			let (p,b) = fExp h in
				if p == NilExp && b == NilExp then
					findBody prefix t
				else
					(p,b)
		| [] -> (NilExp, NilExp)

let transformTask name range body ctx = 
	let rng = TaskSetParser.range TaskSetLexer.range (Lexing.from_string (printExp range)) in
	let (tin, tout, tbody) = 
		let getvarlist exp = TaskSetParser.varlist TaskSetLexer.varlist (Lexing.from_string (printExp exp)) in
		let (_, tsbody)  = findBody "task" [body] in
		let (tparin, inbody)    = findBody "in" [body] in
		let (tparout, outbody)   = findBody "out" [body] in
		let tvarin = (if tparin == NilExp then [] else getvarlist tparin) in
		let tvarout = (if tparout == NilExp then [] else getvarlist tparout) in
		let tbody = if tsbody != NilExp then tsbody else
			if inbody != NilExp then inbody else
			if outbody != NilExp then outbody else NilExp
		in
			(tvarin, tvarout, tbody)
	in
	let rec genAdd lst =
		let isLast t = match t with
			h::t -> false
			| _ -> true
		in
		let typeToConst t = match t with
			"float" -> "TASK_ARG_TYPE_FLOAT"
			| "int" -> "TASK_ARG_TYPE_INT"
			| "double" -> "TASK_ARG_TYPE_DOUBLE"
			| "char" -> "TASK_ARG_TYPE_CHAR"
			| _		 -> "???"
		in 
		match lst with
		v::t -> 
			let rest = (genAdd t) in
				TokenExp(typeToConst v.varType)::
				TokenExp(",")::TokenExp(v.varSize)::
				(if isLast t then NilExp else TokenExp(","))::rest
		| [] -> []
	in
	let rec genPack isPackValue lst = 
		match lst with
			v::t -> 
				let rest = (genPack isPackValue t) in
				if v.varSize = "1" then
					if isPackValue then
						TokenExp("task_pack_"^v.varType)::ParenExp(ComposeExp([
							TokenExp("&__DA"); TokenExp(","); TokenExp(v.varValue)
						]))::TokenExp(";")::Endline::rest
					else
						TokenExp("task_pack_"^v.varType)::ParenExp(ComposeExp([
							TokenExp("&__DA"); TokenExp(","); TokenExp(v.varName)
						]))::TokenExp(";")::Endline::rest
				else
					if isPackValue then
						TokenExp("task_pack_"^v.varType^"_array")::ParenExp(ComposeExp([
							TokenExp("&__DA"); TokenExp(","); TokenExp(v.varValue); TokenExp(","); TokenExp(v.varSize)
						]))::TokenExp(";")::Endline::rest
					else
						TokenExp("task_pack_"^v.varType^"_array")::ParenExp(ComposeExp([
							TokenExp("&__DA"); TokenExp(","); TokenExp(v.varName); TokenExp(","); TokenExp(v.varSize)
						]))::TokenExp(";")::Endline::rest
				
		| [] -> []
	in
	let rec genUnpack isFullUnpack lst = 
		match lst with
			v::t -> 
				let rest = genUnpack isFullUnpack t in
				if v.varSize = "1" then
						TokenExp(v.varName)::TokenExp("= task_unpack_"^v.varType)::ParenExp(ComposeExp([
							TokenExp("&__DA");
						]))::TokenExp(";")::Endline::rest
				else
					if isFullUnpack then
							TokenExp("task_unpack_"^v.varType^"_array")::ParenExp(ComposeExp([
								TokenExp("&__DA"); TokenExp(","); TokenExp(v.varName);
							]))::TokenExp(";")::Endline::rest
					else
							TokenExp("task_quick_unpack_"^v.varType^"_array")::ParenExp(ComposeExp([
								TokenExp("&__DA"); TokenExp(",&"); TokenExp(v.varName);
							]))::TokenExp(";")::Endline::rest
			|[]->[]
	in
	let rec genDecl isDeep lst = 
	match lst with
		v::t -> 
			let rest = (genDecl isDeep t) in
			if v.varSize = "1" then
					TokenExp(v.varType)::TokenExp(v.varName)::TokenExp(";")::Endline::rest
			else
				if isDeep then
					TokenExp(v.varType)::TokenExp(v.varName)::
					TokenExp("[")::TokenExp(v.varSize)::TokenExp("]")::
					TokenExp(";")::Endline::rest
				else
					TokenExp(v.varType)::TokenExp("*")::TokenExp(v.varName)::TokenExp(";")::Endline::rest
		| [] -> []
	in
	let rec findAssert b = match b with
		TokenExp("assert")::ParenExp(exp)::t -> 
			let rest = (findAssert t) in
				ParenExp(exp)::(if rest = [NilExp] then NilExp else TokenExp("&&"))::rest
		| ComposeExp(fas)::t -> findAssert (fas@t)
		| h::t -> (findAssert t)
		| [] -> [NilExp]
	in
	let rec removeAsserts b = match b with
		TokenExp("assert")::ParenExp(exp)::TokenExp(";")::t -> t
		| ComposeExp(fas)::t -> ComposeExp(removeAsserts fas)::(removeAsserts t)
		| h::t -> h::(removeAsserts t)
		| [] -> [NilExp]
	in
	let taskGlblName = "TASK_"^name^"_ID" in
	let taskFxnName = "TASK_"^name^"_FXN" in
	let loopdecls = 
		[TokenExp("int");TokenExp(rng.rangeRank);TokenExp(";");Endline;
				TokenExp("int"); TokenExp("__ACCEPT"); TokenExp(";"); Endline;
				TokenExp("int"); TokenExp("__REJECT"); TokenExp(";"); Endline;
				TokenExp("int"); TokenExp("__AUTO"); TokenExp(";"); Endline;
				TokenExp("double"); TokenExp("__PROB"); TokenExp(";"); Endline] @
		(genDecl true  tin)@
		(genDecl false tout)
	in
	let rec getinfo name outs idx = match outs with
		h::t -> if h.varName = name then
			(h.varType, string_of_int idx, h.varSize)
		else
			getinfo name t (idx+1)
		|[] -> ("???","???", "???")
	in
	let rec autoAssert assertargs =
		let islast lst = match lst with h::t -> false | _ -> true in
		match assertargs with
			ComposeExp(e)::t -> let (a1,a2,a3, a4, a5) = autoAssert e and (b1,b2,b3,b4, b5) = autoAssert t in
				(a1@b1, a2@b2, a3@b3, a4@b4, a5+b5)
			|ParenExp(e)::t -> let (a1,a2,a3, a4, a5) = autoAssert [e] and (b1,b2,b3,b4, b5) = autoAssert t in
				(a1@b1, a2@b2, a3@b3, a4@b4, a5+b5)
			|TokenExp(e)::t -> 
				let (b1,b2,b3,b4,b5) = autoAssert t in
				let outs = Str.split (Str.regexp ",") e in
				let rec construct_calls lst =
					match lst with
					h::t -> 
						let (pe, pi, pt, pu, pcnt) = construct_calls t in
						let (vtype, vidx, vsize) = getinfo h tout 0 in
						(
							TokenExp("CHECK_"^h)::TokenExp(if islast pe then "" else "&&")::pe@TokenExp(if islast b1 then "" else "&&")::[],
							TokenExp("topaz_cls_init")::ParenExp(ComposeExp([TokenExp("__TOPAZ"); TokenExp(","); TokenExp(taskGlblName); TokenExp(","); TokenExp(vidx); TokenExp(","); TokenExp("\\\"out."^h^"\\\"") ]))::TokenExp(";")::Endline::
							TokenExp("topaz_log_output(__TOPAZ, "^taskGlblName^","^vidx^",\\\""^h^"\\\");")::Endline::pi,
							TokenExp("int CHECK_"^h^" = ")::
							TokenExp("topaz_cls_predict_"^vtype)::ParenExp(ComposeExp([
								TokenExp("__TOPAZ"); TokenExp(","); TokenExp(taskGlblName); TokenExp(","); TokenExp(vidx); TokenExp(","); TokenExp("__place"); TokenExp(",");
								if vsize = "1" then TokenExp("&"^h) else TokenExp(h);
								]))::TokenExp(";")::Endline::pt,
							TokenExp("topaz_cls_update_"^vtype)::ParenExp(ComposeExp([
								TokenExp("__TOPAZ"); TokenExp(","); TokenExp(taskGlblName); TokenExp(","); TokenExp(vidx); TokenExp(","); TokenExp("__place"); TokenExp(",");
								if vsize = "1" then TokenExp("&"^h) else TokenExp(h);
								]))::TokenExp(";")::Endline::pu,
							pcnt+1
						)
					| [] -> ([],[],[],[], 0)
				in
				let (a1,a2,a3,a4, a5) = construct_calls outs in
					(a1@b1, a2@b2, a3@b3, a4@b4, a5+b5)				
			| h::t -> autoAssert t
			| [] -> ([], [], [], [], 0)
	in
	let (_, bbody)  = findBody "before" [body] in
	let (_, rbody) = findBody "reduce" [body] in
	let (_, accbody) = findBody "accept" [body] in
	let (_, rejbody) = findBody "reject" [body] in
	let (auaccargs, _) = findBody "auto" [body] in
	let issingle = let (a,b) = findBody "single" [body] in 
		if a = NilExp && b = NilExp then false else true
	in
	let (auaccexp, auaccinit, auaccbody, auaccupdate, auacccnt) = autoAssert [auaccargs] in
	let accassert = findAssert [accbody] and rejassert = (findAssert [rejbody]) in
	let fxnheader	= [TokenExp("void"); TokenExp(taskFxnName); 
		ParenExp(TokenExp("topaz_info * __topaz, task_message * __in, task_message * __out"))];
	in	
	let fxn = fxnheader @ [Endline; BraceExp(ComposeExp((
				[Endline;
				TokenExp("int"); TokenExp(rng.rangeRank); TokenExp(";"); Endline]@
				(genDecl false tin)@
				(genDecl true tout)@
				[
					TokenExp("task_unpack_task(__in, &__DA);"); Endline;
					TokenExp(rng.rangeRank); TokenExp("= __in->task_rank;"); Endline;
				]@
				(genUnpack false tin)@
				[Endline;tbody;Endline;]@
				[
					TokenExp("task_pack_task(__out, &__DA, __in->task_id, __in->task_rank);"); Endline;
				]@
				(genPack false tout)
			))); Endline]
	in
	let taskadd = [TokenExp(taskGlblName);TokenExp("= topaz_add_task(topaz,");
		TokenExp(taskFxnName); TokenExp(",");
		TokenExp(string_of_int (List.length tin));TokenExp(",");
		TokenExp(string_of_int (List.length tout)); TokenExp(",")] @
		(genAdd tin) @ [TokenExp(",")] @
		(genAdd tout) @ 
		[TokenExp(");");Endline];
	in
	let loop1body = BraceExp(ComposeExp([Endline;
						bbody; Endline;
						TokenExp("task_pack_task"); ParenExp(ComposeExp([
							TokenExp("&__IN"); TokenExp(","); TokenExp("&__DA"); TokenExp(",");
							TokenExp(taskGlblName); TokenExp(","); TokenExp(rng.rangeRank)
						])); TokenExp(";"); Endline
					] @
					(genPack true tin) @
					[TokenExp("topaz_send_task_to_any(__TOPAZ, &__IN);"); Endline]; 
					))
	in
	let loop2body = BraceExp(ComposeExp([Endline] @
					[TokenExp("int __place = "); TokenExp("topaz_receive_task_from_any(__TOPAZ, &__OUT);"); Endline;
					TokenExp("task_unpack_task"); ParenExp(TokenExp("&__OUT, &__DA")); TokenExp(";"); Endline;
					TokenExp(rng.rangeRank); TokenExp("= __OUT.task_rank;"); Endline;]@
					(genUnpack false tout) @
					[Endline; BraceExp(ComposeExp((removeAsserts [accbody])@
						[Endline; TokenExp("__ACCEPT = "); 
						(if accassert = [NilExp] then TokenExp("true") else ComposeExp(accassert)); TokenExp(";"); 
						Endline])); Endline] @
					[Endline; BraceExp(ComposeExp((removeAsserts [rejbody])@ 
						[Endline; TokenExp("__REJECT = "); 
						(if rejassert = [NilExp] then TokenExp("false") else ComposeExp(rejassert)); TokenExp(";"); 
						Endline])); Endline] @			
					[Endline; BraceExp(ComposeExp((Endline::auaccbody)@
						[Endline; TokenExp("__AUTO = "); 
						(if auaccargs = NilExp then TokenExp("true") else ComposeExp(auaccexp)); TokenExp(";"); Endline;
						TokenExp("if(topaz_cls_decide(__TOPAZ, "^taskGlblName^", __AUTO) && !__AUTO ){"); Endline; 
									TokenExp("topaz_reexecute(__TOPAZ, "^taskGlblName^", &__IN, &__OUT);"); Endline;
									TokenExp("task_unpack_task"); ParenExp(TokenExp("&__OUT, &__DA")); TokenExp(";"); Endline] @
						(genUnpack false tout) @
						auaccupdate @
						[TokenExp("__AUTO = true;"); Endline; TokenExp("}"); Endline]
						)); Endline] @
					[
						TokenExp("if"); ParenExp(TokenExp("__ACCEPT && !__REJECT && __AUTO")); Endline; BraceExp(rbody); Endline
					]))
	in
	let loop1 = [TokenExp("for");
					ParenExp(ComposeExp([
							TokenExp(rng.rangeRank^"="^rng.rangeMin); TokenExp(";");
							TokenExp(rng.rangeRank^"<"^rng.rangeMax); TokenExp(";");
							TokenExp(rng.rangeRank^"+="^rng.rangeStep)
							])); 
							(if issingle then BraceExp(ComposeExp([loop1body;Endline; Endline;loop2body])) else loop1body)]
	in
	let loop2 = [TokenExp("while");
					ParenExp(TokenExp("topaz_n_tasks_left(__TOPAZ) > 0"));
					loop2body]
	in
		(ComposeExp(loopdecls@loop1@[Endline; Endline]@if issingle then [] else loop2), 
			(ctxAddChecker (auaccinit)
			(ctxAddChecker (if auacccnt > 0 then [TokenExp("topaz_log_task(__TOPAZ, "^taskGlblName^", \\\"logs/"^name^"\\\");"); Endline] else [])
			(ctxAddTaskAdd (taskadd)
			(ctxAddTask   (fxn)
			(ctxAddHeader (fxnheader @ [TokenExp(";"); Endline]) 
			(ctxAddDecl [TokenExp("int");TokenExp(taskGlblName);TokenExp(";"); Endline] ctx
			))))))
		)


			
let rec transformExpList lst ctx = 
		let rec tExp exp ctx = match exp with
			ParenExp(e) -> let (ne, nctx) = tExp e ctx in
				(ParenExp(ne), nctx)
			| BraceExp(e) -> let (ne, nctx) = tExp e ctx in
				(BraceExp(ne),nctx)
			| ComposeExp(e) -> let (ne, nctx) = transformExpList e ctx in
				(ComposeExp(ne), nctx)
			| _ -> (exp, ctx)
		in
		match lst with
		TokenExp("taskset")::TokenExp(name)::ParenExp(range)::BraceExp(body)::t 
		|TokenExp("taskset")::TokenExp(name)::Endline::ParenExp(range)::BraceExp(body)::t 
		|TokenExp("taskset")::TokenExp(name)::ParenExp(range)::Endline::BraceExp(body)::t 
		|TokenExp("taskset")::TokenExp(name)::Endline::ParenExp(range)::Endline::BraceExp(body)::t ->
			let (te, tctx) = (transformTask name range body ctx) in
			let (le, lctx) = (transformExpList t tctx) in
				(te::le, lctx)
		| h::t -> 
			let (he, tctx) = (tExp h ctx) in
			let (le, lctx) = (transformExpList t tctx) in
				(he::le, lctx)
		| [] -> ([], ctx)

	
let rec transformExp exptree = 
	let addContext exp ctx = 
		let tdecls = match ctx.ctxDecls with ComposeExp(e) -> e | a -> [a] in
		let thdrs = match ctx.ctxTaskHdrs with ComposeExp(e) -> e | a -> [a] in
		let ttasks = match ctx.ctxTasks with  ComposeExp(e) -> e | a -> [a] in
		let tworker = [
			Endline; Endline;
			TokenExp("int topaz_worker"); 
			ParenExp(TokenExp("topaz_info * topaz, int argv, char ** argc")); 
			Endline;
			BraceExp(ComposeExp([ Endline;
				TokenExp("__TOPAZ = topaz;"); Endline;
				ctx.ctxTaskAdds; Endline
			]))
		] in
		let tinitmain = [
			Endline; Endline;
			TokenExp("void topaz_main_init"); 
			ParenExp(TokenExp("topaz_info * topaz")); 
			Endline;
			BraceExp(ComposeExp([ Endline;
				TokenExp("__TOPAZ = topaz;"); Endline;
				ctx.ctxTaskAdds; Endline;
				ctx.ctxChecker; Endline;
				TokenExp("task_init_task");
				ParenExp(TokenExp("&__IN, topaz->tasks.max_input_buffer")); 
				TokenExp(";"); Endline;
				TokenExp("task_init_task");
				ParenExp(TokenExp("&__OUT, topaz->tasks.max_output_buffer")); 
				TokenExp(";"); Endline
			]))
		] in
		let tfinishmain = [
			Endline; Endline;
			TokenExp("void topaz_main_finish"); 
			ParenExp(TokenExp("topaz_info * topaz")); 
			Endline;
			BraceExp(ComposeExp([ Endline;
				TokenExp("task_destroy_task");
				ParenExp(TokenExp("&__IN")); 
				TokenExp(";"); Endline;
				TokenExp("task_destroy_task");
				ParenExp(TokenExp("&__OUT")); 
				TokenExp(";"); Endline;
			]))
		] in
		tdecls @ thdrs @ exp @ ttasks @ tinitmain @ tfinishmain @ tworker
	in
	let (e,nctx) = transformExpList [exptree] (startContext) in
		ComposeExp(addContext e nctx)

let parseExp code = 
	let codestream = Lexing.from_string code in
		CodeParser.main CodeLexer.main codestream
