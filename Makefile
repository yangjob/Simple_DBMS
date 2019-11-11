main : test.o CLStatus.o CLTable.o 
	g++ -o test test.o CLStatus.o CLTable.o -lpthread
	rm *.o

test.o : test.cpp
	g++ -o test.o -c test.cpp -g

CLStatus.o : CLStatus.cpp
	g++ -o CLStatus.o -c CLStatus.cpp -g

CLTable.o : CLTable.cpp
	g++ -o CLTable.o -c CLTable.cpp -g

.PHONY : clean
clean:
	rm test test.o CLStatus.o CLTable.o 