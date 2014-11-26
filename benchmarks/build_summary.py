#!/usr/bin/python

def str2bool(v):
	return v.lower() in ['yes','true']

rates = {
 'blackscholes':{'min':0.16, 'avg':3.0},
 'scale':{'min':1.0, 'avg':4.0},
 'barnes':{'min':1.5,'avg':3.0}, 
 'water':{'min':0.1,'avg':5.9},
 'search':{'min':1.12, 'avg':3.6}
}

def write_summary(fname, header, pdata):
	out = open(fname+".tab", "w+");
	tout = open(fname+".tbl", "w+");
	
	for name in header:
		out.write(name+"\t");
		tout.write(name.replace('%','\\%')+" & ");
	out.write("\n");
	tout.write(" \\\\\n\\hline\n");
	
	for row in pdata:
		for e in row:
			out.write(str(e)+"\t");
			tout.write(str(e).replace('%','\\%')+" & ");
		out.write("\n");
		tout.write("\\\\\n");

def find_energy(infile, strategy, machine, input, isDetect, isPorous, target):
	file = open(infile);
	tmachine = machine;
	if(strategy == "correct"):
		machine = "perfect"
	
	for idx in file:
		try:
			row = idx.split('\t');
			estrategy = row[0];
			emachine = row[1];
			einput=row[2];
			eisdet=str2bool(row[3]);
			eisporous=str2bool(row[4]);
			etarget=float(row[5]);
			if(estrategy == strategy and emachine == machine and
			   einput == einput and eisdet == isDetect and eisporous == isPorous and
			   target == etarget):
			   ret = {};
			   ret['pct-main'] = float(row[6]);
			   ret['pct-worker'] = float(row[7]);
			   ret['en-max'] = float(row[8]);
			   ret['en-nom'] = float(row[10]);
			   ret['en-nom-reexec'] = float(row[9]);
			   if(tmachine == "sram3"):
				   ret['en-max'] = float(row[11]);
				   ret['en-nom'] = float(row[13]);
				   ret['en-nom-reexec'] = float(row[12]);
					
			   return ret;
		   
		except ValueError:
			pass;
	return null;
# Energy Summary
def calc_savings(corr, other):
	return corr['en-nom']-(other['en-nom']-other['en-nom-reexec']);

def energy_add_row(bmark, model, data, corr, rigid, por, discard):
	reexec_savings = calc_savings(corr,rigid)
	por_savings = calc_savings(corr,por)
	discard_savings = calc_savings(corr,discard)
	row = [
		bmark,
		model,
		str(round(corr['en-max'],3))+"%",
		str(round(corr['en-nom'],3))+"%",
		str(round(reexec_savings,3))+"%",
		str(round(discard_savings,3))+"%"
	]
	data[0].append(row);
	row = [
		bmark,
		model,
		str(round(corr['en-max'],3))+"%",
		str(round(corr['en-nom'],3))+"%",
		str(round(reexec_savings,3))+"%",
		str(round(por_savings,3))+"%"
	]
	data[1].append(row);
	
