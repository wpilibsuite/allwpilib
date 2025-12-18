#!/bin/sh

file_paths=$(cat update_units_paths)
git restore $file_paths
python3 update_units.py $file_paths
