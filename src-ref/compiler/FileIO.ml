open Printf
open Scanf

let unescape s =
  sscanf ("\"" ^ s ^ "\"") "%S" (fun u -> u)
  
let rec read_lines chnl str = 
	try
		let line = input_line chnl in
			read_lines chnl (str ^ "\n"^ line)
	with End_of_file ->
		close_in chnl; str

let read_file name = 
	let in_channel = open_in name in
	try
		let line = input_line in_channel in
			read_lines in_channel (line)
	with End_of_file ->
		close_in in_channel; ""
  
 let write_file name data = 
	let out_channel= open_out name in 
		output_string out_channel (unescape data);
		close_out out_channel
