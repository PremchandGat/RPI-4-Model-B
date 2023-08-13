# Building Linux and Filesystem for RPI 4 Model B
## Building Linux Kernel

Here are steps to build Linux kernel

`prem@Ubuntu:~/Desktop$` **git clone https://github.com/raspberrypi/linux.git --depth=1**

`prem@Ubuntu:~/Desktop$` **cd linux**

`prem@Ubuntu:~/Desktop/linux$` **make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig**

`prem@Ubuntu:~/Desktop/linux$` **make -j 12 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-**

### Copy DTB and Linux kernel Image to FAT32 partiotion of SD card
<pre>
DTB file location: linux/arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-b-plus.dts
Kernel Image Location: linux/arch/arm64/boot/Image
</pre>
### To build Linux file system using BusyBox for RPI see [this](https://github.com/PremchandGat/U-BOOT-RPI)

## Testing
Connect to RPI 4 Model B via FTDI and Putty COM port
### Setps to Start kernel
`U-Boot>` **setenv bootargs "8250.nr_uarts=1 root=/dev/mmcblk0p2 rootwait console=ttyS0,115200n8"**\

`U-Boot>` **saveenv**\
Saving Environment to FAT... OK

`U-Boot>` **fatload mmc 0 \${kernel_addr_r} Image**\
22962688 bytes read in 1013 ms (21.6 MiB/s)

`U-Boot>` **fatload mmc 0 \${fdt_addr_r} bcm2711-rpi-4-b.dtb**\
54388 bytes read in 20 ms (2.6 MiB/s)

`U-Boot>` **booti \${kernel_addr_r} - \${fdt_addr_r}**\
Moving Image from 0x80000 to 0x200000, end=1900000



<pre>
U-Boot 2023.10-rc1-00188-g7755b22007 (Aug 03 2023 - 12:52:56 +0530)

DRAM:  948 MiB (effective 3.9 GiB)
RPI 4 Model B (0xc03115)
Core:  210 devices, 16 uclasses, devicetree: board
MMC:   mmcnr@7e300000: 1, mmc@7e340000: 0
Loading Environment from FAT... OK
In:    serial
Out:   serial
Err:   serial
Net:   eth0: ethernet@7d580000
PCIe BRCM: link up, 5.0 Gbps x1 (SSC)
starting USB...
Bus xhci_pci: Register 5000420 NbrPorts 5
Starting the controller
USB XHCI 1.00
scanning bus xhci_pci for devices... 2 USB Device(s) found
       scanning usb for storage devices... 0 Storage Device(s) found
Hit any key to stop autoboot:  0
U-Boot>
U-Boot> setenv bootargs "8250.nr_uarts=1 root=/dev/mmcblk0p2 rootwait console=ttyS0,115200n8"
U-Boot> saveenv
Saving Environment to FAT... OK
U-Boot> fatload mmc 0 ${kernel_addr_r} Image
22962688 bytes read in 1013 ms (21.6 MiB/s)
U-Boot> fatload mmc 0 ${fdt_addr_r} bcm2711-rpi-4-b.dtb
54388 bytes read in 20 ms (2.6 MiB/s)
U-Boot> booti ${kernel_addr_r} - ${fdt_addr_r}
Moving Image from 0x80000 to 0x200000, end=1900000
## Flattened Device Tree blob at 02600000
   Booting using the fdt blob at 0x2600000
Working FDT set to 2600000
   Using Device Tree in place at 0000000002600000, end 0000000002610473
Working FDT set to 2600000

Starting kernel ...

