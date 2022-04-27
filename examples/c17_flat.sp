********c17.sp*** ISCAS'85 benchmark*******
**************SPICE*********************

****************************************
.SUBCKT c17 I1 I2 I3 I6 I7 O22 O23 VDD GND
M1 net10 I1 VDD VDD pmos w=2u l=1u
M10 O23 net16 VDD VDD pmos w=2u l=1u
M11 net19 I7 VDD VDD pmos w=2u l=1u
M12 O23 net19 VDD VDD pmos w=2u l=1u
M13 net1 I1 GND GND nmos w=1u l=1u
M14 net10 I3 net1 GND nmos w=1u l=1u
M15 net5 net10 GND GND nmos w=1u l=1u
M16 net2 I3 GND GND nmos w=1u l=1u
M17 O22 net16 net5 GND nmos w=1u l=1u
M18 net11 I6 net2 GND nmos w=1u l=1u
M19 net16 net11 net3 GND nmos w=1u l=1u
M2 net10 I3 VDD VDD pmos w=2u l=1u
M20 net4 net11 GND GND nmos w=1u l=1u
M21 net3 I2 GND GND nmos w=1u l=1u
M22 net6 net16 GND GND nmos w=1u l=1u
M23 net19 I7 net4 GND nmos w=1u l=1u
M24 O23 net19 net6 GND nmos w=1u l=1u
M3 O22 net10 VDD VDD pmos w=2u l=1u
M4 net11 I3 VDD VDD pmos w=2u l=1u
M5 O22 net16 VDD VDD pmos w=2u l=1u
M6 net11 I6 VDD VDD pmos w=2u l=1u
M7 net16 net11 VDD VDD pmos w=2u l=1u
M8 net19 net11 VDD VDD pmos w=2u l=1u
M9 net16 I2 VDD VDD pmos w=2u l=1u
.ENDS
****************************************
