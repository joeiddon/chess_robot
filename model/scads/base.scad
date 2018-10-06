//general
$fn = 30;                        //convexity of general curved faces
thickness = 5;                   //thickness of the flat parts
length = 54;

//bottom plate (feet)
plate_hole_diameter = 4;            //diameter of holes in metal bottom plate
plate_hole_width = 19;             //shortest distance between the pairs of holes in plate
plate_hole_length = 44;            //long distance between the pairs of holes in botto
plate_feet_width = 20;             //width of the pads that connect the short distanced screws
plate_feet_length = length;            //length of the feet
//servo pad
servo_hole_diameter = 3;           //diamter of holes for servo attachment
servo_hole_width = 10;             //width of short distance between hole so same side of servo
servo_hole_length = 48;            //length of long distance between holes on either side of servo
servo_pad_width = 18;              //width of servo pad
servo_pad_length = length;         //length of servo pad
servo_pad_height = 25;             //height of servo pad
servo_length = 41;

module foot(){
   translate([0, plate_feet_width/-2, 0])
   difference(){
      cube([plate_feet_length, plate_feet_width, thickness]);
      translate([plate_feet_length/2 - plate_hole_width/2, plate_feet_width/2, 0])
      cylinder(h=thickness, d=plate_hole_diameter);
      translate([plate_feet_length/2 + plate_hole_width/2, plate_feet_width/2, 0])
      cylinder(h=thickness, d=plate_hole_diameter);
   }
}


module servo_pad(){
   difference(){
      cube([servo_pad_length, servo_pad_width, thickness]);
      translate([servo_pad_length/2 - servo_hole_length/2, servo_pad_width/2 - servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = thickness);
      translate([servo_pad_length/2 - servo_hole_length/2, servo_pad_width/2 + servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = thickness);
      translate([servo_pad_length/2 + servo_hole_length/2, servo_pad_width/2 - servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = thickness);
      translate([servo_pad_length/2 + servo_hole_length/2, servo_pad_width/2 + servo_hole_width/2, 0])
      cylinder(d = servo_hole_diameter, h = thickness);
   }   
}

module wall(){
   hull(){
      translate([0,plate_feet_width/-2, 0])
      cube([plate_feet_length, plate_feet_width, thickness]);
      translate([0,plate_hole_length/2 - servo_pad_width/2, servo_pad_height])
      cube([servo_pad_length, servo_pad_width, thickness]);
   }  
}

module base(){
    difference(){
       union(){
          difference(){
             union(){
                wall();
                translate([0, plate_hole_length, 0])
                mirror([0,1,0])
                wall();
             }
             translate([0, plate_hole_length/2 - servo_pad_width/2, servo_pad_height])
             cube([servo_pad_length, servo_pad_width, thickness]);
          }
          translate([0, plate_hole_length/2 - servo_pad_width/2, servo_pad_height])
          servo_pad();
       }
       translate([(length - servo_length) / 2 ,-0.5 * plate_hole_length, 0])
       cube([servo_length, plate_hole_length*2, servo_pad_height * 2]);   
    }

    foot();
    translate([0, plate_hole_length, 0])
    foot();
}

base();