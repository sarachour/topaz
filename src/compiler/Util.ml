open List

let get_back arr = 
	List.nth arr ((List.length arr) -1) 

let pop_back arr =
	List.rev (List.tl (List.rev arr))
	
