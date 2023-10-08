main:main.o net_property.o net_config/inc/net_intf.h
	gcc -Wall main.o net_property.o -o main -I net_config/inc

main.o:main.c net_config/inc/net_intf.h
	gcc -Wall -c main.c -I net_config/inc/

net_property.o:net_config/src/net_property.c net_config/inc/net_property.h net_config/inc/net_intf.h
	gcc -Wall -c net_config/src/net_property.c -I net_config/inc/

clean:
	rm -f main main.o net_property.o
