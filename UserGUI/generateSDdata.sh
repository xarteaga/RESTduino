#!/bin/sh
echo "Copying Bootstrap files..."
cp js/bootstrap.min.js sdFiles/BSJS.TXT
cp css/bootstrap.min.css sdFiles/BS.CSS
cp css/bootstrap-responsive.min.css sdFiles/BSR.CSS

echo "Copying jQuery"
cp js/jquery.min.js sdFiles/JQUERY.TXT

echo "Copying raw images"
cp img/shield.txt sdFiles/SHIELD.TXT

echo "Compriming index.html"
java -jar htmlcompressor.jar -t html --compress-js --compress-css --remove-quotes --remove-intertag-spaces -o sdFiles/INDEX.TXT index.html

echo "Compriming control.js"
java -jar yuicompressor.jar -v --type js -o sdFiles/CTRL.TXT js/control.js
