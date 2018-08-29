/********************
model of mg996r servo
*********************/

//dimensions of central box (excluding the pads)
//height is including the proruding neck around the shaft
length = 40.3;
width = 20;
height = 37;

//total_length = length + pad_length * 2 = 53.3

//distance from nearest wall to the motor shaft
wall_to_shaft = 10.5;

//self explanatory measuremets
hole_length = 48;
hole_width = 10;
hole_diameter = 4;

//height is from ground to bottom of pad
pad_height = 26;
//self explanatory
pad_thick = 2.5;
pad_length = 6.5;

//convexity of holes
$fn = 20;

module pad(){
    difference(){
        cube([pad_length, width, pad_thick]);
        translate([pad_length/2, width/2-hole_width/2, 0])
        cylinder(d=3,h=pad_thick);
        translate([pad_length/2, width/2+hole_width/2, 0])
        cylinder(d=3,h=pad_thick);
    }
}

module servo(){
    translate([0,0,pad_height])
    pad();
    translate([pad_length+length,0,pad_height])
    pad();
    translate([pad_length, 0, 0])
    cube([length, width, height]);
    translate([pad_length + wall_to_shaft, width/2, height])
    cylinder(h=5,d=5);
}

color([0,1,0])
servo();