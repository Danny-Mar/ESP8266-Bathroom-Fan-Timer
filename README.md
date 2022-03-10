# ESP8266-Bathroom-Fan-Timer

Simple ESP8266 Bathroom Fan Timer. You will need an ESP01 with relay board and connect a light switch into the input GPIO3.

The fan runs normally as you would expect by turning the switch on and off however if you toggle the switch 3 times within 3 seconds the fan will stay on for 20 minutes.

You can increase the amount of time the fan will continue to run for by toggling the switch more than 3 times. For each subsequent toggle the timer will add 10 more minutes.

//FUTURE ADDITIONS
*To add a web portal to modify setup between toggle mode and timer off delay mode.
