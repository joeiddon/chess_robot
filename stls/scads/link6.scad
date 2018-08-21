//general
$fn = 30;                  //convexity of general curved faces
thickness = 3.9;             //thickness of arm

//circular servo horn measurements
hornDiameter = 21;             //diameter of the circular horn

//arm specific stuff
armLength =  155;               //horn center to horn center distance for arm length
leverLength = 39.4;             //length of lever bit on end of arm
supportHeight = 35;             //height between the supporting beam and the other beam going to the gripper

pivotDiameter = 10;            //diameter of pivoting end
freeTurning3mm = 3.4;          //diameter of hole for free turning motion

difference(){
   hull(){
      cylinder(d=hornDiameter,h=thickness, $fn = 60);
      translate([0,leverLength,0])
      cylinder(d=pivotDiameter,h=thickness, $fn = 60);
      translate([supportHeight, 0, 0])
      cylinder(d=pivotDiameter, h=thickness, $fn = 60);
   }
   cylinder(d=freeTurning3mm,h=thickness);
   translate([0,leverLength,0])
   cylinder(d=freeTurning3mm,h=thickness);
   translate([supportHeight, 0,0])
   cylinder(d=freeTurning3mm,h=thickness);
}