Nomenclatura:
	Clases Privadas, Template Arguments, Structs, Enums -> CamelCase
	Resto -> snake_case


Carpetas:

​	Boot:

​		Proceso de boot. Solo codigo fuente.

​	Build:

​		Scripts de compilacion, output del codigo fuente de boot y

​		output de kernel y drivers. Config de boshch

​	Drivers:

​		Aun na

​	Kernel:

​		Codigo fuente del kernel y carpeta donde se generan los arhcivos intermedios de la compilacion de cpp.

​		Tb hay un makefile para esta parte especifica.

Execute: To execute TRES, there are two options, in docker or in local.

Compile:
	Docker:
	./build_docker.sh
	Local:
	./build.sh
Execute
	sudo qemu-system-i386 -machine q35 disco.img -drive file=newdrive.img