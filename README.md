# Userspace I/O Driver for PCITTL32IO

This repository contains a Linux Driver using the userspace I/O (UIO) subsystem of the Linux Kernel for the Quancom PCITTL32IO card. 

## Quancom PCITTL32IO card

The Quancom PCITTL32IO card (Vendor ID: 0x8008, Device ID: 0x3301) is a PCI card which gives you access to 32 I/O Pins. The device uses the legacy PCI Interrupt to signalize a falling edge on a configurable pin as an interrupt. To control the card, it offers 256 bytes of memory mapped IOs. Here is a table with the hardware registers of the card:

| Offset                                   | Register        |                                                                  |                        |
|------------------------------------------|-----------------|------------------------------------------------------------------|------------------------|
| 0xfc                                     | TTL I/O 1...8   |                                                                  |                        |
| 0xfd                                     | TTL I/O 9...16  |                                                                  |                        |
| 0xfe                                     | TTL I/O 17...24 |                                                                  |                        |
| 0xff                                     | TTL I/O 25...32 |                                                                  |                        |
| Setting the direction of a TTL I/O group |                 |                                                                  |                        |
|                                          | Bit             | TTL I/O                                                          | Description            |
| 0xf8                                     | 0               | 1 – 8                                                            | 0 = input / 1 = output |
|                                          | 1               | 9 – 16                                                           | 0 = input / 1 = output |
|                                          | 2               | 17 – 24                                                          | 0 = input / 1 = output |
|                                          | 3               | 25 – 32                                                          | 0 = input / 1 = output |
| Setting up IRQ-generation                |                 |                                                                  |                        |
|                                          | Bit             | Description                                                      |                        |
| 0xf9                                     | 0               | Interrupt enable (0 = disabled / 1 = enabled)                    |                        |
|                                          | 1 – 4           | IRQ-Source TTL-I/O 1..16 (bit-coded, only one source selectable) |                        |

## Playlist

Here is a link to my [playlist](https://www.youtube.com/watch?v=GBq9rUqwQRI&list=RDCMUCHkwlexIsoJVa5g_G9g18GA&start_radio=1).

## Support my work

If you want to support me, you can buy me a coffee [buymeacoffee.com/johannes4linux](https://www.buymeacoffee.com/johannes4linux).



