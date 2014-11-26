
TOOL="im_affinei"
# a b c d dx dy x y w h
ARGS="bilinear 1.5 0 0 1.5 100 100 0 0 2336 2336"
INPUT="input/vulture_2336x2336.v"
OUTPUT="out"
vips $TOOL
src/bin/bin/vips $TOOL $INPUT $OUTPUT.v $ARGS
src/bin/bin/vips im_vips2png $OUTPUT.v $OUTPUT.png
rm $OUTPUT.v
