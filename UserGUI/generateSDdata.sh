BASEDIR=${PWD}/$(dirname $0)
echo " Generating files for Arduino WebPage "
echo "----------------------------------------------------------"
echo "Current dir: ${BASEDIR%?}"
echo "Copying the file 'bootstrap.min.css'"
cp -v ${BASEDIR%?}/css/bootstrap.min.css ${BASEDIR%?}/sdFiles/BS.CSS;
echo "Copying the file 'bootstrap-responsive.min.css'"
cp ${BASEDIR%?}/css/bootstrap-responsive.min.css ${BASEDIR%?}/sdFiles/BSR.CSS;
echo "Copying the file 'bootstrap.min.js'"
cp ${BASEDIR%?}/js/bootstrap.min.js ${BASEDIR%?}/sdFiles/BSJS.TXT;
echo "Copying the file 'control.js'"
cp ${BASEDIR%?}/js/control.js ${BASEDIR%?}/sdFiles/CTRL.TXT;
echo "Copying the file 'jquery.min.js'"
cp ${BASEDIR%?}/js/jquery.min.js ${BASEDIR%?}/sdFiles/JQUERY.TXT;
echo "Copying the file 'shield.txt'" 
cp ${BASEDIR%?}/img/shield.txt ${BASEDIR%?}/sdFiles/SHIELD.TXT;
echo "Copying the file 'index.html'"
cp ${BASEDIR%?}/index.html ${BASEDIR%?}/sdFiles/INDEX.TXT;
echo "----------------------------------------------------------"
