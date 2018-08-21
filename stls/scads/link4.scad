//general
$fn = 30;                  //convexity of general curved faces
thickness = 3.9;             //thickness of arm

//arm specific stuff
armLength =  200;//155;               //horn center to horn center distance for arm length

pivotDiameter = 10;            //diameter of pivoting end
freeTurning3mm = 3.4;          //diameter of hole for free turning motion

difference(){
   hull(){
      cylinder(d=pivotDiameter,h=thickness, $fn = 60);
      translate([0,armLength,0])
      cylinder(d=pivotDiameter,h=thickness, $fn = 60);
   }
   cylinder(d=freeTurning3mm,h=thickness);
   translate([0,armLength,0])
   cylinder(d=freeTurning3mm,h=thickness);
}