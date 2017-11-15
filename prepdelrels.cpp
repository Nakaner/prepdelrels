/*

This file is part of PrepDelRels.

Copyright 2017 Michael Reichert <sourcecode@michreichert.de> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <osmium/io/reader.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/handler.hpp>

#include <osmium/osm/relation.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/node.hpp>

#include <iostream>

#include <osmium/visitor.hpp>

#include "election_relmanager.hpp"

template <typename T>
void copy_vector_to_set(std::vector<T>& vec, std::unordered_set<T>& newset) {
    if (vec.size() == 0) {
        // nothing to sort and copy
        return;
    }
    // sort vector
    std::sort(vec.begin(), vec.end());
    // insert first value. We don't do this in a loop to ensure that last_value is properly initialized.
    T last_value = *(vec.cbegin());
    newset.insert(last_value);
    // insert the other elements
    for (typename std::vector<T>::const_iterator it = vec.cbegin() + 1; it != vec.cend(); ++it) {
        if (last_value != *it) {
            newset.insert(*it);
            last_value = *it;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments.\n" \
                     "Usage: " << argv[0] << " INPUT_FILE\n";
        exit(1);
    }
    osmium::io::File input_file{argv[1]};

    std::vector<osmium::object_id_type> node_ids;
    std::vector<osmium::object_id_type> way_ids;
    std::vector<osmium::object_id_type> relation_ids;

    ElectionRelManager collector(node_ids, way_ids, relation_ids);
    std::cerr << "read relations (1)\n";
    osmium::io::Reader reader{input_file};
    osmium::ProgressBar progress{reader.file_size(), osmium::util::isatty(2)};
    osmium::relations::read_relations(progress, input_file, collector);
    progress.remove();

    std::cerr << "populate sets (2)\n";
    while (osmium::memory::Buffer buffer = reader.read()) {
        progress.update(reader.offset());
        osmium::apply(buffer, collector.handler());

    }
    reader.close();
    progress.remove();

    std::cerr << "sort vectors containing all member IDs of the boundary relations (3)\n";
    std::cerr << "nodes ...\n";
    USet node_set;
    copy_vector_to_set(node_ids, node_set);
    std::cerr << "ways ...\n";
    USet way_set;
    copy_vector_to_set(way_ids, way_set);
    std::cerr << "relations ...\n";
    USet relation_set;
    copy_vector_to_set(relation_ids, relation_set);

    osmium::io::Reader reader2{input_file};
    std::cerr << "clean sets (4)\n";

    HandlerPass2 handler2(node_set, way_set, relation_set);

    while (osmium::memory::Buffer buffer = reader2.read()) {
        progress.update(reader2.offset());
        osmium::apply(buffer, handler2);
    }
    progress.done();
    reader2.close();

    // write output
    std::cout << node_set.size() << " nodes\n";
    for (auto id : node_set) {
        std::cout << "node " << id << " only referenced by candidates of deletion\n";
    }
    std::cout << way_set.size() << " ways\n";
    for (auto id : way_set) {
        std::cout << "way " << id << " only referenced by candidates of deletion\n";
    }
    std::cout << relation_set.size() << " relations\n";
    for (auto id : relation_set) {
        std::cout << "relation " << id << " only referenced by candidates of deletion\n";
    }
}