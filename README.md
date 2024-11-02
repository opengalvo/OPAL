# OPAL

Minimalistic FW for Processing GCode commands to xy2-100 protocol and control periferal devices using a Teensy board.
The default usecase is a 3D printer/engraver. Galvos driven by XY2-100 protocol are refered to as Digital Galvos and are
the subject of this repo.. Other options are Analog Galvos like the RC1001 and the various "Lasershow galvos" sold on e-bay.
Analog galvos are not covered here but might have their own section in the future. The last option is Steppermotor driven
galvos. More info on this can be found in GalvoStep here:  https://github.com/NiklasHammerstone/GalvoStep

If you are using the software please join: https://www.reddit.com/r/OpenGalvo/ and write a comment or share your project!

DEMO
Laser Marking - OpenGalvo - OPAL FW SG7110 Teensy 4.0
Slow:
  https://youtu.be/uMT-sZhYKJo
Fast:
  https://youtu.be/m6PLiacBdws


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

This firmware makes use of rlogiacco's CircularBuffer: https://github.com/rlogiacco/CircularBuffer/issues?q=label:question

Thanks to Les Lab - https://github.com/leswright1977 we have a PCB Design!
for more info: https://github.com/leswright1977/OPAL_PCB

Recently this SW has been used to 3D print metal! Awesome work @metalmatters!
https://www.youtube.com/watch?v=_SwsatEJn9k

Fun fact: OPAL refers to OPengALvo and the name is inspired by the sparks laser engraving does and their resemblance to the fire opal.

