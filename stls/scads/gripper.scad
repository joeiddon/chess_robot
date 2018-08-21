$fn = 50;
locked3mm = 2.9;           //hole diameter for a locked/tight/screwed down 2mm screw
freeTurning2mm = 2.5;      //hole diameter for a free turning 2mm screw
locked2mm  = 1.9;            //hole diameter for a locked/tight/screwed down 2mm
thickness = 2.7;           //thickness of the gears and beams etc.
gearRadius = 15;           //radius of the gears (cogs)
leftGearHoleDiameter = freeTurning2mm; //diameter of hole in left gear
rightGearHoleDiameter = 2.1;   //attacing to servo sotiny bit > 2mm (shrinkage)
noTeeth  = 7;              //number of teeth on the gears
teethWidth = 4;            //width of gear teeth
teethBevel = 3;            //diameter of circles at end of teeth
teethHeight = 3.5;         //height of gear teeth
teethBudge = 0.4;         //distances gears need to be appart to fit together
beamWidths = 4;           //width of all beams
beam1Width = beamWidths;    //width of the first beam
beam1Length = 33;          //length of the first beam
beam1HoleDiameter = freeTurning2mm; //diameter of screw hole at end of beam
beam2Width = beamWidths;   //width of the second beam
beam2Length = beam1Length; //length of the second beam
beam2HoleDiameter = freeTurning2mm;//diameter of screw holes at end of beam
beam2CenterDist = 10;       //translating beam2 from beam1 towards the centerline
beam2ForwardDist = 18;     //translatimg beam2 from beam1 in y dir towards the grabber end
servoWidth = 12;           //width of servo
servoLength = 23;          //length of servo
servoHeight = 10.8;        //height of servo bracket (not whole servo)
servoWallThick = 1.2;      //thickness of bracket
servoShaft = 5.9;          //distance of shaft from closest lengthwise edge
gripperArmLength = 24;     //length of nipper
gripperArmThickness = 5;   //thickness of end nippers
gripperArmPadLength = 16;  //length of the raised pad
gripperArmPadThickness = 3;//thickness of the raised pad
gripperArmPadHeight = 12;  //height of the raised pad
gripperArmWidth = beamWidths; //width of end nippers

gearHoleDist = gearRadius * 2 - teethHeight + teethBudge;

//////

armBlockWidth = 15;        //width of block for connecting arms
armBlockLength = 5;         //length ""
armBlockHeight = 9.9 + 5.5;         //height for screws to attach to connecting arms

supportArmLength = 5;      //length of block
supportArmWidth = 15;       //widhth of block
supportArmHeight = armBlockHeight + 20;     //heighrt for screw to attach to the support arm
supportArmDistance = 32;   //distance from little block
supportArmStengthWidth  = 4;  //width of support for support arm block
supportArmStengthLength = 2.5; //length ""


plateWidth = 40.5;           //width of bottom plate
plateLength = 40 + locked3mm + armBlockLength/2;          //length of bottom palte
plateBevelBack = 5;        //bevel radius for back corners of plate
plateBevelFront = 10;      //bevel radius for front corners of plate


module gear(){
   cylinder(r=gearRadius - teethHeight, h = thickness);
   for (i = [0:180/(noTeeth-1):180]){
      rotate(i)
      hull(){
         translate([0,-0.5*teethWidth,0])
         cube([gearRadius - teethHeight, teethWidth, thickness]);
         translate([gearRadius - teethBevel /2, 0, 0])
         cylinder(d = teethBevel, h=thickness, $fn = 25);
      }
   }
}


module leftGear(){
   difference(){
      union(){
         rotate(-(180/(noTeeth-1)) / 2 - 90)
         gear();
         beam1();
      }
      cylinder(d=leftGearHoleDiameter, h=thickness);
   }
}

module rightGear(){
   difference(){
      union(){
         rotate(90)
         gear();
         beam1();
      }
      cylinder(d=rightGearHoleDiameter, h=thickness);
   }  
}

module beam1(){
   difference(){
      union(){
         translate([-0.5 * beam1Width, 0, 0])
         cube([beam1Width, beam1Length - 0.5 * beam1Width, thickness]);
         translate([0, beam1Length - 0.5 * beam1Width, 0])
         cylinder(d=beam1Width, h=thickness);
      }
      translate([0, beam1Length - 0.5 * beam1Width, 0])
      cylinder(d=beam1HoleDiameter, h=thickness);
   }
}

module beam2(){
   difference(){
      union(){
         translate([-0.5 * beam2Width, 0, 0])
         cube([beam2Width, beam2Length - 0.5 * beam2Width, thickness]);
         cylinder(d=beam2Width, h=thickness);
         translate([0,beam2Length - 0.5 * beam2Width, 0])
         cylinder(d=beam2Width, h=thickness);   
      }
      cylinder(d=beam2HoleDiameter, h=thickness);
      translate([0, beam2Length - 0.5 *beam2Width, 0])
      cylinder(d=beam2HoleDiameter, h=thickness);      
   }
}

