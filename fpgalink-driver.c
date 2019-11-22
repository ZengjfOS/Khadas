//
// Copyright (C) 2014, 2017, 2019 Chris McClelland
// Copyright (C) 2008 Leon Woestenberg    <leon.woestenberg@axon.tv>
// Copyright (C) 2008 Nickolas Heppermann <heppermannwdt@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
// the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program. If
// not, see <http://www.gnu.org/licenses/>.
//
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/pci.h>

#include <asm/uaccess.h>


// Read/write register macros
#define REG_RD(r) (ioread32(ape.regVA + 2*(r) + 1))
#define REG_WR(r, v) (iowrite32((v), ape.regVA + 2*(r) + 1))

// Driver name
#define DRV_NAME "fpgalink"

// Altera PCI Express ('ape') board specific book keeping data
//
// Keeps state of the PCIe core and the Chaining DMA controller
// application.
//
static struct AlteraDevice {
  // The kernel pci device data structure provided by probe()
  struct pci_dev *pciDevice;

  // Character device and device number
  struct cdev charDevice;
  dev_t devNum;

  // FPGA's register region (BAR0)
  resource_size_t regBA;
  u32 __iomem *regVA;

  // Metrics page (mapped read-only in userspace; only mapped into kernel space so the reset ioctl() can zero it)
  dma_addr_t mtrBA;
  volatile u32 *mtrVA;

  // CPU->FPGA buffer region (BAR2) - technically write-only; only mapped into kernel space so ioctl() can demonstrate kernel write-combining
  resource_size_t c2fBA;
  u64 *c2fVA;

  // FPGA->CPU DMA buffer (mapped read-only in userspace, only mapped into kernel space so the reset ioctl() can zero it)
  dma_addr_t f2cBA;
  volatile u32 *f2cVA;

  // Board revision
  u8 revision;
} ape;

// Userspace is opening the device
//
static int cdevOpen(struct inode *inode, struct file *filp) {
  printk(KERN_DEBUG "cdevOpen()\n");
  return 0;
}

// Userspace is closing the device
//
static int cdevRelease(struct inode *inode, struct file *filp) {
  printk(KERN_DEBUG "cdevRelease()\n");
  return 0;
}


static long cdevIOCtl(struct file *filp, unsigned int cmd, unsigned long arg) {

  int err = 0;

  printk(KERN_DEBUG "%s\n", __func__);

  return 0;
}

// Callbacks for file operations on /dev/fpga0
//
static const struct file_operations cdevFileOps = {
  .owner          = THIS_MODULE,
  .open           = cdevOpen,
  .release        = cdevRelease,
  .unlocked_ioctl = cdevIOCtl
};

