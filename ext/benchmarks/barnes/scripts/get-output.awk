
##ENERGY TOTALS
## MASS:        8192
## ENERGY:     (5848927.9466 12043418.1883 -6194490.2417) 
## KIN ENERGY: [(4.04028e+06 830183 936380 )(830183 3.95108e+06 714322 )(936380 714322 4.05205e+06 )]
## POT ENERGY: [(-2.07494e+06 -231136 -320490 )(-224751 -2.11078e+06 -263713 )(-322490 -244520 -2.08395e+06 )]
## AGGREGATE CM POS: ( -4.0001  -1.9947   0.0021) 
## AGGREGATE CM VEL: (  0.0109   0.0460   0.0091) 
## ANGULAR MOMENTUM: (-1162.2472 666.7073 -801.6823)


BEGIN { }
{
    if ( $0 ~ /ENERGY TOTALS/ ) {
	getline
	m=$2
	getline
	gsub(/\(/, ""); gsub(/\)/, "")
	e0 = $2
	e1 = $3
	e2 = $4
	getline
	gsub(/\[/, ""); gsub(/\]/, "")
	gsub(/\(/, ""); gsub(/\)/, "")
	gsub(/KIN ENERGY:/, "")
	k = $0
	
	getline
	gsub(/\[/, ""); gsub(/\]/, "")
	gsub(/\(/, ""); gsub(/\)/, "")
	gsub(/POT ENERGY:/, "")
	p = $0

	getline
	gsub(/\(/, ""); gsub(/\)/, "")
	gsub(/AGGREGATE CM POS:/, "")
	cmp = $0

	getline
	gsub(/\(/, ""); gsub(/\)/, "")
	gsub(/AGGREGATE CM VEL:/, "")
	cmv = $0

	getline
	gsub(/\(/, ""); gsub(/\)/, "")
	gsub(/ANGULAR MOMENTUM:/, "")
	am = $0



    }
}
END { print m, e0, e1, e2, k, p, cmp, cmv, am }