//general
$fn = 30;                  //convexity of general curved faces
thickness = 3.9;             //thickness of arm

//circular servo horn measurements
hornDiameter = 21;             //diameter of the circular horn
hornCenterHole = 9;          //diameter of the center hole (for screw)
hornOuterHoleDiameters = 1.9;  //diameter of the outer holes for screwing down horn
hornOuterHolesCenterDist = 7.8;//distance from center to the outer holes

//arm specific stuff
armLength = 200;               //horn center to horn center distance for arm length

freeTurning3mm = 3.4;          //diameter of hole for free turning motion

module horn(){
   cylinder(d=hornCenterHole, h=thickness);
   translate([hornOuterHolesCenterDist, 0, 0])
   cylinder(d=hornOuterHoleDiameters, h=thickness);
   translate([-hornOuterHolesCenterDist, 0, 0])
   cylinder(d=hornOuterHoleDiameters, h=thickness);
}

difference(){
   hull(){
      cylinder(d=hornDiameter,h=thickness, $fn = 60);
      translate([0,armLength,0])
      cylinder(d=hornDiameter,h=thickness, $fn = 60);
   }
   cylinder(d=freeTurning3mm,h=thickness);
   translate([0,armLength,0])
   cylinder(d=freeTurning3mm,h=thickness);
}