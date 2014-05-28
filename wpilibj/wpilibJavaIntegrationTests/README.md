TO LOAD  & RUN INTEGRATION TESTS

1) Run 'mvn clean package' from this directory
2) Run 'scp target/wpilibJavaIntegrationTests-0.1.0-SNAPSHOT.jar admin@10.1.90.2:/home/admin' and enter the password on the RoboRio
3) ssh into the RoboRio using 'ssh admin@10.1.90.2'
4) Run the integration tests on the roborio using './runintegrationjavaprogram'
5) Enable the robot using a driver station (this will be automatic eventually)
