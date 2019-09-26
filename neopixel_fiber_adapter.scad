/* [Global] */

// Which one would you like to see?
part = "top"; // [top:Top Only,base:Base Only,both:Top and Base]

/* [Top] */

// number of holes for LEDs
led_count = 5;
// length of the tube for fiber optics
tube_height = 10;
// width of the base (wings at bottom)
width = 16;
// radius of the chamfer to the wings. Larger number is more gradual chamfer.
chamfer_radius = 3;
// the height fo the base (wings) on the side. Chamfer radius + wing height must be less than the tube_height
wing_height = 1.5;

/* [Base] */

// thickness of the wall for the base
wall_thickness = 1.5;

// fudge factor for clearance issues on the base
fudge = 0.2;

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
                translate([0, 0, 0.7]) {
                    cube([5.2, 5.4, 1.41], true);
                }

                // Fiber tube
                cylinder(tube_height * 3, 2.2, 2.2, true);
            }
        }
    }
}

module led_base() {
    translate([0,0, wall_thickness/2]) {
        for (i=[0:led_count-1]) {
            translate([0, 7 * i, 0]) {
                difference() {
                    cube([width + wall_thickness*2 + fudge*2, 7, wing_height + wall_thickness*2 + fudge*2], true);
                    cube([width + fudge*2, 8, wing_height + fudge*2], true);
                    translate([0, 0, (wing_height + wall_thickness)/2 + fudge]) {
                        cube([width - fudge*6, 8, wall_thickness + 0.01], true);
                    }
                }
            }
        }
    }
}