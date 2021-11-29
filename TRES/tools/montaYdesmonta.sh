[ -d /media/definitivo ] || mkdir /media/definitivo
sudo mount $1 /media/definitivo
sudo cp SABERR.TXT /media/definitivo
sudo cp KERNEL.BIN /media/definitivo
sudo cp SNDSTG.BIN /media/definitivo
sudo umount /media/definitivo