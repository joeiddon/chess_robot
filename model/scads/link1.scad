//general
$fn = 30;                  //convexity of general curved faces
thickness = 3.9;             //thickness of arm

//circular servo horn measurements
horn_diameter = 21;             //diameter of the circular horn
horn_center_hole = 9;          //diameter of the center hole (for screw)
horn_outer_hole_diameters = 1.9;  //diameter of the outer holes for screwing down horn
horn_outer_holes_center_dist = 7.8;//distance from center to the outer holes

//arm specific stuff
arm_length = 200;//155;               //horn center to horn center distance for arm length

free_turning3mm = 3.4;          //diameter of hole for free turning motion

module horn(){
   cylinder(d=horn_center_hole, h=thickness);
   translate([horn_outer_holes_center_dist, 0, 0])
   cylinder(d=horn_outer_hole_diameters, h=thickness);
   translate([-horn_outer_holes_center_dist, 0, 0])
   cylinder(d=horn_outer_hole_diameters, h=thickness);
}

difference(){
   hull(){
      cylinder(d=horn_diameter,h=thickness, $fn = 60);
      translate([0,arm_length,0])
      cylinder(d=horn_diameter,h=thickness, $fn = 60);
   }
   horn();
   translate([0,arm_length,0])
   cylinder(d=free_turning3mm, h=thickness);
}