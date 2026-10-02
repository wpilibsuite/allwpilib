# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from __future__ import annotations

import math
import typing

inches_per_foot = 12.0
meters_per_inch = 0.0254
seconds_per_minute = 60
milliseconds_per_second = 1000
kilograms_per_lb = 0.453592


def meters_to_feet(m: meters) -> feet:
    """Converts given meters to feet.

    :param m: The meters to convert to feet.

    :returns: Feet converted from meters.
    """
    return meters_to_inches(m) / inches_per_foot


def feet_to_meters(ft: feet) -> meters:
    """Converts given feet to meters.

    :param ft: The feet to convert to meters.

    :returns: Meters converted from feet.
    """
    return inches_to_meters(ft * inches_per_foot)


def meters_to_inches(m: meters) -> inches:
    """Converts given meters to inches.

    :param m: The meters to convert to inches.

    :returns: Inches converted from meters.
    """
    return m / meters_per_inch


def inches_to_meters(i: inches) -> meters:
    """Converts given inches to meters.

    :param i: The inches to convert to meters.

    :returns: Meters converted from inches.
    """
    return i * meters_per_inch


# Converts given degrees to radians.
degrees_to_radians: typing.Callable[[degrees], radians] = math.radians

# Converts given radians to degrees.
radians_to_degrees: typing.Callable[[radians], degrees] = math.degrees


def radians_to_rotations(rad: radians) -> float:
    """Converts given radians to rotations.

    :param rad: The radians to convert.

    :returns: rotations Converted from radians.
    """
    return rad / math.tau


def degrees_to_rotations(deg: degrees) -> float:
    """Converts given degrees to rotations.

    :param deg: The degrees to convert.

    :returns: rotations Converted from degrees.
    """
    return deg / 360.0


def rotations_to_degrees(rotations: float) -> degrees:
    """Converts given rotations to degrees.

    :param rotations: The rotations to convert.

    :returns: degrees Converted from rotations.
    """
    return rotations * 360.0


def rotations_to_radians(rotations: float) -> float:
    """Converts given rotations to radians.

    :param rotations: The rotations to convert.

    :returns: radians Converted from rotations.
    """
    return rotations * math.tau


def rotations_per_minute_to_radians_per_second(
    rpm: revolutions_per_minute,
) -> radians_per_second:
    """Converts rotations per minute to radians per second.

    :param rpm: The rotations per minute to convert to radians per second.

    :returns: Radians per second converted from rotations per minute.
    """
    return (rpm / seconds_per_minute) * math.tau


def radians_per_second_to_rotations_per_minute(
    rps: radians_per_second,
) -> revolutions_per_minute:
    """Converts radians per second to rotations per minute.

    :param rps: The radians per second to convert to from rotations per minute.

    :returns: Rotations per minute converted from radians per second.
    """
    return (rps * seconds_per_minute) / math.tau


def milliseconds_to_seconds(ms: milliseconds) -> seconds:
    """Converts given milliseconds to seconds.

    :param ms: The milliseconds to convert to seconds.

    :returns: Seconds converted from milliseconds.
    """
    return ms / milliseconds_per_second


def seconds_to_milliseconds(s: seconds) -> milliseconds:
    """Converts given seconds to milliseconds.

    :param s: The seconds to convert to milliseconds.

    :returns: Milliseconds converted from seconds.
    """
    return s * milliseconds_per_second


def kilograms_to_lbs(kg: kilograms) -> pounds:
    """Converts kilograms into lbs (pound-mass).

    :param kg: The kilograms to convert to lbs (pound-mass).

    :returns: Lbs (pound-mass) converted from kilograms.
    """
    return kg / kilograms_per_lb


def lbs_to_kilograms(lbs: pounds) -> kilograms:
    """Converts lbs (pound-mass) into kilograms.

    :param lbs: The lbs (pound-mass) to convert to kilograms.

    :returns: Kilograms converted from lbs (pound-mass).
    """
    return lbs * kilograms_per_lb


#
# type aliases to make type checkers happier
# - None of these types do any enforcement of unit related things
#

# acceleration
meters_per_second_squared = float
feet_per_second_squared = float
standard_gravity = float

# angle
radians = float
nanoradians = float
microradians = float
milliradians = float
kiloradians = float
degrees = float
arcminutes = float
arcseconds = float
milliarcseconds = float
turns = float
gradians = float

# angular acceleration
radians_per_second_squared = float
degrees_per_second_squared = float
turns_per_second_squared = float

# angular velocity
radians_per_second = float
degrees_per_second = float
turns_per_second = float
revolutions_per_minute = float
milliarcseconds_per_year = float

# area
square_meters = float
square_feet = float
square_inches = float
square_miles = float
square_kilometers = float
hectares = float
acres = float

# capacitance
farads = float
nanofarads = float
microfarads = float
millifarads = float
kilofarads = float

# compound types
radians_per_meter = float
radians_per_second_per_volt = float
units_per_second = float
units_per_second_squared = float
volt_seconds = float
volt_seconds_squared = float
volt_seconds_per_meter = float
volt_seconds_squared_per_meter = float
volt_seconds_per_feet = float
volt_seconds_squared_per_feet = float
volt_seconds_per_radian = float
volt_seconds_squared_per_radian = float
unit_seconds_squared_per_unit = float
meters_per_second_squared_per_volt = float
meters_per_second_per_radian = float

