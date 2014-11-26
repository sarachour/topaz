{
# Pattern is following:
#
#
#End of fitting process
#
#final parameter set: 
# 6.291456e-01  1.056723e+01  4.480000e+00  1.700000e+00 
#
# Corresponding rmse: 0.332626 
#

    if ( $0 ~ /End of fitting/ ) {
	getline
	getline
	getline
	gsub(/^[ \t]+/, "")
	A=$0

	getline
	getline
	print A, $3
    }
}