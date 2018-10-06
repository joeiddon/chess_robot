//general
$fn = 30;                      //convexity of general curved faces
thickness = 3.5;                 //thickness next to flat servo

servo_distance = 50;             //distance between the inner of the two servo's pads

//circle servo horn measurements
horn_center_hole = 7;            //diameter of the center hole (for screw)
horn_outer_hole_diameters = 1.9;  //diameter of the outer holes for screwing down horn
horn_outer_holes_center_dist = 7.8;//distance from center to the outer holes
horn_dist = 5;                  //distance of horn from wall against servo pad

//servo pad
servo_hole_diameter = 2.87;           //diamter of holes for servo attachment
servo_hole_width = 10;             //width of short distance between hole so same side of servo
servo_hole_length = 48;            //length of long distance between holes on either side of servo
servo_pad_width = 20;              //width of servo pad
servo_pad_length = 54;             //length of servo pad
servo_length = 41;                //wideness (length) of servo
servo_pad_thickness = 8;           //thickness of servo pad
servo_width = servo_pad_length;     //width or thin side of the servo

//arm cutout
cutout_width = 30;        //width of cutout
cutout_depth = 1.5;         //depth of cutout

module horn(){
   cylinder(d=horn_center_hole, h=thickness);
   translate([horn_outer_holes_center_dist, 0, 0])
   cylinder(d=horn_outer_hole_diameters, h=thickness);
   translate([-horn_outer_holes_center_dist, 0, 0])
   cylinder(d=horn_outer_hole_diameters, h=thickness);
}

module servo_holes(){
   translate([servo_pad_length/2 - servo_hole_length/2, servo_pad_width/2 - servo_hole_width/2, 0])
   cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
   translate([servo_pad_length/2 - servo_hole_length/2, servo_pad_width/2 + servo_hole_width/2, 0])
   cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
   translate([servo_pad_length/2 + servo_hole_length/2, servo_pad_width/2 - servo_hole_width/2, 0])
   cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
   translate([servo_pad_length/2 + servo_hole_length/2, servo_pad_width/2 + servo_hole_width/2, 0])
   cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
}


module servo_pad(){
   translate([servo_pad_length/-2,0,thickness])
   rotate([90,0,0])
   difference(){
      cube([servo_pad_length, servo_pad_width, servo_pad_thickness]);
      servo_holes();
   }
}

module hinge_section(){
    //hole is servo_pad_width/2 height
   difference(){
      cube([(servo_pad_length - servo_length) / 2, servo_pad_thickness, servo_pad_width/2 + (servo_pad_length - servo_length) / 4]);
      translate([(servo_pad_length - servo_length) / 4, 0, servo_pad_width / 2])
      rotate([-90,0,0])
      cylinder(d = servo_hole_diameter, h=servo_pad_thickness);
   }
}

module body(){
   translate([servo_pad_length/-2, servo_distance/-2 - servo_pad_thickness, 0])
   cube([servo_pad_length, servo_distance + 2 * servo_pad_thickness, thickness]);
   translate([0, servo_distance/-2, 0])
   servo_pad();
   translate([0, servo_distance/2 + servo_pad_thickness, 0])
   servo_pad();
}

difference(){
   body();
   //servos
   translate([servo_length/-2, servo_distance/-2 - servo_pad_thickness - 1, thickness])
   cube([servo_length, servo_distance + 2 * servo_pad_thickness + 2, servo_pad_width + 2]);
   //cutout
   translate([servo_length/-2, cutout_width / -2, thickness - cutout_depth])
   cube([servo_length/2 - 1.5, cutout_width, cutout_depth]);
   
   rotate(90)
   horn();
   
}
translate([servo_pad_length/2 - (servo_pad_length - servo_length) / 2 ,servo_distance/2,thickness+servo_pad_width])
hinge_section();

/*
difference(){
   hulled_body();
   
   union(){
      translate([-0.5*servo_length,-0.5*horn_diameter,thickness])
      cube([servo_length, horn_diameter, servo_pad_width+10]);
      
      translate([-0.5*servo_width, -0.5*horn_diameter, thickness])
      cube([servo_width, 0.5*horn_diameter-servo_pad_thickness, servo_pad_width+10]);
      
      translate([-0.5 * servo_pad_length, -0.1, thickness])
      rotate([90,0,0])
      servo_pad();
      
      horn();
   }
}   


//servo_pad();
/*
module horn(){
   difference(){
      cylinder(d=horn_diameter, h=thickness, $fn = 60);
      cylinder(d=horn_center_hole, h=thickness);
      translate([horn_outer_holes_center_dist, 0, 0])
      cylinder(d=horn_outer_hole_diameters, h=thickness);
      translate([-horn_outer_holes_center_dist, 0, 0])
      cylinder(d=horn_outer_hole_diameters, h=thickness);
   }   
}

module servo_pad(){
   difference(){
      cube([servo_pad_length, servo_pad_width, servo_pad_thickness]);
      translate([servo_pad_length/2 - servo_hole_length/2, servo_pad_width/2 - servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
      translate([servo_pad_length/2 - servo_hole_length/2, servo_pad_width/2 + servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
      translate([servo_pad_length/2 + servo_hole_length/2, servo_pad_width/2 - servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
      translate([servo_pad_length/2 + servo_hole_length/2, servo_pad_width/2 + servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = servo_pad_thickness);
   }  
}
*/