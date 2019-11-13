# U-Boot

**注意**：u-boot不能直接在主板上进行编译，主要是因为主板的gcc编译器和u-boot的版本对不上；

## 参考文档

* [Install Extra Toolchains For The Amlogic Platform](https://docs.khadas.com/vim1/InstallToolchains.html)
* [Build Ubuntu/Debian Images](https://docs.khadas.com/vim1/FenixScript.html)

## U-Boot Download

* U-Boot 2015.01-gf133d1a-dirty (Sep 07 2019 - 21:02:17)
* https://github.com/khadas/fenix/blob/master/config/boards/VIM3.conf
  ```
  case "$UBOOT" in
  	2015.01)
  		UBOOT_DEFCONFIG="kvim3_defconfig"
  		UBOOT_GIT_BRANCH="khadas-vims-v2015.01"
  	;;
  	mainline)
  		UBOOT_DEFCONFIG="khadas-vim3_defconfig"
  		UBOOT_GIT_BRANCH="master"
  	;;
  esac
  ```
* https://github.com/khadas/u-boot/blob/khadas-vims-v2015.01

## U-Boot Compile

* 参考：https://github.com/khadas/fenix/blob/master/config/functions/build#L8 
* make kvim3_defconfig
* make -j4