// Called when the PCI subsystem thinks we can control the given device. Inspect
// if we can support the device and if so take control of it.
//
// Return 0 when we have taken control of the given device.
//
// - allocate board specific bookkeeping
// - enable the board
// - verify board revision
// - request regions
// - query DMA mask
// - obtain and request irq
// - map regions into kernel address space
// - allocate DMA buffer
// - allocate char driver major/minor
//
static int pcieProbe(struct pci_dev *dev, const struct pci_device_id *id) {
  int rc, alreadyInUse = 0;
  printk(KERN_DEBUG "pcieProbe(dev = 0x%p, pciid = 0x%p)\n", dev, id);

  ape.pciDevice = dev;
  dev_set_drvdata(&dev->dev, &ape);
  printk(KERN_DEBUG "pcieProbe() ape = 0x%p\n", &ape);

  // Enable device
  rc = pci_enable_device(dev);
  if ( rc ) {
    printk(KERN_ERR "pci_enable_device() failed (rc=%d)!\n", rc);
    goto err_enable;
  }

#if 0
  // Enable bus master capability on device
  pci_set_master(dev);

  // Get the revision ID (specified in QSys when PCIe IP is generated)
  pci_read_config_byte(dev, PCI_REVISION_ID, &ape.revision);

  // Reserve I/O regions for all BARs
  rc = pci_request_regions(dev, DRV_NAME);
  if ( rc ) {
    alreadyInUse = 1;
    goto err_regions;
  }

  // Set appropriate DMA mask
  if ( !pci_set_dma_mask(dev, DMA_BIT_MASK(32)) ) {
    pci_set_consistent_dma_mask(dev, DMA_BIT_MASK(32));
    printk(KERN_DEBUG "Using a 32-bit DMA mask.\n");
  } else {
    printk(KERN_DEBUG "pci_set_dma_mask() fails for both 32-bit and 64-bit DMA!\n");
    rc = -ENODEV; goto err_mask;
  }

  // Get FPGA register BAR base address, and validate its size
  ape.regBA = pci_resource_start(dev, REG_BAR);
  if (!ape.regBA || pci_resource_len(dev, REG_BAR) != 2*PAGE_SIZE) {
    printk(KERN_DEBUG "REG_BAR has bad configuration\n");
    rc = -1; goto err_map0;
  }
  printk(KERN_DEBUG "REG_BAR @0x%08X\n", (u32)ape.regBA);
  ape.regVA = pci_iomap(dev, REG_BAR, 2*PAGE_SIZE);
  if (!ape.regVA) {
    printk(KERN_DEBUG "Could not map BAR0 into kernel address-space!\n");
    rc = -1; goto err_iomap0;
  }

  // Get CPU->FPGA BAR base address, and validate its size
  ape.c2fBA = pci_resource_start(dev, C2F_BAR);
  if (!ape.c2fBA || pci_resource_len(dev, C2F_BAR) != C2F_SIZE) {
    printk(KERN_DEBUG "C2F_BAR has bad configuration\n");
    rc = -1; goto err_map2;
  }
  printk(KERN_DEBUG "C2F_BAR @0x%08X\n", (u32)ape.c2fBA);
  ape.c2fVA = (u64 *)ioremap_wc(ape.c2fBA, C2F_SIZE);
  if (!ape.c2fVA) {
    printk(KERN_DEBUG "Could not map BAR2 into kernel address-space!\n");
    rc = -1; goto err_iomap2;
  }

  // Allocate and map coherently-cached memory for a DMA-able buffer (see
  // Documentation/PCI/PCI-DMA-mapping.txt, near line 318)
  //
  ape.mtrVA = (volatile u32 *)__get_free_pages(GFP_USER | GFP_DMA32, 0);  // one page for metrics
  if ( !ape.mtrVA ) {
    printk(KERN_DEBUG "Could not allocate metrics buffer!\n");
    rc = -ENOMEM; goto err_mtr_alloc;
  }
  ape.mtrBA = dma_map_single(
    &dev->dev, (void*)ape.mtrVA, PAGE_SIZE, DMA_FROM_DEVICE);
  printk(
    KERN_DEBUG "Allocated metrics buffer (virt: %p; bus: 0x%08X).\n",
    ape.mtrVA, (u32)ape.mtrBA
  );

  ape.f2cVA = (volatile u32 *)__get_free_pages(GFP_USER | GFP_DMA32, F2C_SIZE_NBITS - PAGE_SHIFT);
  if ( !ape.f2cVA ) {
    printk(KERN_DEBUG "Could not allocate DMA buffer!\n");
    rc = -ENOMEM; goto err_buf_alloc;
  }
  ape.f2cBA = dma_map_single(
    &dev->dev, (void*)ape.f2cVA, F2C_SIZE, DMA_FROM_DEVICE);
  printk(
    KERN_DEBUG "Allocated DMA buffer (virt: %p; bus: 0x%08X).\n",
    ape.f2cVA, (u32)ape.f2cBA
  );

  // Allocate char driver major/minor
  rc = alloc_chrdev_region(&ape.devNum, 0, 1, "fpga0");
  if ( rc ) {
    printk(KERN_ERR "alloc_chrdev_region() failed (rc=%d)\n", rc);
    goto err_cdev_alloc;
  }

  // Initialise char device
  cdev_init(&ape.charDevice, &cdevFileOps);
  ape.charDevice.owner = THIS_MODULE;
  ape.charDevice.ops = &cdevFileOps;

  // Add a single device node
  rc = cdev_add(&ape.charDevice, ape.devNum, 1);
  if ( rc ) {
    printk(KERN_ERR "cdev_add() failed (rc=%d)\n", rc);
    goto err_cdev_add;
  }

  // Successfully took the device
  printk(KERN_DEBUG "pcieProbe() successful.\n");
  return 0;
err_cdev_add:
  unregister_chrdev_region(ape.devNum, 1);
err_cdev_alloc:
  dma_unmap_single(&dev->dev, ape.f2cBA, F2C_SIZE, DMA_FROM_DEVICE);
  free_pages((unsigned long)ape.f2cVA, F2C_SIZE_NBITS - PAGE_SHIFT);
err_buf_alloc:
  dma_unmap_single(&dev->dev, ape.mtrBA, PAGE_SIZE, DMA_FROM_DEVICE);
  free_pages((unsigned long)ape.mtrVA, 0);
err_mtr_alloc:
  pci_iounmap(dev, (void __iomem *)ape.c2fVA);
err_iomap2:
err_map2:
  pci_iounmap(dev, ape.regVA);
err_iomap0:
err_map0:
err_mask:
  pci_release_regions(dev);
#endif
err_regions:
  if ( alreadyInUse ) {
    pci_disable_device(dev); // only disable the device if we're sure it's really ours
  }
err_enable:
  return rc;
}

// Called when the module is removed with rmmod
//
static void pcieRemove(struct pci_dev *dev) {

  printk(KERN_DEBUG "pcieRemove(dev = 0x%p) where ape = 0x%p\n", dev, &ape);


  // Disable the PCIe device
  pci_disable_device(dev);
}

// Using the subsystem vendor id and subsystem id, it is possible to
// distinguish between different cards bases around the same
// (third-party) logic core.
//
// Default Altera vendor and device ID's, and some (non-reserved)
// ID's are now used here that are used amongst the testers/developers.
//
static const struct pci_device_id ids[] = {
  { PCI_DEVICE(0x1172, 0xE001), },
  { PCI_DEVICE(0x3952, 0xe951), },
  { 0, }
};
MODULE_DEVICE_TABLE(pci, ids);

// Used to register the driver with the PCI kernel subsystem (see LDD3 page 311)
//
static struct pci_driver pciDriver = {
  .name = DRV_NAME,
  .id_table = ids,
  .probe = pcieProbe,
  .remove = pcieRemove
};

// Module initialization, registers devices.
//
static int __init flInit(void) {
  int rc;
  //printk(KERN_DEBUG DRV_NAME " flInit(), built at " __DATE__ " " __TIME__ "\n");

  // register this driver with the PCI bus driver
  rc = pci_register_driver(&pciDriver);
  if ( rc < 0 ) {
    return rc;
  }
  return 0;
}

// Module cleanup, unregisters devices.
//
static void __exit flExit(void) {
  //printk(KERN_INFO DRV_NAME " flExit(), built at " __DATE__ " " __TIME__ "\n");

  // Unregister PCIe driver
  pci_unregister_driver(&pciDriver);
}

MODULE_LICENSE("GPL");

module_init(flInit);
module_exit(flExit);
