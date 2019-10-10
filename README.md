# Nautical Neopixel Arduino project

This arduino sketch is designed to be programmed into a framed nautical chart in order to light up the buoys, lighthouses, channel markers and other lighted nautical aids to navigation found on a NOAA nautical chart. Each light has a color and flashing/timing characteristic which can be simulated. This would make an interesting art display or a project for someone learning or experimenting with microcontrollers.

See the comments in the sketch for more information on implementation.

You can simulate the operation of the arduino and LEDs on Tinkercad Circuit simulator https://www.tinkercad.com/things/gWAsxpgxWYh.

The chart I printed required over a hundred lighted aids to navigation, some that even printed out in a large format (20x30in) some lateral buoys/marks are spaced closely. I wanted to light them all, so decided to use 1mm fiber optics. This solved the problem with spacing - Neopixels are very small, but even 5050 LEDs are 5mm wide. Since most of the buoys in a given chart will have the same color and timing, you can also multiplex map several fibers fed from a single LED, and even the chart I printed with hundred of lights only required fewer than 20 unique LEDs. By using fewer LEDs and being smarter about power you could also make this project battery powered. 

# SCAD 3D print
To mate the optical fiber to the LEDs, I designed a coupler that makes it easy to mate the fiber to individual LEDs. This SCAD is customizable and can be accessed directly using thingiverse customizer https://www.thingiverse.com/thing:3881790. 


# parts and tools required and project assembly

1. Frame a nautical chart. Find a nautical chart, and a print service and frame style that will work for you. I went with samsclub photo as they have "standout" prints 20x30in for ~$80. Standout prints are a good option as it is a nice hard surface to drill into to mount the LEDs and have a nice deep frame to hide electronics. To print a nautical chart you can get PDF charts for free from NOAA https://nauticalcharts.noaa.gov/. Keep in mind the aspect ratio is varied with each chart, and you will need to either find a print service that can handle custom sizes or crop/trim the image to fit. To match the modern black standout frame and modern look of LEDs I inverted the image which created an image that was darker and I think goes better with the frame and nautical lights against a dark background at night.\
\
For example this is the image I had printed from chart 12270 (warning large image):\
<img src="chesapeake_map_3x2_inverted.jpg" width="400">\
Note: I am not sure I would recommend the samsclub standouts, as they have a hard backing and it glued up with styrofoam inside which I didn't realize. To get this project to work I had to cut the backing off and carve out the styrofoam to mount the electronics. It would have been easier with standout frame that had a hollow backing with no solid foam layer. Box mount or canvas mount frames that are hollow in the back may be a better option. 

2. Program the Arduino. I used an Andruino nano board clone from Amazon (a pack of 3 was about $13). https://www.amazon.com/ELEGOO-Arduino-ATmega328P-Without-Compatible/dp/B0713XK923. Figure out the different light sequences you will need for your chart and edit the arduino sketch to include the number of unique LEDs and timing needed.  
*ProTip: Note each nautical light should be clearly printed on the NOAA chart and with a sequence described in chart 1. Also the coast guard provides a list of all the lights in a chart for regions which is a great reference https://www.navcen.uscg.gov/?pageName=lightLists. It helps to put all of them in a spreadsheet and sort by unique to find out how many different unique LEDs you will need. If you have a bunch of the same sequenced lights (ex. flashing green 4s), you may want to have multiple LEDs allocated with them. I wouldn't put more than maybe 10 1mm fibers in a single bundle for each LED (note this is an example of an interesting 2D packing problem https://en.wikipedia.org/wiki/Circle_packing_in_a_circle).*

3. Connect the Arduino to the LED strip to test functionality. I used a cheap ws2812b 144/1M 5050 led strip I found on ebay for ~$12. Trim to the number of LEDs your chart requires. I soldered wire directly to the arduino through holes from the LED strip to finalize the attachment.

4. 3D print the coupler from thingiverse. The LED strip should mount easily in the assembly with channels for optical fiber to each LED. Figure out where you will mount the LEDs behind your frame.

5. Drill holes for each fiber optic to poke through the print. I used a 1mm drill bit attached to a dremel. If you don't have a dremel you could use a small hand drill like a pin vise, which are very cheap. 

6. Mount the optical fiber. Push the fiber through each hole in the print, trimming the end with a flush cutter/wire cutter. To prevent the end from pulling through the print and to spread the light a bit, tap the end of the soldering iron to the end of the fiber, the plastic will mushroom out a bit preventing it from pulling through. Make sure you leave enough fiber length to reach the place where you are mounting the LEDs in the frame.
50m of 1mm fiber optics is about $8 on amazon https://smile.amazon.com/gp/product/B07W979RH3/. 

7. route the fiber optics to the LEDs and attach them to the coupler. I recommend only mounting fiber and routing for a single LED color at a time, so for instance all Flashing Red 2.5s should be pushed through and attached before going on to the next color/timing. To attach to the coupler I recommend clear gorilla glue ($8 for a large bottle https://smile.amazon.com/Gorilla-Clear-Glue-ounce-Bottle/dp/B074J7XQZT) as it dries really clear to not block light. I don't recommend hot glue as you can easily melt the fiber optics! The only downside of the Gorilla glue is it takes time to set, so I recommend gluing up one LED at a time and letting it set before working on the next LED.

8. Mount the power cable. I powered the Arduino using a USB panel mount connector. USB cables are cheap and it allows the program to be reprogrammable after framing. You could also power the project with a barrel connector and a cheap 5v supply, or even use batteries. USB type B to USB mini panel mount connector is about $9 https://www.amazon.com/gp/product/B07PJYNTKS/. Note the panel mount I ordered didn't come with screws, but I had some spare screws that worked (I think size M3).

9. Cover the backing. This is optional, but with loose fiber everywhere it helps encapsulate everything. I glued a black plastic corregated board to the back ($4 at https://www.michaels.com/plastic-corrugated-board-by-creatology/M10567770.html).

## results

video of chart LEDs flashing: https://youtu.be/PLgCkBzhD6o  
video of 3D printed LED coupler: https://www.youtube.com/watch?v=q0YkyLGdePY