[    0.000000] Booting Linux on physical CPU 0x0000000000 [0x410fd083]
[    0.000000] Linux version 6.1.42-v8+ (prem@Ubuntu) (aarch64-linux-gnu-gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0, GNU ld (GNU Binutils for Ubuntu) 2.38) #2 SMP PREEMPT Fri Aug  4 11:29:58 IST 2023
[    0.000000] Machine model: Raspberry Pi 4 Model B Rev 1.5
[    0.000000] efi: UEFI not found.
[    0.000000] Reserved memory: created CMA memory pool at 0x000000002c000000, size 64 MiB
[    0.000000] OF: reserved mem: initialized node linux,cma, compatible id shared-dma-pool
[    0.000000] Zone ranges:
[    0.000000]   DMA      [mem 0x0000000000000000-0x000000003fffffff]
[    0.000000]   DMA32    [mem 0x0000000040000000-0x00000000fbffffff]
[    0.000000]   Normal   empty
[    0.000000] Movable zone start for each node
[    0.000000] Early memory node ranges
[    0.000000]   node   0: [mem 0x0000000000000000-0x000000003b3fffff]
[    0.000000]   node   0: [mem 0x0000000040000000-0x00000000fbffffff]
[    0.000000] Initmem setup node 0 [mem 0x0000000000000000-0x00000000fbffffff]
[    0.000000] On node 0, zone DMA32: 19456 pages in unavailable ranges
[    0.000000] On node 0, zone DMA32: 16384 pages in unavailable ranges
[    0.000000] percpu: Embedded 29 pages/cpu s79016 r8192 d31576 u118784
[    0.000000] Detected PIPT I-cache on CPU0
[    0.000000] CPU features: detected: Spectre-v2
[    0.000000] CPU features: detected: Spectre-v3a
[    0.000000] CPU features: detected: Spectre-v4
[    0.000000] CPU features: detected: Spectre-BHB
[    0.000000] CPU features: kernel page table isolation forced ON by KASLR
[    0.000000] CPU features: detected: Kernel page table isolation (KPTI)
[    0.000000] CPU features: detected: ARM erratum 1742098
[    0.000000] CPU features: detected: ARM errata 1165522, 1319367, or 1530923
[    0.000000] alternatives: applying boot alternatives
[    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 996912
[    0.000000] Kernel command line: 8250.nr_uarts=1 root=/dev/mmcblk0p2 rootwait console=ttyS0,115200n8
[    0.000000] Dentry cache hash table entries: 524288 (order: 10, 4194304 bytes, linear)
[    0.000000] Inode-cache hash table entries: 262144 (order: 9, 2097152 bytes, linear)
[    0.000000] mem auto-init: stack:off, heap alloc:off, heap free:off
[    0.000000] software IO TLB: area num 4.
[    0.000000] software IO TLB: mapped [mem 0x0000000037400000-0x000000003b400000] (64MB)
[    0.000000] Memory: 3815116K/4050944K available (12032K kernel code, 2136K rwdata, 4004K rodata, 4096K init, 1079K bss, 170292K reserved, 65536K cma-reserved)
[    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
[    0.000000] ftrace: allocating 38940 entries in 153 pages
[    0.000000] ftrace: allocated 153 pages with 4 groups
[    0.000000] trace event string verifier disabled
[    0.000000] rcu: Preemptible hierarchical RCU implementation.
[    0.000000] rcu:     RCU event tracing is enabled.
[    0.000000] rcu:     RCU restricting CPUs from NR_CPUS=256 to nr_cpu_ids=4.
[    0.000000]  Trampoline variant of Tasks RCU enabled.
[    0.000000]  Rude variant of Tasks RCU enabled.
[    0.000000]  Tracing variant of Tasks RCU enabled.
[    0.000000] rcu: RCU calculated value of scheduler-enlistment delay is 25 jiffies.
[    0.000000] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
[    0.000000] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
[    0.000000] Root IRQ handler: gic_handle_irq
[    0.000000] GIC: Using split EOI/Deactivate mode
[    0.000000] rcu: srcu_init: Setting srcu_struct sizes based on contention.
[    0.000000] arch_timer: cp15 timer(s) running at 54.00MHz (phys).
[    0.000000] clocksource: arch_sys_counter: mask: 0xffffffffffffff max_cycles: 0xc743ce346, max_idle_ns: 440795203123 ns
[    0.000001] sched_clock: 56 bits at 54MHz, resolution 18ns, wraps every 4398046511102ns
[    0.000308] Console: colour dummy device 80x25
[    0.000378] Calibrating delay loop (skipped), value calculated using timer frequency.. 108.00 BogoMIPS (lpj=216000)
[    0.000402] pid_max: default: 32768 minimum: 301
[    0.000527] LSM: Security Framework initializing
[    0.000737] Mount-cache hash table entries: 8192 (order: 4, 65536 bytes, linear)
[    0.000796] Mountpoint-cache hash table entries: 8192 (order: 4, 65536 bytes, linear)
[    0.002071] cgroup: Disabling memory control group subsystem
[    0.004359] cblist_init_generic: Setting adjustable number of callback queues.
[    0.004373] cblist_init_generic: Setting shift to 2 and lim to 1.
[    0.004559] cblist_init_generic: Setting adjustable number of callback queues.
[    0.004570] cblist_init_generic: Setting shift to 2 and lim to 1.
[    0.004750] cblist_init_generic: Setting adjustable number of callback queues.
[    0.004762] cblist_init_generic: Setting shift to 2 and lim to 1.
[    0.005220] rcu: Hierarchical SRCU implementation.
[    0.005231] rcu:     Max phase no-delay instances is 1000.
[    0.006536] EFI services will not be available.
[    0.007082] smp: Bringing up secondary CPUs ...
[    0.008245] Detected PIPT I-cache on CPU1
[    0.008391] CPU1: Booted secondary processor 0x0000000001 [0x410fd083]
[    0.009557] Detected PIPT I-cache on CPU2
[    0.009672] CPU2: Booted secondary processor 0x0000000002 [0x410fd083]
[    0.010818] Detected PIPT I-cache on CPU3
[    0.010939] CPU3: Booted secondary processor 0x0000000003 [0x410fd083]
[    0.011098] smp: Brought up 1 node, 4 CPUs
[    0.011114] SMP: Total of 4 processors activated.
[    0.011125] CPU features: detected: 32-bit EL0 Support
[    0.011134] CPU features: detected: 32-bit EL1 Support
[    0.011146] CPU features: detected: CRC32 instructions
[    0.011270] CPU: All CPU(s) started at EL2
[    0.011303] alternatives: applying system-wide alternatives
[    0.013116] devtmpfs: initialized
[    0.024735] Enabled cp15_barrier support
[    0.024769] Enabled setend support
[    0.024983] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
[    0.025015] futex hash table entries: 1024 (order: 4, 65536 bytes, linear)
[    0.026615] pinctrl core: initialized pinctrl subsystem
[    0.027463] DMI not present or invalid.
[    0.028407] NET: Registered PF_NETLINK/PF_ROUTE protocol family
[    0.031928] DMA: preallocated 512 KiB GFP_KERNEL pool for atomic allocations
[    0.032163] DMA: preallocated 512 KiB GFP_KERNEL|GFP_DMA pool for atomic allocations
[    0.032686] DMA: preallocated 512 KiB GFP_KERNEL|GFP_DMA32 pool for atomic allocations
[    0.032778] audit: initializing netlink subsys (disabled)
[    0.033050] audit: type=2000 audit(0.032:1): state=initialized audit_enabled=0 res=1
[    0.033613] thermal_sys: Registered thermal governor 'step_wise'
[    0.033697] cpuidle: using governor menu
[    0.033868] hw-breakpoint: found 6 breakpoint and 4 watchpoint registers.
[    0.034057] ASID allocator initialised with 32768 entries
[    0.034227] Serial: AMBA PL011 UART driver
[    0.044806] bcm2835-mbox fe00b880.mailbox: mailbox enabled
[    0.060224] raspberrypi-firmware soc:firmware: Attached to firmware from 2023-01-05T10:46:54, variant start
[    0.064239] raspberrypi-firmware soc:firmware: Firmware hash is 8ba17717fbcedd4c3b6d4bce7e50c7af4155cba9
[    0.078654] KASLR enabled
[    0.111164] bcm2835-dma fe007000.dma: DMA legacy API manager, dmachans=0x1
[    0.116782] SCSI subsystem initialized
[    0.117018] usbcore: registered new interface driver usbfs
[    0.117072] usbcore: registered new interface driver hub
[    0.117137] usbcore: registered new device driver usb
[    0.117460] usb_phy_generic phy: supply vcc not found, using dummy regulator
[    0.117678] usb_phy_generic phy: dummy supplies not allowed for exclusive requests
[    0.118002] pps_core: LinuxPPS API ver. 1 registered
[    0.118014] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
[    0.118038] PTP clock support registered
[    0.119101] vgaarb: loaded
[    0.119595] clocksource: Switched to clocksource arch_sys_counter
[    0.120245] VFS: Disk quotas dquot_6.6.0
[    0.120322] VFS: Dquot-cache hash table entries: 512 (order 0, 4096 bytes)
[    0.120506] FS-Cache: Loaded
[    0.120759] CacheFiles: Loaded
[    0.130643] NET: Registered PF_INET protocol family
[    0.131433] IP idents hash table entries: 65536 (order: 7, 524288 bytes, linear)
[    0.136398] tcp_listen_portaddr_hash hash table entries: 2048 (order: 3, 32768 bytes, linear)
[    0.136456] Table-perturb hash table entries: 65536 (order: 6, 262144 bytes, linear)
[    0.136483] TCP established hash table entries: 32768 (order: 6, 262144 bytes, linear)
[    0.136680] TCP bind hash table entries: 32768 (order: 8, 1048576 bytes, linear)
[    0.137635] TCP: Hash tables configured (established 32768 bind 32768)
[    0.138081] MPTCP token hash table entries: 4096 (order: 4, 98304 bytes, linear)
[    0.138283] UDP hash table entries: 2048 (order: 4, 65536 bytes, linear)
[    0.138346] UDP-Lite hash table entries: 2048 (order: 4, 65536 bytes, linear)
[    0.138666] NET: Registered PF_UNIX/PF_LOCAL protocol family
[    0.139474] RPC: Registered named UNIX socket transport module.
[    0.139489] RPC: Registered udp transport module.
[    0.139499] RPC: Registered tcp transport module.
[    0.139508] RPC: Registered tcp NFSv4.1 backchannel transport module.
[    0.139531] PCI: CLS 0 bytes, default 64
[    0.141976] hw perfevents: enabled with armv8_cortex_a72 PMU driver, 7 counters available
[    0.142344] kvm [1]: IPA Size Limit: 44 bits
[    0.143685] kvm [1]: vgic interrupt IRQ9
[    0.143916] kvm [1]: Hyp mode initialized successfully
[    1.320167] Initialise system trusted keyrings
[    1.320604] workingset: timestamp_bits=46 max_order=20 bucket_order=0
[    1.328324] zbud: loaded
[    1.331294] NFS: Registering the id_resolver key type
[    1.331336] Key type id_resolver registered
[    1.331346] Key type id_legacy registered
[    1.331459] nfs4filelayout_init: NFSv4 File Layout Driver Registering...
[    1.331474] nfs4flexfilelayout_init: NFSv4 Flexfile Layout Driver Registering...
[    1.332932] Key type asymmetric registered
[    1.332948] Asymmetric key parser 'x509' registered
[    1.333027] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 247)
[    1.333302] io scheduler mq-deadline registered
[    1.333318] io scheduler kyber registered
[    1.342550] brcm-pcie fd500000.pcie: host bridge /scb/pcie@7d500000 ranges:
[    1.342590] brcm-pcie fd500000.pcie:   No bus range found for /scb/pcie@7d500000, using [bus 00-ff]
[    1.342677] brcm-pcie fd500000.pcie:      MEM 0x0600000000..0x063fffffff -> 0x00c0000000
[    1.342762] brcm-pcie fd500000.pcie:   IB MEM 0x0000000000..0x00ffffffff -> 0x0400000000
[    1.343828] brcm-pcie fd500000.pcie: PCI host bridge to bus 0000:00
[    1.343846] pci_bus 0000:00: root bus resource [bus 00-ff]
[    1.343864] pci_bus 0000:00: root bus resource [mem 0x600000000-0x63fffffff] (bus address [0xc0000000-0xffffffff])
[    1.343938] pci 0000:00:00.0: [14e4:2711] type 01 class 0x060400
[    1.344195] pci 0000:00:00.0: PME# supported from D0 D3hot
[    1.348113] pci_bus 0000:01: supply vpcie3v3 not found, using dummy regulator
[    1.348301] pci_bus 0000:01: supply vpcie3v3aux not found, using dummy regulator
[    1.348400] pci_bus 0000:01: supply vpcie12v not found, using dummy regulator
[    1.413686] brcm-pcie fd500000.pcie: link up, 5.0 GT/s PCIe x1 (SSC)
[    1.413847] pci 0000:01:00.0: [1106:3483] type 00 class 0x0c0330
[    1.413973] pci 0000:01:00.0: reg 0x10: [mem 0x00000000-0x00000fff 64bit]
[    1.414450] pci 0000:01:00.0: PME# supported from D0 D3hot
[    1.415170] pci 0000:00:00.0: BAR 8: assigned [mem 0x600000000-0x6000fffff]
[    1.415195] pci 0000:01:00.0: BAR 0: assigned [mem 0x600000000-0x600000fff 64bit]
[    1.415237] pci 0000:00:00.0: PCI bridge to [bus 01]
[    1.415259] pci 0000:00:00.0:   bridge window [mem 0x600000000-0x6000fffff]
[    1.416279] bcm2708_fb soc:fb: Unable to determine number of FBs. Disabling driver.
[    1.416294] bcm2708_fb: probe of soc:fb failed with error -2
[    1.423066] Serial: 8250/16550 driver, 1 ports, IRQ sharing enabled
[    1.425974] iproc-rng200 fe104000.rng: hwrng registered
[    1.426434] vc-mem: phys_addr:0x00000000 mem_base=0x00000000 mem_size:0x00000000(0 MiB)
[    1.428008] gpiomem-bcm2835 fe200000.gpiomem: Initialised: Registers at 0xfe200000
[    1.440876] brd: module loaded
[    1.449823] loop: module loaded
[    1.450540] Loading iSCSI transport class v2.0-870.
[    1.455721] bcmgenet fd580000.ethernet: GENET 5.0 EPHY: 0x0000
[    1.515750] unimac-mdio unimac-mdio.-19: Broadcom UniMAC MDIO bus
[    1.516882] usbcore: registered new interface driver r8152
[    1.516949] usbcore: registered new interface driver lan78xx
[    1.517005] usbcore: registered new interface driver smsc95xx
[    1.563066] pci 0000:00:00.0: enabling device (0000 -> 0002)
[    1.563240] xhci_hcd 0000:01:00.0: xHCI Host Controller
[    1.563270] xhci_hcd 0000:01:00.0: new USB bus registered, assigned bus number 1
[    1.564033] xhci_hcd 0000:01:00.0: hcc params 0x002841eb hci version 0x100 quirks 0x0b00040000000890
[    1.565079] xhci_hcd 0000:01:00.0: xHCI Host Controller
[    1.565100] xhci_hcd 0000:01:00.0: new USB bus registered, assigned bus number 2
[    1.565121] xhci_hcd 0000:01:00.0: Host supports USB 3.0 SuperSpeed
[    1.565498] usb usb1: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 6.01
[    1.565518] usb usb1: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    1.565532] usb usb1: Product: xHCI Host Controller
[    1.565545] usb usb1: Manufacturer: Linux 6.1.42-v8+ xhci-hcd
[    1.565557] usb usb1: SerialNumber: 0000:01:00.0
[    1.566307] hub 1-0:1.0: USB hub found
[    1.566371] hub 1-0:1.0: 1 port detected
[    1.567318] usb usb2: New USB device found, idVendor=1d6b, idProduct=0003, bcdDevice= 6.01
[    1.567337] usb usb2: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    1.567351] usb usb2: Product: xHCI Host Controller
[    1.567363] usb usb2: Manufacturer: Linux 6.1.42-v8+ xhci-hcd
[    1.567375] usb usb2: SerialNumber: 0000:01:00.0
[    1.568117] hub 2-0:1.0: USB hub found
[    1.568208] hub 2-0:1.0: 4 ports detected
[    1.569842] dwc_otg: version 3.00a 10-AUG-2012 (platform bus)
[    1.570693] usbcore: registered new interface driver uas
[    1.570785] usbcore: registered new interface driver usb-storage
[    1.571006] mousedev: PS/2 mouse device common for all mice
[    1.576644] sdhci: Secure Digital Host Controller Interface driver
[    1.576658] sdhci: Copyright(c) Pierre Ossman
[    1.577196] sdhci-pltfm: SDHCI platform and OF driver helper
[    1.580632] ledtrig-cpu: registered to indicate activity on CPUs
[    1.580921] hid: raw HID events driver (C) Jiri Kosina
[    1.581107] usbcore: registered new interface driver usbhid
[    1.581119] usbhid: USB HID core driver
[    1.587750] NET: Registered PF_PACKET protocol family
[    1.587897] Key type dns_resolver registered
[    1.589162] registered taskstats version 1
[    1.589237] Loading compiled-in X.509 certificates
[    1.590142] Key type .fscrypt registered
[    1.590155] Key type fscrypt-provisioning registered
[    1.605716] uart-pl011 fe201000.serial: there is not valid maps for state default
[    1.606511] uart-pl011 fe201000.serial: cts_event_workaround enabled
[    1.606672] fe201000.serial: ttyAMA1 at MMIO 0xfe201000 (irq = 35, base_baud = 0) is a PL011 rev2
[    1.606935] serial serial0: tty port ttyAMA1 registered
[    1.615000] bcm2835-aux-uart fe215040.serial: there is not valid maps for state default
[    1.615956] printk: console [ttyS0] disabled
[    1.616068] fe215040.serial: ttyS0 at MMIO 0xfe215040 (irq = 36, base_baud = 62500000) is a 16550
[    1.827652] usb 1-1: new high-speed USB device number 2 using xhci_hcd
[    1.834274] printk: console [ttyS0] enabled
[    1.998259] usb 1-1: New USB device found, idVendor=2109, idProduct=3431, bcdDevice= 4.21
[    2.010833] bcm2835-wdt bcm2835-wdt: Broadcom BCM2835 watchdog timer
[    2.016733] usb 1-1: New USB device strings: Mfr=0, Product=1, SerialNumber=0
[    2.023356] bcm2835-power bcm2835-power: Broadcom BCM2835 power domains driver
[    2.030634] usb 1-1: Product: USB2.0 Hub
[    2.037837] mmc-bcm2835 fe300000.mmcnr: mmc_debug:0 mmc_debug2:0
[    2.045773] hub 1-1:1.0: USB hub found
[    2.050001] mmc-bcm2835 fe300000.mmcnr: DMA channel allocated
[    2.080925] of_cfs_init
[    2.085288] hub 1-1:1.0: 4 ports detected
[    2.092028] of_cfs_init: OK
[    2.120080] mmc0: SDHCI controller on fe340000.mmc [fe340000.mmc] using ADMA
[    3.111941] Waiting for root device /dev/mmcblk0p2...
[    3.198854] mmc1: new high speed SDIO card at address 0001
[    3.221519] mmc0: new ultra high speed DDR50 SDHC card at address 0007
[    3.229166] mmcblk0: mmc0:0007 SD16G 14.4 GiB
[    3.237264]  mmcblk0: p1 p2
[    3.240727] mmcblk0: mmc0:0007 SD16G 14.4 GiB
[    3.276150] EXT4-fs (mmcblk0p2): INFO: recovery required on readonly filesystem
[    3.283654] EXT4-fs (mmcblk0p2): write access will be enabled during recovery
[    3.343193] EXT4-fs (mmcblk0p2): recovery complete
[    3.609862] EXT4-fs (mmcblk0p2): mounted filesystem with ordered data mode. Quota mode: none.
[    3.618614] VFS: Mounted root (ext4 filesystem) readonly on device 179:2.
[    3.628579] devtmpfs: mounted
[    3.640957] Freeing unused kernel memory: 4096K
[    3.645785] Run /sbin/init as init process
[    3.874997] EXT4-fs (mmcblk0p2): re-mounted. Quota mode: none.

Please press Enter to activate this console.
~ #
~ # echo "Hello Prem"
Hello Prem
~ #</pre>
