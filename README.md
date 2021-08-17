# OPAL

Minimalistic FW for Processing GCode commands to xy2-100 protocol and control periferal devices.
The default usecase is a 3D printer/engraver.

This project began long time ago but since its code might be useful to others it is published here.
It started as a fun exercise to control mirrors glued to a harddrives reading arm. While his was a
fun DIY hack, hopes where to learn more about how laser scanners work and what uses they might have.

Since this has grown into what it is today. 

HW:
The current code is intended to run on a Teensy 4.0 but might run on a 4.1. It will not run on a 3.6.
To be able to make use of the FW you will need an SN75174N wired to pins 22,19,17,14 on a Teensy 4.0.
The output fron the SN75174N goes directly to your digital XY2-100 galvo.

FW:
Is what you find in this repo. 

IT IS A PRE ALPHA VERSION NOT READY FOR MAINSTREAM USE!
All USE IT AT YOUR OWN RISK

DO NOT USE THIS UNLESS YOU ARE AWARE OF THE RISKS!
- YOUR TEENSY BOARD MIGHT BREAK
- YOUR GALVO MIGHT BE DESTROYED
- YOUR LASER MIGHT BE DESTROYED
- YOUR LASER MIGHT DESTROY SOMETHING NEARBY
- YOUR GALVO MIGHT EXPERIENCE UNEXPECTED BEHAVIOUR
- DO NOT USE THIS WITH OTHER LASERS THAN LASERPOINTERS
- WEAR SAFETYGLASSES INTENDED FOR THE LASER YOU USE
- YOU MIGHT GET BLIND

Always work SAFE and in an contained environment. The FW might crash at any time leaving your galvo and laser in an unknown state!

The firmware has features to control:

- a XY2-100 galvo (SinoGalvo SG7110 used for testing)
- a Synrad series 48 Laser
- an SSR to control your lasers powersource

Future plans include stepper integration and more I/O for SSRs but atm. stabillity is the main focus.

Fun fact: OPAL refers to OPengALvo and the name is inspired by the sparks laser engraving does and their resemblance to the fire opal.

