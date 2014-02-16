#!/bin/sh
cp css/bootstrap-responsive.min.css sdFiles/BSR.CSS
cp js/bootstrap.min.js sdFiles/BSJS.TXT
java -jar yuicompressor.jar -v --type js -o sdFiles/CTRL.TXT js/control.js
cp -v js/jquery.min.js sdFiles/JQUERY.TXT
cp -v img/shield.txt sdFiles/SHIELD.TXT
java -jar htmlcompressor.jar -t html --compress-js --compress-css --remove-quotes --remove-intertag-spaces -o sdFiles/INDEX.TXT index.html