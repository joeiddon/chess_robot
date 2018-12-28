$fn=60;
plate_width = 58;
plate_length = 84;
thickness = 5.1;
support_hole_diameter = 3.95;
support_hole_dist = 31;
support_hole_edge_dist = 7;
support_width = 14; //used for cutout for stepper
stepper_hole_dist = 31;
stepper_hole_diameter = 3.1;
stepper_support_height = 10;
stepper_support_width = 5.6;
stepper_hole_height = 5.5;
stepper_edge_cut = 30;


module stepper_support(){
    difference(){
        cube([stepper_support_width, plate_width - support_width, stepper_support_height]);
        translate([0, (plate_width - support_width + stepper_hole_dist) / 2, stepper_hole_height])
        rotate([0, 90, 0])
        cylinder(d=stepper_hole_diameter,  h=stepper_support_width);
        translate([0, (plate_width - support_width - stepper_hole_dist) / 2, stepper_hole_height])
        rotate([0, 90, 0])
        cylinder(d=stepper_hole_diameter,  h=stepper_support_width);
    }
}

module support_holes(){
    translate([plate_length/4 - support_hole_dist/2, support_hole_edge_dist, 0]){
        cylinder(d=support_hole_diameter, h=thickness);
        translate([support_hole_dist, 0, 0])
        cylinder(d=support_hole_diameter, h=thickness);
    }
    translate([3*plate_length/4 - support_hole_dist/2, support_hole_edge_dist, 0]){
        cylinder(d=support_hole_diameter, h=thickness);
        translate([support_hole_dist, 0, 0])
        cylinder(d=support_hole_diameter, h=thickness);
    }
}

difference(){
    union(){
        cube([plate_length, plate_width, thickness]);
        translate([stepper_edge_cut, support_width, thickness])
        stepper_support();
    }
    support_holes();
    translate([0,  support_width, 0])
    cube([stepper_edge_cut, plate_width - support_width, thickness]);
}