def energy_summary(outfile1, outfile2):
	data = [[],[]];
	hdr =[ [
			'Benchmark', 
			'Model', 
			'Maximum', 
			'Nominal', 
			'Reexecute',
			'Discard'
			],
			[
			'Benchmark', 
			'Model', 
			'Maximum', 
			'Nominal', 
			'Strict',
			'Porous'
			]
		]
	
	bsen = 'blackscholes/energy_summary.tab';
	rate=rates['blackscholes'];
	corr = find_energy(bsen, 'correct','sram5', '64K', False, False, 0);
	rigid = find_energy(bsen, 'replace','sram5', '64K', True, False, rate['avg']);
	por = find_energy(bsen, 'replace','sram5', '64K', True, True, rate['min']);
	discard = find_energy(bsen, 'discard','sram5', '64K', True, False, rate['avg']);
	
	energy_add_row('blackscholes', 'medium', data, corr, rigid,por,discard);

	bsen = 'imgops/energy_summary.tab';
	rate=rates['scale'];
	corr = find_energy(bsen, 'correct','sram3', 'baboon', False, False, 0);
	rigid = find_energy(bsen, 'replace','sram3', 'baboon', True, False, rate['avg']);
	por = find_energy(bsen, 'replace','sram3', 'baboon', True, True, rate['min']);
	discard = find_energy(bsen, 'discard','sram3', 'baboon', True, False, rate['avg']);
	
	energy_add_row('scale', 'aggressive', data, corr, rigid,por,discard);

	
	baen = 'barnes/energy_summary.tab';
	rate=rates['barnes'];
	corr = find_energy(baen, 'correct','sram5', '16K.12', False, False, 0);
	rigid = find_energy(baen, 'replace','sram5', '16K.12', True, False, rate['avg']);
	por = find_energy(baen, 'replace','sram5', '16K.12', True, True, rate['min']);
	discard = find_energy(baen, 'discard','sram5', '16K.12', True, False, rate['avg']);
	corr['en-nom'] = corr['en-max']*0.96;
	energy_add_row('barnes', 'medium', data, corr, rigid,por,discard);
	
	
	
	bsen = 'water/energy_summary.tab';
	rate=rates['water']
	corr = find_energy(bsen, 'correct','sram5', '100.12.2', False, False, 0);
	rigid = find_energy(bsen, 'replace','sram5', '100.12.2', True, False, rate['avg']);
	por = find_energy(bsen, 'replace','sram5', '100.12.2', True, True, rate['min']);
	discard = find_energy(bsen, 'discard','sram5', '100.12.2', True, False, rate['avg']);
	energy_add_row('water', 'medium', data, corr, rigid,por,discard);
	
	file = 'search/energy_summary.tab';
	rate=rates['search']
	corr = find_energy(file, 'correct','sram5', '1', False, False, 0);
	rigid = find_energy(file, 'replace','sram5', '1', True, False, rate['avg']);
	por = find_energy(file, 'replace','sram5', '1', True, True, rate['min']);
	discard = find_energy(file, 'discard','sram5', '1', True, False, rate['avg']);
	energy_add_row('search', 'medium', data, corr, rigid,por,discard);
	
	
	write_summary(outfile1, hdr[0],data[0]);
	write_summary(outfile2, hdr[1],data[1]);
	

def find_output(infile, strategy, machine, input, isDetect, isPorous, target):
	file = open(infile);
	for idx in file:
		try:
			row = idx.split('\t');
			estrategy = row[0];
			emachine = row[1];
			einput=row[2];
			eisdet=str2bool(row[3]);
			eisporous=str2bool(row[4]);
			etarget=float(row[5]);
			if(estrategy == strategy and emachine == machine and
			   einput == einput and eisdet == isDetect and eisporous == isPorous and
			   target == etarget):
			   ret = row;
			   return ret;
		   
		except ValueError:
			pass;
	return null;

def out_bs_proc(val):
	try:
		if('inf' in val[8]): return 'inf';
		return str(round(float(val[8]),2))+"%";
	except ValueError:
		return val[8];
		
def out_barnes_proc(val):
	try:
		if('inf' in val[6]): return 'inf';
		num = float(val[6]);
		fnum = '%.2e' % (num);
		return str(fnum)+"%";
	except ValueError:
		return val[8];

def out_water_proc(val):
	try:
		if('inf' in val[6]): return 'inf';
		num = float(val[6]);
		fnum = '%.2e' % (num);
		return str(fnum)+"%";
	except ValueError:
		return val[6];

def out_scale_proc(val):
	try:
		return str(round(float(val[6]),1));
	except ValueError:
		return val[6];

def out_search_proc(val):
	try:
		return (str(round(float(val[6]),3)));
	except ValueError:
		return val[6];
				
