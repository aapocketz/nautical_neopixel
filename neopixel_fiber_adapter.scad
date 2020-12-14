/* [Global] */

// Which one would you like to see?
part = "both"; // [top:Top Only,base:Base Only,both:Top and Base]

/* [Top] */

// number of holes for LEDs
led_count = 10;
// length of the tube for fiber optics
tube_height = 10;
// width of the base (wings at bottom)
width = 14.0;
// radius of the chamfer to the wings. Larger number is more gradual chamfer.
chamfer_radius = 3.0;
// radius of the fillet of the base. This is mainly there to make it easier to print
fillet_radius = 1.0;
// the height fo the base (wings) on the side. Chamfer radius + wing height must be less than the tube_height
wing_height = 1.5;

// LED size (ws2812b according to datasheet is 5.0 * 5.4 * 1.57 with a 0.05mm tolerance.)
led_size = [5.4, 5.6, 1.65];

/* [Base] */

// thickness of the wall for the base
wall_thickness = 2.5;

// base clearance factor for clearance issues on the base
clearance = 1.1;
// led strip thickness
strip_thickness = 1.1;

/* [Hidden] */
$fn=100;

print_part();

module print_part() {
    if (part == "top") {
        led_top();
    } else if (part == "base") {
        led_base();
    } else {
        led_top();
        led_base();
    }
}


module led_top() {
    for (i=[0:led_count-1]) {
        translate([0, 7 * i, 0]) {
            difference() {
                union() {
                    // wing and chamfer material
                    translate([0, 0, (chamfer_radius + wing_height)/2]){
                        cube([width, 7, chamfer_radius + wing_height], true);
                    }
                    // tube material
                    translate([0, 0, tube_height/2]) {
                        cylinder(tube_height, 3.5, 3.5, true);
                    }
                }

                // chamfer each side
                for (side = [-1,1]) {
                    translate([side * (chamfer_radius + 3.5), 0, chamfer_radius + wing_height]) {
                        rotate([0, 90, 90]) {
                            cylinder(8, chamfer_radius, chamfer_radius, true);
                        }
                    }
                    translate([side * (width/2 + chamfer_radius + 3.5), 0, chamfer_radius + wing_height]) {
                        cube([width, 8, chamfer_radius*2], true);
                    }
                }

                // LED slot
                translate([0, 0, led_size[2]/2 - 0.01]) {
                    cube(led_size, true);
                }

                // Fiber tube
                cylinder(tube_height * 3, 2.2, 2.2, true);
            }
        }
    }
}

module led_base() {
    slot_depth = strip_thickness + wing_height;
    translate([0, 0, wing_height/2]) {
        for (i=[0:led_count-1]) {
            translate([0, 7 * i, 0]) {
                difference() {
                    // the overall base
                    cube([width + wall_thickness*2 + clearance*2, 7, slot_depth + wall_thickness*2 + clearance*2], true);
                    difference(){
                        // main LED slot in base
                        cube([width + clearance*2, 8, slot_depth + clearance*2], true);
                        // subtract out the fillet area
                        union() {
                            translate([width/2 + clearance, 0, slot_depth/2 + clearance]) {
                                difference(){
                                    translate([-fillet_radius/2, 0, -fillet_radius/2]){
                                        cube([fillet_radius, 8, fillet_radius], true);
                                    }
                                    translate([-fillet_radius, 0, -fillet_radius]){
                                        rotate([0, 90, 90]) {
                                            cylinder(9, fillet_radius, fillet_radius, true);
                                        }
                                    }
                                }
                            }
                            translate([-(width/2 + clearance), 0, slot_depth/2 + clearance]) {
                                difference(){
                                    translate([fillet_radius/2, 0, -fillet_radius/2]){
                                        cube([fillet_radius, 8, fillet_radius], true);
                                    }
                                    translate([fillet_radius, 0, -fillet_radius]){
                                        rotate([0, 90, 90]) {
                                            cylinder(9, fillet_radius, fillet_radius, true);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    // the opening at the top of the base
                    translate([0, 0, (slot_depth + wall_thickness)/2 + clearance]) {
                       cube([width - clearance*2, 8, wall_thickness + 0.01], true);
                    }
                }
            }
        }
    }
}
