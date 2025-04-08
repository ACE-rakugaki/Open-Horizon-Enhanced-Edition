# open-horizon

An enhanced fork of the open source implementation of [Ace Combat: Assault Horizon](http://acecombat.wikia.com/wiki/Ace_Combat:_Assault_Horizon)‘s render and physics.
I want to revive it my way and also need help from contributors. I'm grateful if you join the project.

[DFM](http://acecombat.wikia.com/wiki/Close_Range_Assault)-Free

Requires resource files from the original game.

Roadmap (Note: Completed tasks are removed)

Campaign mode

Convertor for AC4-6 locations

Correct material assignment

Plane shader

Plane lods

ADMM, EML

Bombers

Correct flight model

Take off and landing

Weather

GUI map

Clouds corrections

Aircraft viewer

Air effects

Decals

Damaged planes

Cockpit avionics


Characters

Character animations

Advanced AI

Helicopters

Replays

Final part

Graphics tune to look alike/better

Optimisation

Refactoring

Python bindings

Unscheduled (anytime)

Animations fix

Loading remained ACAH locations

PS3 resources research (ACAH, Infinity)

AC6 resources research

AC4 resources research

AC5 resources research

Mission editor help

Change mode: ctrl+(1-6)

Selection widget
Focus on object: double click

Selecting multiple objects: ctrl/shift

Clear selection: ctrl+d

Select all: ctrl+a

Central widget
Move camera: mouseRight

Rotate camera: mouseRight + shift

Change camera height: mouseRight + alt

Modes widget
Add
Add object: mouseLeft

Rotate: shift+mouseY

Change height: alt+mouseY

Edit
Move objects: mouseLeft

Rotate objects: shift+mouseY

Change objects height: alt+mouseY

Remove objects: del

Path
New path: mouseLeft without selection

Add path point: mouseLeft with path selected

Remove last point: del

Change height: alt+mouseY

Zone
Add zone: mouseLeft

Adjust radius: alt+mouseY

Script
Lua reference manual

Included modules: string, math

Functions:

start_timer(id, time, function)

setup_timer(id, name)

stop_timer(id)

set_active(id, bool)

set_path(id, path_id)

set_follow(id, target_id)

set_target(id, target_id)

set_target_search(id, mode) --all, player, none

set_align(id, align) --target, enemy, ally, neutral

set_speed(id, speed)

set_speed_limit(id, max_speed)

get_height(id)

destroy(id)

setup_radio(id, name, color)

clear_radio()

add_radio(id, message, time)

set_hud_visible(bool)

mission_clear()

mission_update()

mission_fail()

How to create custom decals

Open Horizon loads decals as zip files from the folder decals/

Zip file must contain file info.xml

Example:

<!--Open Horizon example decal-->

<decal plane="f22a" base="0" name="Example">
    <image file="open_horizon.tga"/>
    <specular file="open_horizon_spec.tga"/>
    <color2 r="98" g="222" b="253"/>
</decal>
root node decal

attribute plane: plane id

attribute base: base decal index

node image, optional. Drawn on top of the base decal

attribute file: filename of the texture inside zip, must be in tga format

node specular, optional. Drawn on top of the specular map

attribute file

nodes color0 - color5, optional. Override base decal colors

attributes r, g, b: color in range 0-255.

UVMaps reference: uvmaps.zip

Resources research progress

  AC4	AC5	ACZ	AC6	AH(PC)	AH(PS3)	Infinity

Base container	CDP	PAC5	PAC5	PAC6	QDF, DPL, CPK	DPL	DPL

Compression	none	Ulz2	none	xored deflate	none, xored deflate		

Internal container	POC	POC	POC	FHM	FHM	"FHM"	"FHM"

Textures	GIM	GIM	GIM	NTXR	NTXR		

Meshes	"SM"	SM	SM	NDXR	NDXR		

Notes:
AC6 uses different FHM

Names are given by signature, extension, or made up, like, POC - Plain Old Container

Quotes mean unconfirmed assumption


Build instructions

#Getting sources 

git clone --recursive https://github.com/ACE-rakugaki/Open-Horizon-Enhanced-Edition/

#Sources update:

git pull && git submodule init && git submodule update

#Linux requirements:

cmake

freeglut3-dev

xorg-dev

build-essential

libopenal-dev

lua5.1

lua5.1-dev

glfw3

on debian-based:

sudo apt-get update && sudo apt-get install freeglut3-dev xorg-dev build-essential libopenal-dev lua5.1 lua5.1-dev libglfw3-dev cmake git

git clone --recursive https://github.com/undefined-darkness/open-horizon/

mkdir open-horizon-cmake && cd open-horizon-cmake && cmake ../open-horizon && make open_horizon

locations convert:

mkdir convert-locations-cmake && cd convert-locations-cmake && cmake ../open-horizon/convert_locations && make convert_locations

## Build status:

Windows [![Build status](https://ci.appveyor.com/api/projects/status/070bsencsnrncjmc?svg=true)](https://ci.appveyor.com/project/undefined-darkness/open-horizon)

Linux [![Build status](https://api.travis-ci.org/undefined-darkness/open-horizon.svg)](https://travis-ci.org/undefined-darkness/open-horizon)
