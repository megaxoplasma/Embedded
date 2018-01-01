Assumption note : During one of the lab time we spoke with Niyanth and he presented the goal as "To be able to navigate
from point A to point B without getting stuck etc", we interperated it as any point A to point B, not necessarily exit the maze
thus our algorithem was simply to navigate the maze to avoid collision and recover when stuck by backing up, and that
if we navigate the maze for long enough time, it will eventually get from any random point A to B chosen by the TA. Niyanth told
us to include this in the readme to clarify the misunderstanding/assumption that contributed to our design. The design does what
WE meant for it to do, it just doesn't exit due to our misunderstanding.  


The code can be divided into two parts. The hardware setup part and the algorithm to navigate the car inside the maze.

The hardware setup part contains (1) the three ultrasonic configuration, which feedback the distance information;
(2) the PWM configuration, which controls the rotation of the motor. Four functions are established and is called in different cases.
They are ahead(); back(); turn_around(); stop_run(); The input of them (except the last one) are two duty cycles. 
(3) the UART configuration, which enable the printing of some parameters inside the code, we use this to test the ultrasonic sensors.
The hardware setup part is developed based on Abhi's sample code.

The algorithm can be briefly described as follows: 
(1) The car will first check the value front sensor, if it's smaller than a value but larger than a small value,
it means there is a wall in front of it. Then it turn around.
(2) If there is no wall in front, and no wall at left side, it follow the right side wall.
(3) If there is no wall in front but a wall at left side, it won't follow the left side wall but move to the opposite direction.
(4) To avoid crash and stucked to the front wall, if the sensor in the front gives a little value (less than the lower bound),
let it move backward and return to normal state.


