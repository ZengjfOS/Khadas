all:
	dtc -O dtb -o at24c02_overlay.dtbo at24c02_overlay.dts
clean:
	rm at24c02_overlay.dtbo
