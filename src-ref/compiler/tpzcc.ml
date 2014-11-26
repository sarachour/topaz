open Printf
open Scanf
open Translator
open FileIO
open Sys

(* default values *)
let input_file = ref ""
let output_file = ref ""
 
let usage = "usage: " ^ Sys.argv.(0) ^ "-i input_file.tpz"
 
let speclist = [
    ("-i", Arg.Set_string input_file, ": set input file.");
    ("-o", Arg.Set_string output_file, ": set output file.");
  ]

(** Main module. *)
(** Program entry point. *)
let main (infile, outfile) =
   let input_string = read_file infile in
   let exptree = parseExp input_string in
   let nexptree = transformExp exptree in
   write_file outfile (printExp nexptree)
   (**let output_string = transform input_nodes in**)
   (**write_file outfile output_string**)

let _ = 
	Arg.parse speclist (fun x -> raise (Arg.Bad ("Bad argument : " ^ x))) usage;
	let inp = !input_file in
	let outp = !output_file in
	main (inp, outp)
