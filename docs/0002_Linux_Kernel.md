# Linux Kernel

## 参考文档

* [Build Ubuntu/Debian Images](https://docs.khadas.com/vim1/FenixScript.html)

## Kernel Download

* https://github.com/khadas/fenix/blob/master/config/boards/VIM3.conf#L16
  ```
  case "$LINUX" in
  	mainline)
  		LINUX_DTB="arch/arm64/boot/dts/amlogic/meson-g12b-a311d-khadas-vim3.dtb"
  		LINUX_GIT_BRANCH="master"
  		LINUX_DEFCONFIG="defconfig"
  		SERIALCON="ttyAML0"
  		GPU_VER=""
  		GPU_PLATFORM=""
  		GPU_DESC=""
  		MODULES="brcmfmac"
  		;;
  	4.9)
  		LINUX_DTB="arch/arm64/boot/dts/amlogic/kvim3_linux.dtb"
  		LINUX_GIT_BRANCH="khadas-vims-4.9.y"
  		LINUX_DEFCONFIG="kvims_defconfig"
  		SERIALCON="ttyS0"
  		GPU_VER="r12p0"
  		GPU_PLATFORM="fbdev"
  		GPU_DESC="gondul"
  		MODULES="dhd media_clock firmware decoder_common stream_input amvdec_avs amvdec_h264 amvdec_mh264 amvdec_h264mvc amvdec_h265 amvdec_mjpeg amvdec_mmjpeg amvdec_mpeg12 amvdec_mmpeg12 amvdec_mpeg4 amvdec_mmpeg4 amvdec_real amvdec_vc1 amvdec_vp9 amvdec_avs2 encoder vpu galcore"
  		;;
  esac
  ```
* uname -a
  ```
  Linux Khadas 4.9.190 #51 SMP PREEMPT Fri Aug 30 11:44:18 CST 2019 aarch64 aarch64 aarch64 GNU/Linux
  ```
* https://github.com/khadas/linux/tree/khadas-vims-4.9.y
  * Makefile
    ```
    VERSION = 4
    PATCHLEVEL = 9
    SUBLEVEL = 190
    EXTRAVERSION =
    NAME = Roaring Lionus
    [...省略]
    ```

## Compile Kernel

* 参考：https://github.com/khadas/fenix/blob/master/config/functions/build#L78
* arch/arm64/configs/
  ```
  defconfig        meson64_defconfig            ranchu64_defconfig
  kvims_defconfig  meson64_smarthome_defconfig
  ```
* make ARCH=arm64 kvims_defconfig
  ```
    HOSTCC  scripts/basic/fixdep
    HOSTCC  scripts/kconfig/conf.o
    SHIPPED scripts/kconfig/zconf.tab.c
    SHIPPED scripts/kconfig/zconf.lex.c
    SHIPPED scripts/kconfig/zconf.hash.c
    HOSTCC  scripts/kconfig/zconf.tab.o
    HOSTLD  scripts/kconfig/conf
  drivers/net/wireless/bcmdhd/Kconfig:53:warning: defaults for choice values not supported
  #
  # configuration written to .config
  #
  ```
* make ARCH=arm64 -j4  
  这条命令会编译出zImage、.ko、dtb（arch/arm64/boot/dts/amlogic/kvim3_linux.dtb），或者`make -j4 ARCH=arm64 Image dtbs modules`
* 确认内核：
  ```Console
  khadas@Khadas:~/zengjf/linux-khadas-vims-4.9.y$ file arch/arm64/boot/Image
  arch/arm64/boot/Image: MS-DOS executable
  khadas@Khadas:~/zengjf/linux-khadas-vims-4.9.y$ sudo file /boot/zImage_bak
  [sudo] password for khadas:
  /boot/zImage_bak: MS-DOS executable
  ```
  * sudo cp arch/arm64/boot/Image /boot/zImage
* 确认设备树
  ```Console
  khadas@Khadas:~/zengjf/linux-khadas-vims-4.9.y$ file arch/arm64/boot/dts/amlogic/kvim3_linux.dtb
  arch/arm64/boot/dts/amlogic/kvim3_linux.dtb: Device Tree Blob version 17, size=86214, boot CPU=0, string block size=8214, DT structure block size=77944
  khadas@Khadas:~/zengjf/linux-khadas-vims-4.9.y$ sudo file /boot/dtb.img
  /boot/dtb.img: Device Tree Blob version 17, size=86214, boot CPU=0, string block size=8214, DT structure block size=77944
  ```
  * sudo cp arch/arm64/boot/dts/amlogic/kvim3_linux.dtb /boot/dtb.img
* make headers_install


## hello_module compile

* https://github.com/ZengjfOS/RaspberryPi/tree/hello_module
* make error
  ```
  make -C /lib/modules/4.9.190/build M=/home/khadas/zengjf/hello_module modules
  make[1]: Entering directory '/usr/src/linux-headers-4.9.190'
    CC [M]  /home/khadas/zengjf/hello_module/hello.o
  In file included from ./include/linux/irqflags.h:15:0,
                   from ./include/linux/spinlock.h:53,
                   from ./include/linux/seqlock.h:35,
                   from ./include/linux/time.h:5,
                   from ./include/uapi/linux/timex.h:56,
                   from ./include/linux/timex.h:56,
                   from ./include/linux/sched.h:19,
                   from ./arch/arm64/include/asm/compat.h:25,
                   from ./arch/arm64/include/asm/stat.h:23,
                   from ./include/linux/stat.h:5,
                   from ./include/linux/module.h:10,
                   from /home/khadas/zengjf/hello_module/hello.c:3:
  ./arch/arm64/include/asm/irqflags.h:24:10: fatal error: ../drivers/amlogic/debug/irqflags_debug_arm64.h: No such file or directory
   #include <../drivers/amlogic/debug/irqflags_debug_arm64.h>
            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  compilation terminated.
  scripts/Makefile.build:340: recipe for target '/home/khadas/zengjf/hello_module/hello.o' failed
  make[2]: *** [/home/khadas/zengjf/hello_module/hello.o] Error 1
  Makefile:1639: recipe for target '_module_/home/khadas/zengjf/hello_module' failed
  make[1]: *** [_module_/home/khadas/zengjf/hello_module] Error 2
  make[1]: Leaving directory '/usr/src/linux-headers-4.9.190'
  Makefile:4: recipe for target 'all' failed
  make: *** [all] Error 2
  ```
* sudo cp drivers/amlogic/debug/irqflags_debug_arm64.h /usr/src/linux-headers-4.9.190/drivers/amlogic/debug/
* make
  ```
  make -C /lib/modules/4.9.190/build M=/home/khadas/zengjf/hello_module modules
  make[1]: Entering directory '/usr/src/linux-headers-4.9.190'
    CC [M]  /home/khadas/zengjf/hello_module/hello.o
    Building modules, stage 2.
    MODPOST 1 modules
    CC      /home/khadas/zengjf/hello_module/hello.mod.o
    LD [M]  /home/khadas/zengjf/hello_module/hello.ko
  make[1]: Leaving directory '/usr/src/linux-headers-4.9.190'
  ```
* sudo insmod hello.ko
* dmesg | tail
  ```
  [   45.174852] nf_conntrack: default automatic helper assignment has been turned off for security reasons and CT-based  firewall rule not found. Use the iptables CT target to attach helpers instead.
  [   46.245304] fb: osd[0] enable: 1 (Xorg)
  [   47.648585] meson_uart ff803000.serial: ttyS0 use xtal(24M) 24000000 change 115200 to 115200
  [   50.654143] meson_uart ffd24000.serial: ttyS1 use xtal(24M) 24000000 change 115200 to 2000000
  [   70.574456] fb: mem_free_work, free memory: addr:800000
  [  647.142408] fb: osd[0] enable: 1 (Xorg)
  [  648.144004] fb: osd[0] enable: 0 (Xorg)
  [  649.167971] fb: osd[0] enable: 0 (Xorg)
  [  951.750970] hello: loading out-of-tree module taints kernel.
  [  951.751328] Hello, world.
  ```