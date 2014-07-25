#!/usr/bin/env bash

# Temporary method to deploy C++ integration tests to the RoboRIO

if [ $(which sshpass) ]
then
	# Send stderr to /dev/null - the only thing printed to it is the login prompt
	sshpass -p "" ssh admin@10.1.90.2 killall FRCUserProgram java 2> /dev/null
	sshpass -p "" scp target/cmake/wpilibc/wpilibC++IntegrationTests/FRCUserProgram admin@10.1.90.2:/home/admin 2> /dev/null
	sshpass -p "" ssh admin@10.1.90.2 ./FRCUserProgram --gtest_color=yes $* 2> /dev/null
else
	ssh admin@10.1.90.2 killall FRCUserProgram
	scp target/cmake/wpilibc/wpilibC++IntegrationTests/FRCUserProgram admin@10.1.90.2:/home/admin
	ssh admin@10.1.90.2 ./FRCUserProgram --gtest_color=yes $*
fi



