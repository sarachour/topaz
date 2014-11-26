
type var = {varType:string; varName:string; varSize:string; varValue: string}
type range = {rangeRank:string; rangeMin:string; rangeMax:string; rangeStep:string}
type taskchunk = {chunkInputs : var list; chunkOutputs: var list; chunkBody : string}
type taskset = {taskName:string; taskRange:range; taskBefore:string; taskChunk: taskchunk; taskReduce:string}
type node = TaskSetNode of taskset | TextNode of string

type exp = TaskSetExp of taskset | ErrorExp of string | NilExp | ParenExp of exp | BraceExp of exp | ComposeExp of exp list | TokenExp of string | Endline
type context = {ctxChecker : exp; ctxDecls : exp ; ctxTaskHdrs : exp; ctxTasks : exp; ctxTaskAdds: exp}

let ctxAddChecker hdrs ctx = 
	let newhdrs = match ctx.ctxChecker with
		ComposeExp(e) -> ComposeExp(e @ hdrs)
		| a -> ComposeExp(a::hdrs)
	in
		{ctxDecls=ctx.ctxDecls; 
		 ctxTaskHdrs=ctx.ctxTaskHdrs; 
		 ctxTasks = ctx.ctxTasks; 
		 ctxTaskAdds = ctx.ctxTaskAdds;
		 ctxChecker = newhdrs}


let ctxAddDecl decls ctx = 
	let newdecls = match ctx.ctxDecls with
		ComposeExp(e) -> ComposeExp(e @ decls)
		| a -> ComposeExp(a::decls)
	in
		{ctxDecls=newdecls; 
		 ctxTaskHdrs=ctx.ctxTaskHdrs; 
		 ctxTasks = ctx.ctxTasks; 
		 ctxTaskAdds = ctx.ctxTaskAdds;
		 ctxChecker = ctx.ctxChecker}

let ctxAddHeader hdrs ctx = 
	let newhdrs = match ctx.ctxTaskHdrs with
		ComposeExp(e) -> ComposeExp(e @ hdrs)
		| a -> ComposeExp(a::hdrs)
	in
		{ctxDecls=ctx.ctxDecls; 
		 ctxTaskHdrs=newhdrs; 
		 ctxTasks = ctx.ctxTasks; 
		 ctxTaskAdds = ctx.ctxTaskAdds;
		 ctxChecker = ctx.ctxChecker}

let ctxAddTask tsks ctx = 
	let newhdrs = match ctx.ctxTasks with
		ComposeExp(e) -> ComposeExp(e @ tsks)
		| a -> ComposeExp(a::tsks)
	in
		{ctxDecls=ctx.ctxDecls; 
		 ctxTaskHdrs=ctx.ctxTaskHdrs; 
		 ctxTasks = newhdrs; 
		 ctxTaskAdds = ctx.ctxTaskAdds;
		 ctxChecker = ctx.ctxChecker}

let ctxAddTaskAdd tsks ctx = 
	let newhdrs = match ctx.ctxTaskAdds with
		ComposeExp(e) -> ComposeExp(e @ tsks)
		| a -> ComposeExp(a::tsks)
	in
		{ctxDecls=ctx.ctxDecls; 
		 ctxTaskHdrs=ctx.ctxTaskHdrs; 
		 ctxTasks = ctx.ctxTasks; 
		 ctxTaskAdds = newhdrs;
		 ctxChecker = ctx.ctxChecker}


type translatectx = {global_decls: string; task_adds: string; fxn_headers: string; fxn_bodies: string}
