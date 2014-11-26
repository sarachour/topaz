#!/bin/bash
#copy this into bashrc

MROOT=$(pwd)
echo "#COPY THIS INTO BASHRC"
echo ""
echo "export TOPAZ_ROOT=$MROOT"
echo ""
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:/usr/local/lib:\$LD_LIBRARY_PATH:\$TOPAZ_ROOT/lib"
echo "export INCLUDE_PATH=\$INCLUDE_PATH:\$TOPAZ_ROOT/include"
echo "export PIN_ROOT=\$TOPAZ_ROOT/emul/siACT/tool/pin-2.12-58423-gcc.4.4.7-linux"
echo "export PATH=\$PATH:\$PIN_ROOT:\$TOPAZ_ROOT/util"
echo "export CLASSPATH=\".:\$TOPAZ_ROOT/lib/antlr-4.2-complete.jar:$CLASSPATH\""
echo "alias antlr4='java -jar \$TOPAZ_ROOT/lib/antlr-4.2-complete.jar'"
echo "alias grun='java org.antlr.v4.runtime.misc.TestRig'"