# charge
coulombs = float
nanocoulombs = float
microcoulombs = float
millicoulombs = float
kilocoulombs = float
ampere_hours = float
nanoampere_hours = float
microampere_hours = float
milliampere_hours = float
kiloampere_hours = float

# concentration
parts_per_million = float
parts_per_billion = float
parts_per_trillion = float
percent = float

# conductance
siemens = float
nanosiemens = float
microsiemens = float
millisiemens = float
kilosiemens = float

# current
amperes = float
nanoamperes = float
microamperes = float
milliamperes = float
kiloamperes = float

# data
exabytes = float
exabits = float

# data transfer
exabytes_per_second = float
exabits_per_second = float

# density
kilograms_per_cubic_meter = float
grams_per_milliliter = float
kilograms_per_liter = float
ounces_per_cubic_foot = float
ounces_per_cubic_inch = float
ounces_per_gallon = float
pounds_per_cubic_foot = float
pounds_per_cubic_inch = float
pounds_per_gallon = float
slugs_per_cubic_foot = float

# energy
joules = float
nanojoules = float
microjoules = float
millijoules = float
kilojoules = float
calories = float
nanocalories = float
microcalories = float
millicalories = float
kilocalories = float
kilowatt_hours = float
watt_hours = float
british_thermal_units = float
british_thermal_units_iso = float
british_thermal_units_59 = float
therms = float
foot_pounds = float

# force
newtons = float
nanonewtons = float
micronewtons = float
millinewtons = float
kilonewtons = float
pounds = float
dynes = float
kiloponds = float
poundals = float

# frequency
hertz = float
nanohertz = float
microhertz = float
millihertz = float
kilohertz = float

# illuminance
luxes = float
nanoluxes = float
microluxes = float
milliluxes = float
kiloluxes = float
footcandles = float
lumens_per_square_inch = float
phots = float

# inductance
henries = float
nanohenries = float
microhenries = float
millihenries = float
kilohenries = float

# length
meters = float
nanometers = float
micrometers = float
millimeters = float
centimeters = float
kilometers = float
feet = float
mils = float
inches = float
miles = float
nautical_miles = float
astronical_units = float
lightyears = float
parsecs = float
angstroms = float
cubits = float
fathoms = float
chains = float
furlongs = float
hands = float
leagues = float
nautical_leagues = float
yards = float

# luminous flux
lumens = float
nanolumens = float
microlumens = float
millilumens = float
kilolumens = float

# luminous intensity
candelas = float
nanocandelas = float
microcandelas = float
millicandelas = float
kilocandelas = float

# magnetic flux
webers = float
nanowebers = float
microwebers = float
milliwebers = float
kilowebers = float
maxwells = float

# magnetic strength
teslas = float
nanoteslas = float
microteslas = float
milliteslas = float
kiloteslas = float
gauss = float

# mass
grams = float
nanograms = float
micrograms = float
milligrams = float
kilograms = float
metric_tons = float
pounds = float
long_tons = float
short_tons = float
stone = float
ounces = float
carats = float
slugs = float

# moment of inertia
kilogram_square_meters = float

# power
watts = float
nanowatts = float
microwatts = float
milliwatts = float
kilowatts = float
horsepower = float

# pressure
pascals = float
nanopascals = float
micropascals = float
millipascals = float
kilopascals = float
bars = float
mbars = float
atmospheres = float
pounds_per_square_inch = float
torrs = float

# radiation
becquerels = float
nanobecquerels = float
microbecquerels = float
millibecquerels = float
kilobecquerels = float
grays = float
nanograys = float
micrograys = float
milligrays = float
kilograys = float
sieverts = float
nanosieverts = float
microsieverts = float
millisieverts = float
kilosieverts = float
curies = float
rutherfords = float
rads = float

# resistance
ohms = float
nanoohms = float
microohms = float
milliohms = float
kiloohms = float

# solid angle
steradians = float
nanosteradians = float
microsteradians = float
millisteradians = float
kilosteradians = float
degrees_squared = float
spats = float

# substance
moles = float

# temperature
kelvin = float
celsius = float
fahrenheit = float
reaumur = float
rankine = float

# time
seconds = float
nanoseconds = float
microseconds = float
milliseconds = float
kiloseconds = float
minutes = float
hours = float
days = float
weeks = float
years = float
julian_years = float
gregorian_years = float

# torque
newton_meters = float
foot_poundals = float
inch_pounds = float
meter_kilograms = float

# velocity
meters_per_second = float
feet_per_second = float
miles_per_hour = float
kilometers_per_hour = float
knots = float

# voltage
volts = float
nanovolts = float
microvolts = float
millivolts = float
kilovolts = float
statvolts = float
abvolts = float

# volume
cubic_meters = float
cubic_millimeters = float
cubic_kilometers = float
liters = float
nanoliters = float
microliters = float
milliliters = float
kiloliters = float
cubic_inches = float
cubic_feet = float
cubic_yards = float
cubic_miles = float
gallons = float
quarts = float
pints = float
cups = float
fluid_ounces = float
barrels = float
bushels = float
cords = float
cubic_fathoms = float
tablespoons = float
teaspoons = float
pinches = float
dashes = float
drops = float
fifths = float
drams = float
gills = float
pecks = float
sacks = float
shots = float
strikes = float