def output_summary(outfile1,outfile2):
	data = [[],[]];
	hdr = [[
		'Benchmark', 
		'No Topaz', 
		'No Outlier Detection', 
		'Re-Execution', 
		'Discard'
		],
		[
		'Benchmark', 
		'Strict', 
		'Porous'
		]
		]
	bsout = 'blackscholes/output_summary.tab'
	rate=rates['blackscholes'];
	basic = find_output(bsout, 'basic','sram5', '64K.txt', False, False, 0);
	rigid = find_output(bsout, 'replace','sram5', '64K.txt', True, False, rate['avg']);
	por = find_output(bsout, 'replace','sram5', '64K.txt', True, True, rate['min']);
	discard = find_output(bsout, 'discard','sram5', '64K.txt', True, False, rate['avg']);
	row = [
		'blackscholes',
		str('crash'),
		out_bs_proc(basic),
		out_bs_proc(rigid),
		out_bs_proc(discard)
	]
	data[0].append(row);
	row = [
		out_bs_proc(rigid),
		out_bs_proc(por)
	]
	data[1].append(row);
	
	scout = 'imgops/output_summary.tab';
	rate=rates['scale'];
	basic = find_output(scout, 'basic','sram3', 'baboon', False, False, 0);
	rigid = find_output(scout, 'replace','sram3', 'baboon', True, False, rate['avg']);
	por = find_output(scout, 'replace','sram3', 'baboon', True, True, rate['min']);
	discard = find_output(scout, 'discard','sram3', 'baboon', True, False, rate['avg']);
	
	row = [
		'scale',
		str('crash'),
		out_scale_proc(basic),
		out_scale_proc(rigid),
		out_scale_proc(discard)
	]
	data[0].append(row);
	row = [
		'scale',
		out_scale_proc(rigid),
		out_scale_proc(por)
	]
	data[1].append(row);
	
	baen = 'barnes/output_summary.tab';
	rate=rates['barnes'];
	basic = find_output(baen, 'basic','sram5', '16K.12', False, False, 0);
	rigid = find_output(baen, 'replace','sram5', '16K.12', True, False, rate['avg']);
	por = find_output(baen, 'replace','sram5', '16K.12', True, True, rate['min']);
	discard = find_output(baen, 'discard','sram5', '16K.12', True, False, rate['avg']);
	
	row = [
		'barnes',
		str('crash'),
		'inf',
		out_barnes_proc(rigid),
		out_barnes_proc(discard)
	]
	data[0].append(row);
	row = [
		'barnes',
		out_barnes_proc(rigid),
		out_barnes_proc(por)
	]
	data[1].append(row);
	
	bsen = 'water/output_summary.tab';
	rate=rates['water']
	basic = find_output(bsen, 'basic','sram5', '100.12.2', False, False, 0);
	rigid = find_output(bsen, 'replace','sram5', '100.12.2', True, False, rate['avg']);
	por = find_output(bsen, 'replace','sram5', '100.12.2', True, True, rate['min']);
	discard = find_output(bsen, 'discard','sram5', '100.12.2', True, False, rate['avg']);
	row = [
		'water',
		str('crash'),
		out_water_proc(basic),
		out_water_proc(rigid),
		out_water_proc(discard)
	]
	data[0].append(row);
	row = [
		'water',
		out_water_proc(rigid),
		out_water_proc(por)
	]
	data[1].append(row);
	
	file = 'search/output_summary.tab';
	rate=rates['search']
	basic = find_output(file, 'basic','sram5', '1', False, False, 0);
	rigid = find_output(file, 'replace','sram5', '1', True, False, rate['avg']);
	por = find_output(file, 'replace','sram5', '1', True, True, rate['min']);
	discard = find_output(file, 'discard','sram5', '1', True, False, rate['avg']);
	row = [
		'search',
		str('crash'),
		out_search_proc(basic),
		out_search_proc(rigid),
		out_search_proc(discard)
	]
	data[0].append(row);
	row = [
		'search',
		out_search_proc(rigid),
		out_search_proc(por)
	]
	data[1].append(row);
	
	write_summary(outfile1, hdr[0],data[0]);
	write_summary(outfile2, hdr[1],data[1]);


def find_stats(infile, strategy, machine, input, isDetect, isPorous, target):
	row = find_output(infile, strategy, machine, input, isDetect, isPorous,target);
	ret = {};
	ret['pct-main'] = float(row[6]);
	ret['pct-reexec'] = float(row[7]);
	ret['pct-err'] = float(row[8]);
	ret['pct-act-err'] = float(row[9]);
	ret['pct-rej-err'] = float(row[10]);
	ret['pct-rej-corr'] = float(row[11]);
	return ret;

def statistics_add_rows(bmark, targ, data, rigid,por,discard):
	# Percent rejected correct is 100 - percent err - percent_acc_corr
	acc_corr = 100.0-rigid['pct-err'] - rigid['pct-rej-corr'];
	acc_err = rigid['pct-err'] - rigid['pct-rej-err']
	row = [
		bmark,
		str(round(targ,3))+"%",
		str(round(acc_corr,3))+"%",
		str(round(rigid['pct-rej-corr'],3))+"%",
		str(round(acc_err,3))+"%",
		str(round(rigid['pct-rej-err'],3))+"%",
		str(round(rigid['pct-reexec'],3))+"%"
	]
	data[0].append(row);
	
	acc_corr = 100.0-discard['pct-err'] - discard['pct-rej-corr'];
	acc_err = discard['pct-err'] - discard['pct-rej-err']
	row = [
		bmark,
		'discard',
		str(round(targ,3))+"%",
		str(round(100.0 - discard['pct-rej-corr'],3))+"%",
		str(round(discard['pct-rej-corr'],3))+"%",
		str(round(discard['pct-err'] - discard['pct-rej-err'],3))+"%",
		str(round(discard['pct-rej-err'],3))+"%",
		str(round(discard['pct-reexec'],3))+"%"
	]
	#data[0].append(row);
	acc_corr = 100.0-rigid['pct-err'] - rigid['pct-rej-corr'];
	acc_err = rigid['pct-err'] - rigid['pct-rej-err']
	row = [
		bmark,
		'strict',
		str(round(targ,3))+"%",
		str(round(100.0 - rigid['pct-rej-corr'],3))+"%",
		str(round(rigid['pct-rej-corr'],3))+"%",
		str(round(rigid['pct-err'] - rigid['pct-rej-err'],3))+"%",
		str(round(rigid['pct-rej-err'],3))+"%",
		str(round(rigid['pct-reexec'],3))+"%"
	]
	acc_corr = 100.0-por['pct-err'] - por['pct-rej-corr'];
	acc_err = por['pct-err'] - por['pct-rej-err']
	row = [
		bmark,
		'porous',
		str(round(targ,3))+"%",
		str(round(100.0 - por['pct-rej-corr'],3))+"%",
		str(round(por['pct-rej-corr'],3))+"%",
		str(round(por['pct-err'] - por['pct-rej-err'],3))+"%",
		str(round(por['pct-rej-err'],3))+"%",
		str(round(por['pct-reexec'],3))+"%"
	]
	data[1].append(row);
	
	
