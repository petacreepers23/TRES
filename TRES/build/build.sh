#Compilan y copian aqui el output
cd ../boot
./build.sh
cd ../kernel
./build.sh
cd ../build
#Monta y demás
./create_d.sh disco.img   
./montaYdesmonta.sh 