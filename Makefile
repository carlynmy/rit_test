
DEBUG_BUILD_TYPE=-DCMAKE_BUILD_TYPE=Debug
RELEASE_BUILD_TYPE=-DCMAKE_BUILD_TYPE=Release

STANDART_BUILD=
# STANDART_BUILD= $(DEBUG_BUILD_TYPE)

PATH_BUILD=build

.PHONY: all clean rebuild common server client

all: server client

clean:
	rm -rf $(PATH_BUILD)

rebuild: clean all

common:
	cmake -B $(PATH_BUILD) $(STANDART_BUILD) 
	cmake --build $(PATH_BUILD) --target common

server:
	cmake -B $(PATH_BUILD) $(STANDART_BUILD) 
	cmake --build $(PATH_BUILD) --target server
	
client:
	cmake -B $(PATH_BUILD) $(STANDART_BUILD) 
	cmake --build $(PATH_BUILD) --target client

