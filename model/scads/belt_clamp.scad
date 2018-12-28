$fn = 60;
height = 7.6;
width = 12;
length = 40;
screw_hole_diameter = 3;
belt_hole_outer = 8;
belt_hole_inner = 4;
belt_width = 6;
belt_cutout_thickness = 2.5;
belt_holes_separation = 5.6;

module belt(){
    translate([0, -belt_cutout_thickness/2, 0])
    cube([(length-belt_holes_separation)/2 - (belt_hole_outer + belt_hole_inner)/2, belt_cutout_thickness, belt_width]); 
    translate([(length-belt_holes_separation)/2 - belt_hole_outer/2, 0, 0])
    difference(){
        cylinder(d=belt_hole_outer, h=belt_width);
        cylinder(d=belt_hole_inner, h=belt_width);
    }
}

difference(){
    cube([length, width, height]);
    translate([length/2, 0, height/2])
    rotate([-90, 0, 0])
    cylinder(d=screw_hole_diameter, h=width);
    translate([0, width/2, height-belt_width])
    belt();
    translate([length, width/2, height-belt_width])
    mirror([1, 0, 0])
    belt();
}