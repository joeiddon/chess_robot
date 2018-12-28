//general
$fn = 60;                              //convexity of general curved faces
thickness = 3.5;                       //thickness next to flat servo

servo_distance = 50;                   //distance between the inner of the two servo's pads

//servo pad
servo_hole_diameter = 2.86;            //diamter of holes for servo attachment
servo_hole_width = 10;                 //width of short distance between hole so same side of servo
servo_hole_length = 48;                //length of long distance between holes on either side of servo
servo_pad_width = 20;                  //width of servo pad
servo_pad_length = 54;                 //length of servo pad
servo_length = 41;                     //wideness (length) of servo
servo_pad_thickness = 8;               //thickness of servo pad
servo_width = servo_pad_length;        //width or thin side of the servo
servo_raise = 9.3;                     //raise of servos off ground

//holes that connect this to the bearing plate
fastening_holes_diameter = 3;          //diameter of them
fastening_holes_separation = 42;       //center to center separation distance

//cutout so can slide over stepper
cutout_length = 17;
cutout_width  = 4;
cutout_height = 8;

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
        cube([servo_pad_length, servo_pad_width + servo_raise, servo_pad_thickness]);
        translate([0, servo_raise, 0])
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
    translate([0, 0, thickness/2])
    cube([servo_pad_length, servo_distance + 2 * servo_pad_thickness, thickness], center=true);
    translate([0, servo_distance/-2, 0])
    servo_pad();
    translate([0, servo_distance/2 + servo_pad_thickness, 0])
    servo_pad();
}

difference(){
    body();
    //servo bit cutout
    translate([servo_length/-2, servo_distance/-2 - servo_pad_thickness - 1, thickness + servo_raise])
    cube([servo_length, servo_distance + 2 * servo_pad_thickness + 2, servo_pad_width + 2]); //the +2  just gets rid of a glitch
    //fastening holes
    translate([fastening_holes_separation/2, 0, 0])
    cylinder(d=fastening_holes_diameter, h=thickness);
    translate([-fastening_holes_separation/2, 0, 0])
    cylinder(d=fastening_holes_diameter, h=thickness);
    //servo cutout
    translate([servo_pad_length/2 - cutout_length, -servo_distance/2-2*cutout_width, 0])
    cube([cutout_length, cutout_width, cutout_height]);
}

translate([servo_pad_length/2 - (servo_pad_length - servo_length) / 2 ,servo_distance/2,thickness+servo_pad_width+servo_raise])
hinge_section();