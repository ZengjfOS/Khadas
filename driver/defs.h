// Global settings
#define PCIE_PAGESIZE_NBITS  (12)                      // this will be checked against PAGE_SHIFT when the driver gets built
#define PCIE_PAGESIZE        (1U<<PCIE_PAGESIZE_NBITS)  // this will be checked against PAGE_SHIFT when the driver gets built

// FPGA registers
#define REGADDR_NBITS        (PCIE_PAGESIZE_NBITS-2)   // we have 2^(PCIE_PAGESIZE_NBITS-2) -> 1024 registers on machines with 4KiB pages...
#define NUM_REGS             (1U<<REGADDR_NBITS)        // total number of registers
#define PRV_BASE             (NUM_REGS/2)              // the upper 50% of the registers are kernel-accessible only
#define CTL_BASE             (PRV_BASE - 2)            // this must be set equal to the lowest-numbered user-accessible control register
#define C2F_WRPTR            (PRV_BASE - 2)            // CPU->FPGA write pointer, updated by the CPU after it has written a chunk
#define F2C_RDPTR            (PRV_BASE - 1)            // FPGA->CPU read pointer, updated by the CPU after it has read a chunk
#define DMA_ENABLE           (PRV_BASE + 0)            // reset switch and DMA enable
#define F2C_BASE             (PRV_BASE + 1)            // FPGA->CPU base address
#define MTR_BASE             (PRV_BASE + 2)            // metrics base address

// The FPGA register region (R/W, noncacheable): two pages mapped to a BAR on the FPGA
#define REG_BAR              (0)
#define REG_SIZE_NBITS       (PCIE_PAGESIZE_NBITS+1)   // the register region is two pages in size
#define REG_SIZE             (1U<<REG_SIZE_NBITS)       // size of the register region in bytes

// The CPU->FPGA region (write-only, write-combined): potentially multiple pages mapped to a BAR on the FPGA
#define C2F_BAR              (2)
#define C2F_SIZE_NBITS       (PCIE_PAGESIZE_NBITS+0)   // the CPU->FPGA buffer is one page
#define C2F_CHUNKSIZE_NBITS  (8)                       // each chunk is 256 bytes -> therefore there will be 4096/256 -> 16 chunks
#define C2F_NUMCHUNKS_NBITS  (C2F_SIZE_NBITS - C2F_CHUNKSIZE_NBITS)
#define C2F_SIZE             (1U<<C2F_SIZE_NBITS)
#define C2F_CHUNKSIZE        (1U<<C2F_CHUNKSIZE_NBITS)
#define C2F_NUMCHUNKS        (1U<<C2F_NUMCHUNKS_NBITS)

// The metrics buffer (e.g f2cWrPtr, c2fRdPtr - read-only): one page allocated by the kernel and DMA'd into by the FPGA
#define MTR_SIZE_NBITS       (PCIE_PAGESIZE_NBITS)
#define MTR_SIZE             (1U<<MTR_SIZE_NBITS)

// The FPGA->CPU buffer (read-only): potentially multiple pages allocated by the kernel and DMA'd into by the FPGA
#define F2C_TLPSIZE_NBITS    (7)                       // as per PCIe spec this is ≤8 (meaning F2C_TLPSIZE ≤256 bytes)
#define F2C_TLPSIZE          (1U<<F2C_TLPSIZE_NBITS)
#define F2C_SIZE_NBITS       (PCIE_PAGESIZE_NBITS+0)   // the FPGA->CPU buffer is one page
#define F2C_CHUNKSIZE_NBITS  (9)                       // each chunk is 512 bytes -> therefore there will be 4096/512 -> 8 chunks
#define F2C_NUMCHUNKS_NBITS  (F2C_SIZE_NBITS - F2C_CHUNKSIZE_NBITS)
#define F2C_SIZE             (1U<<F2C_SIZE_NBITS)
#define F2C_CHUNKSIZE        (1U<<F2C_CHUNKSIZE_NBITS)
#define F2C_NUMCHUNKS        (1U<<F2C_NUMCHUNKS_NBITS)
