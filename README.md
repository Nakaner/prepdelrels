# ACT RESPONSIBLY

This program is no invitation to delete any relation in OpenStreetMap you do
not like. You should only delete relations are malicious, outdated or both
wrong and unfixable or violate the *On the Ground Rule*. In addition, the
[Automated Edits Code of
Conduct](http://wiki.openstreetmap.org/wiki/Automated_Edits) and lots of
unwritten rules (OSM is full of them) may apply. You should not use this tool
if you are not an experienced mapper. 

PLEASE DISCUSS YOUR PLANS TO DELETE "SUPERFLUOUS" RELATIONS ON THE MAILING
LISTS IN ADVANCE!

# PrepDelRels

This repository contains a tool to help you deleting relations from OpenStreetMap.


## Dependencies
This software uses the [Osmium library](https://github.com/osmcode/libosmium)
for everything related with reading OSM data.


## License

See the [LICENSE](LICENSE) file for the license of this program.


## How it works

This program reads an unfiltered OpenStreetMap data extract and collects all
IDs of all objects which are member of a relation which have some certain
(currently hardcoded) tags. In a second pass it reads all OSM objects in the
input file and removes the ID of the objects from the list which have proper
tags (long and hardcoded list). As an result, a list of objects is written to
STDOUT. These objects have to be reverted if you delete the relations they are
member of because these objects do not have any proper tags and are not member
of any other relation (except indirectly like relations which are members of
relations and nodes which are members of a member way of a relations).


## Dependencies

* C++11 compiler
* libosmium (`libosmium-dev`) and all its [important
* dependencies](http://osmcode.org/libosmium/manual.html#dependencies)
* CMake (`cmake`)

You can install libosmium either using your package manager or just cloned from
its Github repository to any location on your disk. Take care to use libosmium
v2.x not the old Osmium v1.x!


## Building

```sh
mkdir build
cd build
cmake ..
make
```
