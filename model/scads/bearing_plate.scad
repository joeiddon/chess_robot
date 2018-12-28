$fn = 60;
length = 76;
width = 30;
height = 3;
bearing_separation = 42;
bearing_holes_diameter = 4;
bearing_holes_flange_diameter = 8.7;
bearing_holes_width_spacing = 18;
bearing_holes_length_spacing = 24;
bearing_holes_separation = 18;
belt_hole_diameter = 3;
fastening_holes_diameter = 3;

module bearing_hole(){
    cylinder(d=bearing_holes_diameter, h=height);
    cylinder(d1=bearing_holes_diameter, d2=bearing_holes_flange_diameter, h=height);
}

module bearing_holes(){
    translate([-bearing_holes_length_spacing/2, -bearing_holes_width_spacing/2, 0]){
        bearing_hole();
        translate([bearing_holes_length_spacing, 0, 0])
        bearing_hole();
        translate([bearing_holes_length_spacing, bearing_holes_width_spacing, 0])
        bearing_hole();
        translate([0, bearing_holes_width_spacing, 0])
        bearing_hole();
    }
}

difference(){
    cube([length, width, height]);
    translate([length/2+bearing_separation/2, width/2, 0]){
        bearing_holes();
        cylinder(d=fastening_holes_diameter, h=height);
    }
    translate([length/2-bearing_separation/2, width/2, 0]){
        bearing_holes();
        cylinder(d=fastening_holes_diameter, h=height);
    }
    translate([length/2, width/2, 0])
    cylinder(d=belt_hole_diameter, h=height);
}