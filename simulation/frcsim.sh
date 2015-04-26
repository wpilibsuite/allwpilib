#!/bin/bash
export GAZEBO_PLUGIN_PATH="${GAZEBO_PLUGIN_PATH}:${HOME}/wpilib/simulation/plugins"
export GAZEBO_MODEL_PATH="${GAZEBO_MODEL_PATH}:${HOME}/wpilib/simulation/models"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${HOME}/wpilib/simulation/plugins"
gazebo $@
