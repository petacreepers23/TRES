#Compilan y copian aqui el output
echo "Deprecated"
DISCO=disco.img

cd ../boot
./build.sh
cd ../kernel
./build.sh
cd ../tools
#Monta y demás
./create_d.sh $DISCO #disco.img #/dev/sde #disco.img   
./montaYdesmonta.sh $DISCO 