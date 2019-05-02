all: compile


compile:
	touch disk_storage1.txt
	touch disk_storage2.txt
	touch disk_storage3.txt
	touch disk_storage4.txt
	g++ -g -fpermissive  lsm.cpp test.cpp -o lsm

clean: 
	rm -f lsm 
	rm -f *~
	rm -rf *.dSYM
	rm -f disk_storage1.txt
	rm -f disk_storage2.txt
	rm -f disk_storage3.txt
	rm -f disk_storage4.txt
