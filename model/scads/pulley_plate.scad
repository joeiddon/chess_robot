$fn=60;
plate_width = 37;
plate_length = 84;
thickness = 5.1;
support_hole_diameter = 3.95;
support_hole_dist = 31;
support_hole_edge_dist = 7;
pillar_separation = 19;
pillar_thickness = 3;
pillar_width = 20;
pillar_height = 30;
pillar_hole_height =  21;
pillar_hole_diameter = 5;

module pillars(){
    translate([-pillar_separation/2, 0, 0]){
        translate([-pillar_thickness, 0, 0])
        difference(){
            cube([pillar_thickness, pillar_width, pillar_height]);
            translate([0, pillar_width/2, pillar_hole_height])
            rotate([0, 90, 0])
            cylinder(d=pillar_hole_diameter, h=pillar_thickness);
        }
        translate([pillar_separation, 0, 0])
        difference(){
            cube([pillar_thickness, pillar_width, pillar_height]);
            translate([0, pillar_width/2, pillar_hole_height])
            rotate([0, 90, 0])
            cylinder(d=pillar_hole_diameter, h=pillar_thickness);
        }
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
        translate([plate_length/2, plate_width-pillar_width, thickness])
        pillars();
    }
    support_holes();
}