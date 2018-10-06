$fn = 50;
locked3mm = 2.9;           //hole diameter for a locked/tight/screwed down 2mm screw
free_turning2mm = 2.5;      //hole diameter for a free turning 2mm screw
locked2mm  = 1.9;            //hole diameter for a locked/tight/screwed down 2mm
thickness = 2.7;           //thickness of the gears and beams etc.
gear_radius = 15;           //radius of the gears (cogs)
left_gear_hole_diameter = free_turning2mm; //diameter of hole in left gear
right_gear_hole_diameter = 2.1;   //attacing to servo sotiny bit > 2mm (shrinkage)
no_teeth  = 7;              //number of teeth on the gears
teeth_width = 4;            //width of gear teeth
teeth_bevel = 3;            //diameter of circles at end of teeth
teeth_height = 3.5;         //height of gear teeth
teeth_budge = 0.4;         //distances gears need to be appart to fit together
beam_widths = 4;           //width of all beams
beam1_width = beam_widths;    //width of the first beam
beam1_length = 33;          //length of the first beam
beam1_hole_diameter = free_turning2mm; //diameter of screw hole at end of beam
beam2_width = beam_widths;   //width of the second beam
beam2_length = beam1_length; //length of the second beam
beam2_hole_diameter = free_turning2mm;//diameter of screw holes at end of beam
beam2_center_dist = 10;       //translating beam2 from beam1 towards the centerline
beam2_forward_dist = 18;     //translatimg beam2 from beam1 in y dir towards the grabber end
servo_width = 12;           //width of servo
servo_length = 23;          //length of servo
servo_height = 10.8;        //height of servo bracket (not whole servo)
servo_wall_thick = 1.2;      //thickness of bracket
servo_shaft = 5.9;          //distance of shaft from closest lengthwise edge
gripper_arm_length = 24;     //length of nipper
gripper_arm_thickness = 5;   //thickness of end nippers
gripper_arm_pad_length = 16;  //length of the raised pad
gripper_arm_pad_thickness = 3;//thickness of the raised pad
gripper_arm_pad_height = 12;  //height of the raised pad
gripper_arm_width = beam_widths; //width of end nippers

gear_hole_dist = gear_radius * 2 - teeth_height + teeth_budge;

//////

arm_block_width = 15;        //width of block for connecting arms
arm_block_length = 5;         //length ""
arm_block_height = 9.9 + 5.5;         //height for screws to attach to connecting arms

support_arm_length = 5;      //length of block
support_arm_width = 15;       //widhth of block
support_arm_height = arm_block_height + 20;     //heighrt for screw to attach to the support arm
support_arm_distance = 32;   //distance from little block
support_arm_stength_width  = 4;  //width of support for support arm block
support_arm_stength_length = 2.5; //length ""


plate_width = 40.5;           //width of bottom plate
plate_length = 40 + locked3mm + arm_block_length/2;          //length of bottom palte
plate_bevel_back = 5;        //bevel radius for back corners of plate
plate_bevel_front = 10;      //bevel radius for front corners of plate


module gear(){
   cylinder(r=gear_radius - teeth_height, h = thickness);
   for (i = [0:180/(no_teeth-1):180]){
      rotate(i)
      hull(){
         translate([0,-0.5*teeth_width,0])
         cube([gear_radius - teeth_height, teeth_width, thickness]);
         translate([gear_radius - teeth_bevel /2, 0, 0])
         cylinder(d = teeth_bevel, h=thickness, $fn = 25);
      }
   }
}


module left_gear(){
   difference(){
      union(){
         rotate(-(180/(no_teeth-1)) / 2 - 90)
         gear();
         beam1();
      }
      cylinder(d=left_gear_hole_diameter, h=thickness);
   }
}

module right_gear(){
   difference(){
      union(){
         rotate(90)
         gear();
         beam1();
      }
      cylinder(d=right_gear_hole_diameter, h=thickness);
   }  
}

module beam1(){
   difference(){
      union(){
         translate([-0.5 * beam1_width, 0, 0])
         cube([beam1_width, beam1_length - 0.5 * beam1_width, thickness]);
         translate([0, beam1_length - 0.5 * beam1_width, 0])
         cylinder(d=beam1_width, h=thickness);
      }
      translate([0, beam1_length - 0.5 * beam1_width, 0])
      cylinder(d=beam1_hole_diameter, h=thickness);
   }
}

module beam2(){
   difference(){
      union(){
         translate([-0.5 * beam2_width, 0, 0])
         cube([beam2_width, beam2_length - 0.5 * beam2_width, thickness]);
         cylinder(d=beam2_width, h=thickness);
         translate([0,beam2_length - 0.5 * beam2_width, 0])
         cylinder(d=beam2_width, h=thickness);   
      }
      cylinder(d=beam2_hole_diameter, h=thickness);
      translate([0, beam2_length - 0.5 *beam2_width, 0])
      cylinder(d=beam2_hole_diameter, h=thickness);      
   }
}

