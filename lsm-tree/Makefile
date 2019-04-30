all: compile


compile:
	touch disk_storage1.txt
	touch disk_storage2.txt
	touch disk_storage3.txt
	touch disk_storage4.txt
	g++ -fpermissive -std=c++11 -Istdc++ -O3 lsm.cpp test.cpp -o lsm

clean: 
	rm -f lsm 
	rm -f *~
	rm -rf *.dSYM
	rm -f disk_storage.txt