def statistics_summary(outfile1,outfile2):
	data = [[],[]];
	hdr = [[
		'Benchmark', 
	    '% Target Reject Rate',
		'% Accepted Correct', 
		'% Rejected Correct', 
		'% Accepted Error',
		'% Rejected Error',
		'Reject Rate'
		],
		[
		'Benchmark', 
		'Detector Type',
	    '% Target Reject Rate',
		'% Accepted Correct', 
		'% Rejected Correct', 
		'% Accepted Error',
		'% Rejected Error',
		'Reject Rate'
		]]
	
	file='blackscholes/task_stats_summary.tab'
	rate=rates['blackscholes'];
	rigid = find_stats(file, 'replace','sram5', '64K', True, False, rate['avg']);
	por = find_stats(file, 'replace','sram5', '64K', True, True, rate['min']);
	discard = find_stats(file, 'discard','sram5', '64K', True, False, rate['avg']);
	statistics_add_rows('blackscholes', rate['avg'], data, rigid, por, discard);

	file='imgops/task_stats_summary.tab'
	rate=rates['scale']
	rigid = find_stats(file, 'replace','sram3', 'baboon', True, False, rate['avg']);
	por = find_stats(file, 'replace','sram3', 'baboon', True, True, rate['min']);
	discard = find_stats(file, 'discard','sram3', 'baboon', True, False, rate['avg']);
	statistics_add_rows('scale', rate['avg'], data, rigid, por, discard);

	
	file = 'barnes/task_stats_summary.tab';
	rate=rates['barnes']
	rigid = find_stats(file, 'replace','sram5', '16K.12', True, False, rate['avg']);
	por = find_stats(file, 'replace','sram5', '16K.12', True, True, rate['min']);
	discard = find_stats(file, 'discard','sram5', '16K.12', True, False, rate['avg']);
	print rigid
	statistics_add_rows('barnes', rate['avg'], data, rigid, por, discard);
	
	
	
	bsen = 'water/task_stats_summary.tab';
	rate=rates['water']
	rigid = find_stats(bsen, 'replace.0','sram5', '100.12.2', True, False, rate['avg']);
	por = find_stats(bsen, 'replace.0','sram5', '100.12.2', True, True, rate['min']);
	discard = find_stats(bsen, 'discard.0','sram5', '100.12.2', True, False, rate['avg']);
	statistics_add_rows('water-interf', rate['avg'], data, rigid, por, discard);
	
	rigid = find_stats(bsen, 'replace.1','sram5', '100.12.2', True, False, rate['avg']);
	por = find_stats(bsen, 'replace.1','sram5', '100.12.2', True, True, rate['min']);
	discard = find_stats(bsen, 'discard.1','sram5', '100.12.2', True, False, rate['avg']);
	statistics_add_rows('water-poteng', rate['avg'], data, rigid, por, discard);
	
	file = 'search/task_stats_summary.tab';
	rate=rates['search']
	rigid = find_stats(file, 'replace','sram5', '1', True, False, rate['avg']);
	por = find_stats(file, 'replace','sram5', '1', True, True, rate['min']);
	discard = find_stats(file, 'discard','sram5', '1', True, False, rate['avg']);
	statistics_add_rows('search', rate['avg'], data, rigid, por, discard);
	
	write_summary(outfile1, hdr[0],data[0]);
	write_summary(outfile2, hdr[1],data[1]);

energy_summary('energy_summary', 'por_energy_summary');
output_summary('output_summary', 'por_output_summary');
statistics_summary('stats_summary', 'por_stats_summary');
