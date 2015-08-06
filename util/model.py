#!/usr/bin/python
import re
import sys

if len(sys.argv) < 4:
	print "USAGE: detector_summary stats_summary energy_summary"
	sys.exit(0);
	
def read_file(name, amap, trans):
	fd = open(name,"r");
	res = {};
	for line in fd:
		line = line.split("\n")[0];
		flds = re.split('[,\t:=]+',line);
		for idx in range(0,len(flds)):
			if flds[idx] in amap:
				key = amap[flds[idx]];
				val = trans(flds,idx);
				res[key] = val;
		print flds
		
	return res;
		
		
det = sys.argv[1];
stats = sys.argv[2];
energy = sys.argv[3];

fun = lambda a,x : float(a[x + 1]);
fun2 = lambda a,x : float(a[x + 2]);

argmap = {"aov-err-acc":"ae"}
opts = read_file(stats,argmap, fun2);
print opts
eps_aov = opts["ae"]/100 


argmap = {"false-accepts":"fa","true-rejects":"tr", "false-rejects":"fr","n":"n"}
opts = read_file(det, argmap, fun);
eps_det = opts["tr"]/(opts["fa"] + opts["tr"])
n = opts["n"]
eps_err_per_task = (opts["fa"] + opts["tr"])*0.01
eps_rej_per_task = (opts["fr"] + opts["tr"])*0.01
pct_fr_per_tr = opts["fr"]/(opts["tr"])

argmap = {"Nominal Cache+Memory Savings": "sav", "M": "M", "TE": "TE", "TA": "TA", "D":"D", "OE":"OE", "OA":"OA"};
opts = read_file(energy, argmap, fun);

Delta_approx = 1- opts["sav"]/100;
Delta_prec = 1.0;
# per task
sigma_main = opts["M"]
sigma_task = opts["TA"]
sigma_reexec = opts["TE"]
sigma_det = opts["D"]
sigma_oe_prec = opts["OE"]
sigma_oa_prec = opts["OA"]
eps_err = eps_err_per_task/sigma_task


print "---------------------------------------"
print "Property, Predicted, Actual"
# compute reexec rates

pred_err = eps_err*sigma_task
print "Error Rate,", pred_err,",",eps_err_per_task


pred_reexec = eps_aov*eps_det*pred_err*(1+pct_fr_per_tr)
print "Reexecution Rate,", pred_reexec,",",eps_rej_per_task


baseline = Delta_prec*sigma_task + Delta_prec*sigma_main


notopaz = Delta_prec*sigma_det + Delta_prec*pred_reexec*sigma_task + Delta_approx*sigma_task + Delta_prec*sigma_main
topaz = notopaz + Delta_prec*sigma_oe_prec + Delta_prec*sigma_oa_prec

pred_savings_notpz = 1 - notopaz/baseline
pred_savings_tpz = 1 - topaz/baseline
print "Energy Savings NoTopaz,",pred_savings_notpz,",","???"
print "Energy Savings Topaz,",pred_savings_tpz,",","???"
