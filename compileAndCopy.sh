make clean; make;
sudo mount /dev/sda /media/nucleo;
sudo cp .build/bot1_411_Leika.bin /media/nucleo/;sync;
sudo umount /media/nucleo
