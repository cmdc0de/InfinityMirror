# InfinityMirror

## Materials you'll need:
* [Acrylic Mirror](https://www.amazon.com/Acrylic-Mirror-Sheet-Laser-Creations/dp/B01MXE417D/ref=sr_1_17?crid=3CSNSR02CE1KK&dchild=1&keywords=acrylic%2Bmirror%2Bsheet&qid=1609135905&s=home-garden&sprefix=acrylic%2Bmirror%2Cgarden%2C228&sr=1-17&th=1)
* [Acrylic see through Mirror](https://www.amazon.com/0-04-Acrylic-See-Through-Mirror-Transparent/dp/B07XTRC7F1?th=1)
* [ABS](https://www.raise3d.com/filaments/abs/) - I have a Raise 3D printer
* [22 Gauge Wire](https://www.amazon.com/gp/product/B00B4ZRPEY/ref=ppx_yo_dt_b_asin_title_o05_s02?ie=UTF8&psc=1)
* Solder 
* [Solder paste](https://www.amazon.com/Solder-Bi57-6-No-Clean-Lead-Free-Temperature/dp/B0195V1QEI/) - LOW TEMP solder
* Hot Glue
* [rechargable battery](https://www.amazon.com/Capacity-Rechargeable-Batteries-Flashlights-Headlamps/dp/B08MQDFQD4/)

## Equipment you'll need:
* 3d Printer
* laser cutter
* Solder Iron
* Hot Glue Gun

## Instructions
1. Mill/Etch/Order 20x Single-APA102c-breakout boards
2. Mill/Etch/Order 1x STM32G031J6M6 board
3. 3D Print: 3dprint/Base.stl, 3dprint/Base-bottom.stl, 3dprint/lid.stl, and 3dprint/octagon.stl
4. laser cut: laser-cutter/octagon v13.dxf 
5. Add components to the STM32G031J6M6 board.
6. Add componetns to 20x Single-APA102c-breakout boards - if you use an IR Oven make sure you use low temp solder!!! APA102c HATE 250 degress C!
7. Cut wires to length and connect all 20 Single-APA102c-breakout boards
8. Hot Glue apa102c breakouts into wholes of octogan - use a wire length of 3-4 inches for the first apa102c so you can feed it through the hole in the octagon and through the hole in the base to connect it to the STM32G031J6M6 baord.
9. Place acrylic mirror into bottom (it should fit via friction but I used a bit of CA glue as well).
10. place see through (2-way) mirror in the lid, again should fit via friction but I used a bit of CA glue as well.
11. Snap the lid on the octagon
12. Connect wires to STM32G031J6M6
13. Program the STM32G031J6M6 via J-LInk or ST-LInk connector and [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)
  1. Open project in firmware directory
  2. compile and program 
14. Hot Glue board to bottom-lid such that the STM32G031J6M6 board fits in the whole in the base.
15. Connect a rechargable 3.7-5V battery to the screw terminals 
16. Connect Base-bottom to base