module bottomPlate(){
   difference (){
      //bottom base plate to cut crap out of
      hull(){
         translate([gearHoleDist/2 - plateWidth/2 + plateBevelFront, plateLength/2 - plateBevelFront, 0])
         cylinder(r=plateBevelFront, h = thickness);
         translate([gearHoleDist/2 + plateWidth/2 - plateBevelFront, plateLength/2 - plateBevelFront, 0])
         cylinder(r=plateBevelFront, h = thickness);
         translate([gearHoleDist/2 - plateWidth/2 + plateBevelBack , -plateLength/2 + plateBevelBack, 0])
         cylinder(r=plateBevelBack, h = thickness);
         translate([gearHoleDist/2 + plateWidth/2 - plateBevelBack , -plateLength/2 + plateBevelBack, 0])
         cylinder(r=plateBevelBack, h = thickness);
      }
      
      /*
      translate([-0.5 * plateLength + 0.5 * gearHoleDist, -0.5 * plateWidth, 0])
      cube([plateLength, plateWidth, thickness]);
      */
      
      //holes for screws of gears and servo mount cutouts
      cylinder(d=locked2mm,h=thickness);
      translate([gearHoleDist -2, 0, 0])
      cube([15, servoWidth, 30], center=true);  
      
      //cutouts for beam 2 bottom hole mounts
      translate([beam2CenterDist, beam2ForwardDist, 0])
      cylinder(d=locked2mm, h=thickness);
      translate([gearHoleDist - beam2CenterDist, beam2ForwardDist, 0])
      cylinder(d=locked2mm, h=thickness);
      
   }
   translate([gearHoleDist, 0, -servoHeight + thickness])
   servoBracket();
   
   //arm block
   translate([gearHoleDist / 2 - armBlockWidth/2, -plateLength/2, -armBlockHeight])
   difference(){
      cube([armBlockWidth, armBlockLength, armBlockHeight + armBlockLength/2]);
      translate([0, armBlockLength/2, armBlockLength/2 ])
      rotate([0, 90, 0])
      cylinder(d = locked3mm, h=armBlockWidth);
   }
   
   //suport arm block
   translate([gearHoleDist / 2 - supportArmWidth/2, -plateLength/2 + armBlockLength/2 + supportArmDistance - supportArmLength/2, -supportArmHeight])
   difference(){
      union(){
         cube([supportArmWidth, supportArmLength, supportArmHeight + supportArmLength/2]);
         translate([supportArmWidth/2-supportArmStengthWidth/2,-supportArmStengthLength,supportArmLength/2+ supportArmHeight - servoHeight])
         cube([supportArmStengthWidth, supportArmStengthLength, servoHeight]);
      }
      translate([0,supportArmLength/2, supportArmLength/2])
      rotate([0, 90, 0])
      cylinder(d=locked3mm, h = supportArmWidth);
   }
         
}

module servoBracket(){
 translate([-(servoLength+2*servoWallThick)/2 - servoShaft,-(servoWidth+2*servoWallThick)/2,0])
 difference(){
   cube([servoLength + 2*servoWallThick, servoWidth + 2*servoWallThick, servoHeight]);
   translate([servoWallThick, servoWallThick, 0])
   cube([servoLength, servoWidth, servoHeight]);
 }   
}

module gripperArm(){
   difference(){
      union(){
         hull(){
            cylinder(d = gripperArmWidth, h = thickness);   
            translate([beam2CenterDist, beam2ForwardDist, 0])
            cylinder(d = gripperArmWidth, h = thickness);
         }
         translate([-0.5 * gripperArmWidth + beam2CenterDist, beam2ForwardDist, 0])
         cube([gripperArmWidth, gripperArmLength, thickness]);

         translate([-0.5 * gripperArmWidth + beam2CenterDist + (gripperArmWidth - gripperArmPadThickness), beam2ForwardDist + (gripperArmLength - gripperArmPadLength), 0])
         cube([gripperArmPadThickness, gripperArmPadLength, gripperArmPadHeight]);
         
      }
      cylinder(d = locked2mm, h = thickness);   
      translate([beam2CenterDist, beam2ForwardDist, 0])
      cylinder(d = locked2mm, h = thickness);
      
   }
}

module leftArm(){
   gripperArm();
}

module rightArm(){
   mirror([1,0,0])
   gripperArm();
}


translate([0,beam1Length - 0.5 * beamWidths,-thickness])
leftArm();

translate([gearHoleDist,beam1Length - 0.5 * beamWidths,-thickness])
!rightArm();

translate([beam2CenterDist,beam2ForwardDist,0])
beam2();

translate([gearHoleDist - beam2CenterDist,beam2ForwardDist,0])
beam2();


translate([0,0,-thickness])
bottomPlate();

leftGear();

translate([gearRadius * 2 - teethHeight + teethBudge,0,0])
rightGear();