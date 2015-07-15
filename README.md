# Beachsand
Project Beachsand is all about low-power easy to set-up RF sensor networks. At the moment it consists of SenseGrain and  SandCasle, with ActGrain coming some time in the future. 

The SenseGrain is an Attiny85 with an RF transmitter and a battery. The SandCastle is an Arduino with an RF receiver. Once assembled the only thing the user needs to do is select the update frequency. At the moment those are continuous, 5 sec, 30 sec, 5 min, 30 min, 2h, 6h, 24h and on interrupt. The update frequency has a great affect on the battery life - on continuous mode the battery should last a good month, on all the time modes 3-4 years and on interrupt up to 30 years (theoretically). After the initial setup all the user has to do is add the analog sensor or switch to the SenseGrain. The last step is to leave the SenseGrain in the location where it needs to measure things and then forget about it for a very long time.

The SenseGrain then keeps transmitting it’s name and the value from the analog sensor according to the intervals it was set up to. Those values are then received by the SandCastle and can from there be used for whatever application they are needed. One SandCastle can receive values from multiple SenseGrains.

TO DO:
- Automatic name generation for SenseGrains
- ActGrain remote wakeup and auto-pair
- Improve documentation
- Add links to other resources
- Visualize the set-up process
- Create direct connections between SenseGrain and ActGrain
- Create SandCastle Arduino shield with Wi-fi and SD-card support, for logging and sending values without the need of a PC

Wishlist:
- Web dashboard
- Solar power
- Mesh network