module bottom_plate(){
   difference (){
      //bottom base plate to cut crap out of
      hull(){
         translate([gear_hole_dist/2 - plate_width/2 + plate_bevel_front, plate_length/2 - plate_bevel_front, 0])
         cylinder(r=plate_bevel_front, h = thickness);
         translate([gear_hole_dist/2 + plate_width/2 - plate_bevel_front, plate_length/2 - plate_bevel_front, 0])
         cylinder(r=plate_bevel_front, h = thickness);
         translate([gear_hole_dist/2 - plate_width/2 + plate_bevel_back , -plate_length/2 + plate_bevel_back, 0])
         cylinder(r=plate_bevel_back, h = thickness);
         translate([gear_hole_dist/2 + plate_width/2 - plate_bevel_back , -plate_length/2 + plate_bevel_back, 0])
         cylinder(r=plate_bevel_back, h = thickness);
      }
      
      /*
      translate([-0.5 * plate_length + 0.5 * gear_hole_dist, -0.5 * plate_width, 0])
      cube([plate_length, plate_width, thickness]);
      */
      
      //holes for screws of gears and servo mount cutouts
      cylinder(d=locked2mm,h=thickness);
      translate([gear_hole_dist -2, 0, 0])
      cube([15, servo_width, 30], center=true);  
      
      //cutouts for beam 2 bottom hole mounts
      translate([beam2_center_dist, beam2_forward_dist, 0])
      cylinder(d=locked2mm, h=thickness);
      translate([gear_hole_dist - beam2_center_dist, beam2_forward_dist, 0])
      cylinder(d=locked2mm, h=thickness);
      
   }
   translate([gear_hole_dist, 0, -servo_height + thickness])
   servo_bracket();
   
   //arm block
   translate([gear_hole_dist / 2 - arm_block_width/2, -plate_length/2, -arm_block_height])
   difference(){
      cube([arm_block_width, arm_block_length, arm_block_height + arm_block_length/2]);
      translate([0, arm_block_length/2, arm_block_length/2 ])
      rotate([0, 90, 0])
      cylinder(d = locked3mm, h=arm_block_width);
   }
   
   //suport arm block
   translate([gear_hole_dist / 2 - support_arm_width/2, -plate_length/2 + arm_block_length/2 + support_arm_distance - support_arm_length/2, -support_arm_height])
   difference(){
      union(){
         cube([support_arm_width, support_arm_length, support_arm_height + support_arm_length/2]);
         translate([support_arm_width/2-support_arm_stength_width/2,-support_arm_stength_length,support_arm_length/2+ support_arm_height - servo_height])
         cube([support_arm_stength_width, support_arm_stength_length, servo_height]);
      }
      translate([0,support_arm_length/2, support_arm_length/2])
      rotate([0, 90, 0])
      cylinder(d=locked3mm, h = support_arm_width);
   }
         
}

module servo_bracket(){
 translate([-(servo_length+2*servo_wall_thick)/2 - servo_shaft,-(servo_width+2*servo_wall_thick)/2,0])
 difference(){
   cube([servo_length + 2*servo_wall_thick, servo_width + 2*servo_wall_thick, servo_height]);
   translate([servo_wall_thick, servo_wall_thick, 0])
   cube([servo_length, servo_width, servo_height]);
 }   
}

module gripper_arm(){
   difference(){
      union(){
         hull(){
            cylinder(d = gripper_arm_width, h = thickness);   
            translate([beam2_center_dist, beam2_forward_dist, 0])
            cylinder(d = gripper_arm_width, h = thickness);
         }
         translate([-0.5 * gripper_arm_width + beam2_center_dist, beam2_forward_dist, 0])
         cube([gripper_arm_width, gripper_arm_length, thickness]);

         translate([-0.5 * gripper_arm_width + beam2_center_dist + (gripper_arm_width - gripper_arm_pad_thickness), beam2_forward_dist + (gripper_arm_length - gripper_arm_pad_length), 0])
         cube([gripper_arm_pad_thickness, gripper_arm_pad_length, gripper_arm_pad_height]);
         
      }
      cylinder(d = locked2mm, h = thickness);   
      translate([beam2_center_dist, beam2_forward_dist, 0])
      cylinder(d = locked2mm, h = thickness);
      
   }
}

module left_arm(){
   gripper_arm();
}

module right_arm(){
   mirror([1,0,0])
   gripper_arm();
}


translate([0,beam1_length - 0.5 * beam_widths,-thickness])
left_arm();

translate([gear_hole_dist,beam1_length - 0.5 * beam_widths,-thickness])
!right_arm();

translate([beam2_center_dist,beam2_forward_dist,0])
beam2();

translate([gear_hole_dist - beam2_center_dist,beam2_forward_dist,0])
beam2();


translate([0,0,-thickness])
bottom_plate();

left_gear();

translate([gear_radius * 2 - teeth_height + teeth_budge,0,0])
right_gear();