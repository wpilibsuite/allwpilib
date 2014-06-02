#!/usr/bin/env bash

# Temporary method to deploy C++ integration tests to the RoboRIO

if [ $(which sshpass) ]
then
	sshpass -p "" ssh admin@10.1.90.2 killall FRCUserProgram
	sshpass -p "" scp cmake/target/cmake/wpilibc/wpilibC++IntegrationTests/FRCUserProgram admin@10.1.90.2:/home/admin
	sshpass -p "" ssh admin@10.1.90.2 ./FRCUserProgram
else
	ssh admin@10.1.90.2 killall FRCUserProgram
	scp cmake/target/cmake/wpilibc/wpilibC++IntegrationTests/FRCUserProgram admin@10.1.90.2:/home/admin
	ssh admin@10.1.90.2 ./FRCUserProgram
fi


