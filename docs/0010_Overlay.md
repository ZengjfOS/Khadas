# Overlay

## 参考文档

* [VIM3 Ubuntu Kernel support CONFIG_OF_OVERLAY](https://forum.khadas.com/t/vim3-ubuntu-kernel-support-config-of-overlay/6093)

## Bad of_node_put() patch

打开OF_OVERLAY会导致内核崩溃（OF: ERROR: Bad of_node_put() on），需要打这个补丁

```diff
diff --git a/drivers/amlogic/thermal/meson_cooldev.c b/drivers/amlogic/thermal/meson_cooldev.c
index db2c9e47..91713461 100644
--- a/drivers/amlogic/thermal/meson_cooldev.c
+++ b/drivers/amlogic/thermal/meson_cooldev.c
@@ -129,6 +129,9 @@ static int meson_set_min_status(struct thermal_cooling_device *cdev,
                goto end;
        for_each_available_child_of_node(tzdnp, child) {
                coolmap = of_find_node_by_name(child, "cooling-maps");
+               if (!coolmap)
+                       goto end;
+
                for_each_available_child_of_node(coolmap, gchild) {
                        struct of_phandle_args cooling_spec;
                        int ret;
```

## DTS configfs 

* [drivers/of/configfs.c](https://github.com/raspberrypi/linux/blob/rpi-4.9.y-stable/drivers/of/configfs.c)
* patch
  ```diff
  diff --git a/drivers/of/Kconfig b/drivers/of/Kconfig
  index ba7b034b..de0da10d 100644
  --- a/drivers/of/Kconfig
  +++ b/drivers/of/Kconfig
  @@ -112,4 +112,11 @@ config OF_OVERLAY
   config OF_NUMA
          bool
  
  +config OF_CONFIGFS
  +       bool "Device Tree Overlay ConfigFS interface"
  +       select CONFIGFS_FS
  +       select OF_OVERLAY
  +       help
  +         Enable a simple user-space driven DT overlay interface.
  +
   endif # OF
  diff --git a/drivers/of/Makefile b/drivers/of/Makefile
  index d7efd9d4..aa5ef9dd 100644
  --- a/drivers/of/Makefile
  +++ b/drivers/of/Makefile
  @@ -1,4 +1,5 @@
   obj-y = base.o device.o platform.o
  +obj-$(CONFIG_OF_CONFIGFS) += configfs.o
   obj-$(CONFIG_OF_DYNAMIC) += dynamic.o
   obj-$(CONFIG_OF_FLATTREE) += fdt.o
   obj-$(CONFIG_OF_EARLY_FLATTREE) += fdt_address.o
  ```
* rebuild kernel and replace `/boot/zImage`
* reboot
* ls /sys/kernel/config/device-tree/
  ```
  overlays
  ```

## at24c02 overlay

* at24c02_overlay.dts
  ```
  // VIM3
  /dts-v1/;
  /plugin/;
  
  / {
          compatible = "amlogic, g12b";
  
          fragment@0 {
                  target = <&i2c3>;
                  __overlay__ {
                          #address-cells = <1>;
                          #size-cells = <0>;
                          status = "okay";
  
                          DebugHAT@51 {
                                  compatible = "atmel,24c02";
                                  reg = <0x50>;
                                  status = "okay";
                          };
                  };
          };
  };
  ```
* dtc -O dtb -o at24c02_overlay.dtbo at24c02_overlay.dts
* https://github.com/ZengjfOS/Khadas/tree/overlay_example

## test overlay

* sudo su
* mkdir /sys/kernel/config/device-tree/overlays/at24
* ls /sys/kernel/config/device-tree/overlays/at24
  ```
  dtbo  path  status
  ```
* cat at24c02_overlay.dtbo > /sys/kernel/config/device-tree/overlays/at24/dtbo
* cat /sys/kernel/config/device-tree/overlays/at24/status
  ```
  applied
  ```
* ls /sys/bus/i2c/devices/
  ```
  3-0038  3-0050  4-0018  4-0020  4-0051  i2c-3  i2c-4
  ```
* ls /sys/bus/i2c/devices/3-0050/
  ```
  3-00500  driver  eeprom  modalias  name  of_node  power  subsystem  uevent
  ```
* echo "zengjf" > /sys/bus/i2c/devices/3-0050/eeprom
* cat /sys/bus/i2c/devices/3-0050/eeprom
  ```
  zengjf
  ```
