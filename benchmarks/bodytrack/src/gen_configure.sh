#!/bin/bash

echo $TOPAZ_ROOT
echo "cat configure.template | sed s#@@@TOPAZ@@@#$TOPAZ_ROOT#g > configure"
cat configure.template | sed s\#@@@TOPAZ@@@\#$TOPAZ_ROOT\#g > configure
chmod +x configure
