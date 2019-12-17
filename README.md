# ReadResolver

[Arduino 1.8.10]

Ang_Vel_Fin.ino file provides the full descriptive walk-through of reading Angle and Velocity values of a motor via the PGA411Q1-EVM board with an Arduino uno board. The values are read via Serial Peripheral Interface (SPI) communication protocol. PGA411Q1-EVM is an evaluation board by Texas Instruments which uses PGA411Q1 resolver to digital converter. This converter reads the Sin,Cosine signals coming out of the resolver interface and outputs the Angle and the Velocity of the motor. Here I have used the HVH250 Three phase permanant magnet motor. All the datasheets, resources and the evaluation board GUI are available in the TI website. 

In this Arduino file I have used the 12bit data aquisition method and I have set the config registers with their default values. These can be adjusted. I have also used sample codes found in the [Software Developer Guide for PGA411-Q1](http://www.ti.com/lit/an/slaa708/slaa708.pdf) and have also used some of the code found in TIDA-00796 written by Jiri Panacek.

[pga411-q1 Resolver-to-digital converter with integrated exciter amplifier and power supply](http://www.ti.com/product/PGA411-Q1/) is used widely in Electronic automobile industry in order to monitor the Angle/Velocity of the motor.

I spent several weeks trying to find some way to read the values from an Arduino board but didn't find any online resources. This is my final working code and I wish anyone who is struggling to find something remotely closer to this may find this. 



<br/>
<br/>
<br/>
<br/>

Ranitha Mataraarachchi, <br/>
Room No: 2234, <br/>
Akbar-Nell Hall, <br/>
Faculty of Engineering, <br/>
University of Peradeniya, <br/>
Peradeniya, Sri Lanka.

(+94)777722662 </br>
ranitha@ieee.org <br/>
[Facebook](https://www.facebook.com/1994ranitha) | [LinkedIn](https://www.linkedin.com/in/ranitha/)

