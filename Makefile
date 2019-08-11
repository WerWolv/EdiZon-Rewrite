.PHONY: clean all application sysmodule

all: application sysmodule

application: sysmodule
	$(MAKE) -C application

sysmodule:
	$(MAKE) -C sysmodule
	@rm -rf ./application/romfs/sysmodule
	@mkdir ./application/romfs/sysmodule
	@cp ./sysmodule/out/exefs.nsp ./application/romfs/sysmodule/exefs.nsp

install:
	$(MAKE) -C application install

clean:
	$(MAKE) -C application clean
	$(MAKE) -C sysmodule clean
	