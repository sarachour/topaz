open Printf
open Scanf
open Translator
open FileIO
open Sys

(* Open struct classes *)
open Exp

(* Open routine classes *)
open Parser
open Printf 
(* default values *)
let input_file = ref ""
let output_file = ref ""
 
let usage = "usage: " ^ Sys.argv.(0) ^ "-i input_file.tpz"
 
let speclist = [
    ("-i", Arg.Set_string input_file, ": set input file.");
    ("-o", Arg.Set_string output_file, ": set output file.");
  ]

let parseFile infile = 
	let code = read_file infile in
	(
	(printf "%s\n") ">> read source from file";
	let codestream = Lexing.from_string code in
		(
		(printf "%s\n") ">> coarsely parsed source.";
		let pcode = parse codestream in
			(
			(printf "%s\n") ">> finely parsed source from file";
			let ccode = translate pcode in
				(
					(printf "%s\n") ">> translated from topaz to c source.";
					ccode;
				)
			)
		)
	)

let generateFile outfile structure = 
	let data = print structure in
	write_file outfile data
	
(** Main module. *)
(** Program entry point. *)
let main (infile, outfile) =
   let exptree = parseFile infile in
   generateFile outfile exptree
   (**let output_string = transform input_nodes in**)
   (**write_file outfile output_string**)

let _ = 
	Arg.parse speclist (fun x -> raise (Arg.Bad ("Bad argument : " ^ x))) usage;
	let inp = !input_file in
	let outp = !output_file in
	main (inp, outp)
