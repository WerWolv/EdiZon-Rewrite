.PHONY: clean all application sysmodule

all: application sysmodule

application:
	$(MAKE) -C application

sysmodule:
	$(MAKE) -C sysmodule

clean:
	$(MAKE) -C application clean
	$(MAKE) -C sysmodule clean
	