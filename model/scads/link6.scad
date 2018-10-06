//general
$fn = 30;                  //convexity of general curved faces
thickness = 3.9;             //thickness of arm

//circular servo horn measurements
horn_diameter = 21;             //diameter of the circular horn

//arm specific stuff
lever_length = 39.4;             //length of lever bit on end of arm
support_height = 35;             //height between the supporting beam and the other beam going to the gripper

pivot_diameter = 10;            //diameter of pivoting end
free_turning3mm = 3.4;          //diameter of hole for free turning motion

difference(){
   hull(){
      cylinder(d=horn_diameter,h=thickness, $fn = 60);
      translate([0,lever_length,0])
      cylinder(d=pivot_diameter,h=thickness, $fn = 60);
      translate([support_height, 0, 0])
      cylinder(d=pivot_diameter, h=thickness, $fn = 60);
   }
   cylinder(d=free_turning3mm,h=thickness);
   translate([0,lever_length,0])
   cylinder(d=free_turning3mm,h=thickness);
   translate([support_height, 0,0])
   cylinder(d=free_turning3mm,h=thickness);
}