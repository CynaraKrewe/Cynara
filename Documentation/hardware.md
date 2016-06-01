---

# Hardware

## Overview

Overview:

<object type="image/svg+xml" data="../Pictures/CynaraOverview.svg">GitHub doesn't render <a href= "../Pictures/CynaraOverview.svg">CynaraOverview.svg</a>. Clone the repository and create the offline documentation using make.</object>

Petal pin description:

<object type="image/svg+xml" data="../Pictures/CynaraPinDescription.svg">GitHub doesn't render <a href= "../Pictures/CynaraPinDescription.svg">CynaraPinDescription.svg</a>. Clone the repository and create the offline documentation using make.</object>

Microcontroller-FPGA interconnect description:

<object type="image/svg+xml" data="../Pictures/CynaraInterconnectDescription.svg">GitHub doesn't render <a href= "../Pictures/CynaraInterconnectDescription.svg">CynaraInterconnectDescription.svg</a>. Clone the repository and create the offline documentation using make.</object>

## Mechanical drawings

| Part | Dimensions (L x W x H) |
| --- | ---:|
| Carrier PCB | 160,00 x 100,00 x 1,50 mm |
| Carrier | 160,00 x 100,00 x ?? mm |
| Petal | ?? x 52,00 x 1,50 mm |

## Design decisions

### Microcontroller-FPGA interconnect

#### External Peripheral Bus (EPI)

As per [System Design Guidelines for the TM4C129x Family of Tiva™ C Series Microcontrollers](http://www.ti.com/lit/an/spma056/spma056.pdf) the EPI0S31 signal is given extra space to prevent crosstalk. 

Lengths and delta lengths from EPIS031 of the individual traces can be found in the following graph:

<object type="image/svg+xml" data="../Pictures/EpiTraceLength.svg">GitHub doesn't render <a href= "../Pictures/EpiTraceLength.svg">EpiTraceLength.svg</a>. Clone the repository and create the offline documentation using make.</object>

#### Protection

Placeholders for series resistors can be used to protect the pins of the microcontroller and FPGA.

### Petal

#### Connector

The [SAMTEC MB1 Mini Edge Card Socket](https://www.samtec.com/products/mb1) reduces the cost of the Petals. 
Petals only need to provide plain pads, no mating connector.
The 50 pins variant is chosen to provide a proper amount of space on the Petals.

34 pins 2.54 mm pitch headers provide cheaper alternative to the SAMTEC MB1 socket. 
Keep in mind that a mating header is needed on the Petals in this case, adding to Petal cost.

#### Protection

Placeholders for series resistors can be used to protect the pins of the microcontroller and FPGA.

## PCB Characteristics

This chapter describes the properties and characteristics of the Cynara Viride Carrier PCB (Carrier PCB).

### Characteristics

The dimensions of the Carrier PCB are 160 x 100 millimeters and 4 layers.
The design is compliant with the EAGLE Standard schematic + layout license.

The Carrier PCB uses a standard build-up with the following properties:

| Layer | Name | Material | Thickness |
| ---:| --- | --- | ---:|
|| Top Silkscreen |||
| 1 | Top | Copper | 35 um / 1 oz | 
|| Prepreg | PR7628 | 360 um / 14 mil |
| 2 | Inner (GND) | Copper | 35 um / 1 oz |
|| Core | FR4 | 710 um / 28 mil |
| 3 | Inner (Power + signal) | Copper | 35 um / 1 oz |
|| Prepreg | PR7628 | 360 um / 14 mil |
| 4 | Bottom | Copper | 35 um / 1 oz |
|| Bottom Silkscreen |||

Resulting in a total Carrier PCB thickness of 1,5 mm.

The Carrier PCB should be finished with ENIG (preferred) or HaSL Lead-Free.

### Technology

The Carrier PCB complies with the following technology classes:

* Pattern Class 6
* Drill Class C

*The Carrier PCB design has all the setting already loaded into the PCB file. However, a separate EAGLE design-rule file is accessable.*

### Impedances

The Carrier PCB design includes impedance matched nets.

| Technology | Ethernet 100BASE-TX | USB 2.0 FullSpeed |
| --- | ---:| ---:|
| Z differential (target) | 100 Ohm | 90 Ohm |
| Z differential (actual) | 102,529 Ohm | 90,136 Ohm |
| Trace width | 0,3 mm | 0,37 mm |
| Trace spacing | 0,15 mm | 0,15 mm |
| Trace height | 0,36 mm | 0,36 mm |

For Ethernet 100BASE-TX the TX and RX pairs are separated with a GND trace.
All ethernet traces of both differential pairs are length matched.

For USB 2.0 FullSpeed the USB_N and USB_P are length matched.









