
PCB Characteristics
===================

This chapter will describe the Cynara Viride PCB and its properties and characteristics

----

Characteristics
----------
The Cynara Viride PCB has dimensions of 160 x 100 milimeters and consists out of 4 layers.
This has to do with the fact that the Cynara Viride design is compliant with an EAGLE standard schematic + layout license.

The Cynara Viride uses a standardised build-up with the following propperties:

* Top Silkscreen
* L1 - Top Copper - 35 um/ 1 oz 
* Prepreg - PR7628 - 360 um/xx mil 
* L2 - Inner Copper (GND) - 35 um/ 1 oz
* Core - FR4 - 710 um/xx mil
* L3 - Inner Copper (Power + signal) - 35 um/ 1 oz
* Prepreg - PR7628 - 360 um/xx mil 
* L4 - Bottom Copper - 35 um/ 1 oz 
* Bottom Silkscreen

Resulting in a PCB thickness of 1.5 mm.

Preferrably the PCB should be finished with ENIG, but HaSL Lead-Free will also function fine.

Technology
----------

For the Cynara Viride a technology clas has been choosen for the design to comply with.
In this case the choosen pattern class is: Class 6
This choosen drill class is: Class C

*The Cynara Viride design has all the setting already loaded into the PCB file. However, a separate EAGLE design-rule file is accessable.*

Impedances
----------

Usage of the earlier mentioned PCB build-up is not mandatory, however.
The Cynara Viride desing has impedance matched nets in the design.

These nets are:

* USB (FS) - differential impedance of 90 ohm.
* Ethernet (100 mbps) - differential impedance of 100 ohm.


### Characteristics USB Traces

To obtain the impedance of 90 ohms differential the traces of the USB bus need a spacing of 0.15mm/ mil over the full length.

The individual trace width of the USB_N and USB_P should be 0.58 mm/ xx mil.

The traces of USB_N and USB_P should have approximatly the same length.

### Characteristics Ethernet Traces

To obtain the impedance of 100 ohms differential the coupled traces of the ethernet bus need a spacing of 0.15mm/ mil over the full lenght.

The TX and RX pairs need to be separated with a GND trace.

The individual trace width of the ETH_RX_P, ETH_RX_N, ETH_TX_P and ETH_RX_N should be 0.46 mm/ xx mil.

**The length of the ethernet traces mentioned above should be all the same length.**










