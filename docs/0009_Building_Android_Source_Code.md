# Building Android Source Code

主要还是看参考文档，仅补充可能需要修改或者注意的地方。


## 参考文档

* [Building Android Source Code](https://docs.khadas.com/vim3/BuildAndroid.html)
* [Android 9 Source Codes for VIM3](https://forum.khadas.com/t/android-9-source-codes-for-vim3/4607)


## 注意

* 在参考文档中，和之前做NXP系统是有差异的，这个系统需要自己下载安装编译器，以及环境内容；
* VMware® Workstation 14 Pro 14.1.3 build-9474260
* 本人使用的Ubuntu版本：16.04；
* 硬盘：500G。主要是因为其源代码也是很大的，将近300G；


## Ubuntu 阿里源

* https://opsx.alibaba.com/mirror
  * 点击对应的系统后面的【帮助】可以看到相应的配置内容


## 依赖

* [git-lfs](https://docs.khadas.com/vim3/DownloadAndroidSourceCode.html#Steps)是必须安装的；
* sudo apt-get install u-boot-tools
* sudo apt-get install openjdk-8-jdk android-tools-adb bc bison build-essential curl flex g++-multilib gcc-multilib gnupg gperf imagemagick lib32ncurses5-dev lib32readline-dev lib32z1-dev libesd0-dev liblz4-tool libncurses5-dev libsdl1.2-dev libssl-dev libwxgtk3.0-dev libxml2 libxml2-utils lzop pngcrush rsync schedtool squashfs-tools xsltproc yasm zip zlib1g-dev


## 清华源替换

* https://mirrors.tuna.tsinghua.edu.cn/help/AOSP/
* 修改`.repo/manifest.xml`
  * 将`https://android.googlesource.com/`全部使用`https://aosp.tuna.tsinghua.edu.cn/`代替即可。
* 整个下载过程可能需要3天，吐槽一下：实在是大，大大大，太大，哪都大；


## Build out dir

```
zengjf@zengjf:~/android$ ls out/target/product/kvim3/
2ndbootloader                         kvim3-kernel
android-info.txt                      kvim3-ota-eng.zengjf.zip
android_p_overlay_dt.dtbo             obj
boot.img                              odm
bootloader.img                        odm.img
build_fingerprint.txt                 odm.map
build_thumbprint.txt                  previous_build_config.mk
cache                                 product
cache.img                             product_copy_files_ignored.txt
clean_steps.mk                        product.img
data                                  ramdisk.img
dex_bootjars                          ramdisk-recovery.img
dtbo.img                              recovery
dt.img                                recovery.id
fake_packages                         recovery.img
fastboot                              root
fastboot_auto                         symbols
gen                                   system
installed-files.json                  system.img
installed-files-product.json          update.img
installed-files-product.txt           upgrade
installed-files.txt                   userdata.img
installed-files-vendor.json           utilities
installed-files-vendor.txt            vbmeta.img
kernel                                vendor
kvim3-fastboot-flashall-20191117.zip  vendor.img
kvim3-fastboot-image-20191117.zip
```

`out/target/product/kvim3/update.img`


## Tail Build log

```
[...省略]
vendor_fs_type            = (str) ext4
vendor_size               = (int) 335544320
  running:  openssl pkcs8 -in build/target/product/security/testkey.pk8 -inform DER -nocrypt
(using device-specific extensions from target_files)
loaded device-specific extensions from /home/zengjf/android/out/target/product/kvim3/obj/PACKAGING/target_files_intermediates/kvim3-target_files-eng.zengjf/META/releasetools.py
amlogic extensions:FullOTA_Assertions
using prebuilt recovery.img from IMAGES...
amlogic extensions:FullOTA_InstallBegin
Total of 327680 4096-byte output blocks in 18 input chunks.
Finding transfers...
Generating digraph...
Finding vertex sequence...
Reversing backward edges...
  0/0 dependencies (0.00%) were violated; 0 source blocks stashed.
Improving vertex order...
Revising stash size...
  Total 0 blocks (0 bytes) are packed as new blocks due to insufficient cache size.
Reticulating splines...
 680464384  680464384 (100.00%)     new __DATA 0-86 90-91 101-254 5221-32769 32849-98305 98385-163841 163921-171339 229376-229377 294912-294913
max stashed blocks: 0  (0 bytes), limit: 55364812 bytes (0.00%)

  Imgdiff Stats Report  
========================

Compressing system.new.dat with brotli
  running:  brotli --quality=6 --output=/tmp/tmpctkWlw/system.new.dat.br /tmp/tmpctkWlw/system.new.dat
using prebuilt boot.img from IMAGES...
Total of 81920 4096-byte output blocks in 8 input chunks.
Finding transfers...
Generating digraph...
Finding vertex sequence...
Reversing backward edges...
  0/0 dependencies (0.00%) were violated; 0 source blocks stashed.
Improving vertex order...
Revising stash size...
  Total 0 blocks (0 bytes) are packed as new blocks due to insufficient cache size.
Reticulating splines...
 184844288  184844288 (100.00%)     new __DATA 0-24 27-56 2589-32769 32789-47680
max stashed blocks: 0  (0 bytes), limit: 55364812 bytes (0.00%)

  Imgdiff Stats Report  
========================

Compressing vendor.new.dat with brotli
  running:  brotli --quality=6 --output=/tmp/tmpgMvDUz/vendor.new.dat.br /tmp/tmpgMvDUz/vendor.new.dat
   boot size (9709568) is 57.87% of limit (16777216)
amlogic extensions:FullOTA_InstallEnd
******has odm partition********* out/target/product/kvim3/obj/PACKAGING/target_files_intermediates/kvim3-target_files-eng.zengjf
Total of 32768 4096-byte output blocks in 4 input chunks.
Finding transfers...
Generating digraph...
Finding vertex sequence...
Reversing backward edges...
  0/0 dependencies (0.00%) were violated; 0 source blocks stashed.
Improving vertex order...
Revising stash size...
  Total 0 blocks (0 bytes) are packed as new blocks due to insufficient cache size.
Reticulating splines...
    540672     540672 (100.00%)     new __DATA 0-41 1065-1154
max stashed blocks: 0  (0 bytes), limit: 55364812 bytes (0.00%)

  Imgdiff Stats Report  
========================

Compressing odm.new.dat with brotli
  running:  brotli --quality=6 --output=/tmp/tmpZ8LlXs/odm.new.dat.br /tmp/tmpZ8LlXs/odm.new.dat
Total of 32768 4096-byte output blocks in 4 input chunks.
Finding transfers...
Generating digraph...
Finding vertex sequence...
Reversing backward edges...
  0/0 dependencies (0.00%) were violated; 0 source blocks stashed.
Improving vertex order...
Revising stash size...
  Total 0 blocks (0 bytes) are packed as new blocks due to insufficient cache size.
Reticulating splines...
   3469312    3469312 (100.00%)     new __DATA 0-41 1065-1869
max stashed blocks: 0  (0 bytes), limit: 55364812 bytes (0.00%)

  Imgdiff Stats Report  
========================

Compressing product.new.dat with brotli
  running:  brotli --quality=6 --output=/tmp/tmpCDP5Uk/product.new.dat.br /tmp/tmpCDP5Uk/product.new.dat
  running:  java -Xmx2048m -Djava.library.path=out/host/linux-x86/lib64 -jar out/host/linux-x86/framework/signapk.jar -w build/target/product/security/testkey.x509.pem build/target/product/security/testkey.pk8 /tmp/tmpvFV_MK.zip /tmp/tmpORXiDL.zip
  running:  zip -d /tmp/tmpORXiDL.zip META-INF/com/android/metadata
  running:  java -Xmx2048m -Djava.library.path=out/host/linux-x86/lib64 -jar out/host/linux-x86/framework/signapk.jar -w build/target/product/security/testkey.x509.pem build/target/product/security/testkey.pk8 /tmp/tmpORXiDL.zip out/target/product/kvim3/kvim3-ota-eng.zengjf.zip
done.

#### build completed successfully (16:22 (mm:ss)) ####
```