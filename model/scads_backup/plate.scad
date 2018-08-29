//general
$fn = 30;                      //convexity of general curved faces
thickness = 3.5;                 //thickness next to flat servo

servoDistance = 50;             //distance between the inner of the two servo's pads

//circle servo horn measurements
hornCenterHole = 7;            //diameter of the center hole (for screw)
hornOuterHoleDiameters = 1.9;  //diameter of the outer holes for screwing down horn
hornOuterHolesCenterDist = 7.8;//distance from center to the outer holes
hornDist = 5;                  //distance of horn from wall against servo pad

//servo pad
servoHoleDiameter = 2.87;           //diamter of holes for servo attachment
servoHoleWidth = 10;             //width of short distance between hole so same side of servo
servoHoleLength = 48;            //length of long distance between holes on either side of servo
servoPadWidth = 20;              //width of servo pad
servoPadLength = 54;             //length of servo pad
servoLength = 41;                //wideness (length) of servo
servoPadThickness = 8;           //thickness of servo pad
servoWidth = servoPadLength;     //width or thin side of the servo

//arm cutout
cutoutWidth = 30;        //width of cutout
cutoutDepth = 1.5;         //depth of cutout

module horn(){
   cylinder(d=hornCenterHole, h=thickness);
   translate([hornOuterHolesCenterDist, 0, 0])
   cylinder(d=hornOuterHoleDiameters, h=thickness);
   translate([-hornOuterHolesCenterDist, 0, 0])
   cylinder(d=hornOuterHoleDiameters, h=thickness);
}

module servoHoles(){
   translate([servoPadLength/2 - servoHoleLength/2, servoPadWidth/2 - servoHoleWidth/2, 0])
   cylinder(d = servoHoleDiameter, h = servoPadThickness);
   translate([servoPadLength/2 - servoHoleLength/2, servoPadWidth/2 + servoHoleWidth/2, 0])
   cylinder(d = servoHoleDiameter, h = servoPadThickness);
   translate([servoPadLength/2 + servoHoleLength/2, servoPadWidth/2 - servoHoleWidth/2, 0])
   cylinder(d = servoHoleDiameter, h = servoPadThickness);
   translate([servoPadLength/2 + servoHoleLength/2, servoPadWidth/2 + servoHoleWidth/2, 0])
   cylinder(d = servoHoleDiameter, h = servoPadThickness);
}


module servoPad(){
   translate([servoPadLength/-2,0,thickness])
   rotate([90,0,0])
   difference(){
      cube([servoPadLength, servoPadWidth, servoPadThickness]);
      servoHoles();
   }
}

module hingeSection(){
   difference(){
      cube([(servoPadLength - servoLength) / 2, servoPadThickness, servoPadWidth/2 + (servoPadLength - servoLength) / 4]);
      translate([(servoPadLength - servoLength) / 4, 0, servoPadWidth / 2])
      rotate([-90,0,0])
      cylinder(d = servoHoleDiameter, h=servoPadThickness);
   }
}

module body(){
   translate([servoPadLength/-2, servoDistance/-2 - servoPadThickness, 0])
   cube([servoPadLength, servoDistance + 2 * servoPadThickness, thickness]);
   translate([0, servoDistance/-2, 0])
   servoPad();
   translate([0, servoDistance/2 + servoPadThickness, 0])
   servoPad();
}

difference(){
   body();
   //servos
   translate([servoLength/-2, servoDistance/-2 - servoPadThickness - 1, thickness])
   cube([servoLength, servoDistance + 2 * servoPadThickness + 2, servoPadWidth + 2]);
   //cutout
   translate([servoLength/-2, cutoutWidth / -2, thickness - cutoutDepth])
   cube([servoLength/2 - 1.5, cutoutWidth, cutoutDepth]);
   
   rotate(90)
   horn();
   
}
translate([servoPadLength/2 - (servoPadLength - servoLength) / 2 ,servoDistance/2,thickness+servoPadWidth])
hingeSection();

/*
difference(){
   hulledBody();
   
   union(){
      translate([-0.5*servoLength,-0.5*hornDiameter,thickness])
      cube([servoLength, hornDiameter, servoPadWidth+10]);
      
      translate([-0.5*servoWidth, -0.5*hornDiameter, thickness])
      cube([servoWidth, 0.5*hornDiameter-servoPadThickness, servoPadWidth+10]);
      
      translate([-0.5 * servoPadLength, -0.1, thickness])
      rotate([90,0,0])
      servoPad();
      
      horn();
   }
}   


//servoPad();
/*
module horn(){
   difference(){
      cylinder(d=hornDiameter, h=thickness, $fn = 60);
      cylinder(d=hornCenterHole, h=thickness);
      translate([hornOuterHolesCenterDist, 0, 0])
      cylinder(d=hornOuterHoleDiameters, h=thickness);
      translate([-hornOuterHolesCenterDist, 0, 0])
      cylinder(d=hornOuterHoleDiameters, h=thickness);
   }   
}

module servoPad(){
   difference(){
      cube([servoPadLength, servoPadWidth, servoPadThickness]);
      translate([servoPadLength/2 - servoHoleLength/2, servoPadWidth/2 - servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = servoPadThickness);
      translate([servoPadLength/2 - servoHoleLength/2, servoPadWidth/2 + servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = servoPadThickness);
      translate([servoPadLength/2 + servoHoleLength/2, servoPadWidth/2 - servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = servoPadThickness);
      translate([servoPadLength/2 + servoHoleLength/2, servoPadWidth/2 + servoHoleWidth/2, 0])
      cylinder(d = servoHoleDiameter, h = servoPadThickness);
   }  
}
