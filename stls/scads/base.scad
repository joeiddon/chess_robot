//general
$fn = 30;                        //convexity of general curved faces
thickness = 5;                   //thickness of the flat parts
length = 54;

//bottom plate (feet)
plateHoleDiameter = 4;            //diameter of holes in metal bottom plate
plateHoleWidth = 19;             //shortest distance between the pairs of holes in plate
plateHoleLength = 44;            //long distance between the pairs of holes in botto
plateFeetWidth = 20;             //width of the pads that connect the short distanced screws
plateFeetLength = length;            //length of the feet
//servo pad
servoHoleDiameter = 3;           //diamter of holes for servo attachment
servoHoleWidth = 10;             //width of short distance between hole so same side of servo
servoHoleLength = 48;            //length of long distance between holes on either side of servo
servoPadWidth = 18;              //width of servo pad
servoPadLength = length;             //length of servo pad
servoPadHeight = 25;             //height of servo pad
servoLength = 41;

module foot(){
   translate([0, plateFeetWidth/-2, 0])
   difference(){
      cube([plateFeetLength, plateFeetWidth, thickness]);
      translate([plateFeetLength/2 - plateHoleWidth/2, plateFeetWidth/2, 0])
      cylinder(h=thickness, d=plateHoleDiameter);
      translate([plateFeetLength/2 + plateHoleWidth/2, plateFeetWidth/2, 0])
      cylinder(h=thickness, d=plateHoleDiameter);
   }
}


module servoPad(){
   difference(){
      cube([servoPadLength, servoPadWidth, thickness]);
      translate([servoPadLength/2 - servoHoleLength/2, servoPadWidth/2 - servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = thickness);
      translate([servoPadLength/2 - servoHoleLength/2, servoPadWidth/2 + servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = thickness);
      translate([servoPadLength/2 + servoHoleLength/2, servoPadWidth/2 - servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = thickness);
      translate([servoPadLength/2 + servoHoleLength/2, servoPadWidth/2 + servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = thickness);
   }   
}

module wall(){
   hull(){
      translate([0,plateFeetWidth/-2, 0])
      cube([plateFeetLength, plateFeetWidth, thickness]);
      translate([0,plateHoleLength/2 - servoPadWidth/2, servoPadHeight])
      cube([servoPadLength, servoPadWidth, thickness]);
   }  
}

difference(){
   union(){
      difference(){
         union(){
            wall();
            translate([0, plateHoleLength, 0])
            mirror([0,1,0])
            wall();
         }
         translate([0, plateHoleLength/2 - servoPadWidth/2, servoPadHeight])
         cube([servoPadLength, servoPadWidth, thickness]);
      }
      translate([0, plateHoleLength/2 - servoPadWidth/2, servoPadHeight])
      servoPad();
   }
   translate([(length - servoLength) / 2 ,-0.5 * plateHoleLength, 0])
   cube([servoLength, plateHoleLength*2, servoPadHeight * 2]);   
}

foot();
translate([0, plateHoleLength, 0])
foot();


/*
hull(){
foot();
translate([0, plateHoleLength/2 - servoPadWidth/2, servoPadHeight])
servoPad();
}