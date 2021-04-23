#Compilan y copian aqui el output

DISCO=disco.img

cd ../boot
./build.sh
cd ../kernel
./build.sh
cd ../build
#Monta y demás
./create_d.sh $DISCO #disco.img #/dev/sde #disco.img   
./montaYdesmonta.sh $DISCO 