.PHONY: clean all application overlay release

all: application overlay

--release:
	$(eval SNAPSHOT := 0)

release: --release clean application

application: overlay
	@printf "\x1b[32;01m ==== Building application ==== \x1b[0m\n"

	@$(MAKE) -C application --no-print-directory

	@printf "\x1b[32;01m ==== Compiled Successfully ==== \x1b[0m\n\n"

overlay:
	@printf "\x1b[32;01m ==== Building Overlay ==== \x1b[0m\n"

	@$(MAKE) -C overlay --no-print-directory

	@printf "\x1b[32;01m ==== Compiled Successfully ==== \x1b[0m\n\n"
	
	@rm -rf ./application/romfs/overlay
	@mkdir -p ./application/romfs/overlay
	@cp ./overlay/out/ovlEdiZon.ovl ./application/romfs/overlay/ovlEdiZon.ovl

install: application
	@$(MAKE) -C application install --no-print-directory

install_overlay: overlay
	@$(MAKE) -C overlay install --no-print-directory

clean:
	@$(MAKE) -C application clean --no-print-directory
	@$(MAKE) -C overlay clean --no-print-directory
	