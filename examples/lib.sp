
*************** MUX2 ***************
.SUBCKT MUX2 A B S Y VDD GND
M1 AN A VDD VDD pmos w=2u l=1u
M2 BN B VDD VDD pmos w=2u l=1u
M3 SN S VDD VDD pmos w=2u l=1u
M7 AN A GND GND nmos w=1u l=1u
M8 BN B GND GND nmos w=1u l=1u
M9 SN S GND GND nmos w=1u l=1u
.ENDS
****************************************

*************** NOR3 ***************
.SUBCKT NOR3 A B C Y VDD GND
M1 net1 A VDD VDD pmos w=2u l=1u
M2 net1 B net2 VDD pmos w=2u l=1u
M3 Y C net2 VDD pmos w=2u l=1u
M4 Y A GND GND nmos w=1u l=1u
M5 Y B GND GND nmos w=1u l=1u
M6 Y C GND GND nmos w=1u l=1u
.ENDS
****************************************

*************** NAND3 ***************
.SUBCKT NAND3 A B C Y VDD GND
M1 Y A VDD VDD pmos w=2u l=1u
M2 Y B VDD VDD pmos w=2u l=1u
M3 Y C VDD VDD pmos w=2u l=1u
M4 net1 A GND GND nmos w=1u l=1u
M5 net1 B net2 GND nmos w=1u l=1u
M6 Y C net2 GND nmos w=1u l=1u
.ENDS
****************************************

*************** NOR2 ***************
.SUBCKT NOR2 A B Y VDD GND
M1 net A VDD VDD pmos w=2u l=1u
M2 Y B net VDD pmos w=2u l=1u
M3 Y A GND GND nmos w=1u l=1u
M4 Y B GND GND nmos w=1u l=1u
.ENDS
****************************************

*************** NAND2 ***************
.SUBCKT NAND2 A B Y VDD GND
M1 Y A VDD VDD pmos w=2u l=1u
M2 Y B VDD VDD pmos w=2u l=1u
M3 net A GND GND nmos w=1u l=1u
M4 Y B net GND nmos w=1u l=1u
.ENDS
****************************************

*************** Inverter ***************
.SUBCKT INVERTER A AN VDD GND
M1 AN A VDD VDD pmos w=2u l=1u
M2 AN A GND GND nmos w=1u l=1u
.ENDS
****************************************
