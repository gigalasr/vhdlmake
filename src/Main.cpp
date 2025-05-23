#include <iostream>

#include "Builder.hpp"
#include "Unit.hpp"
#include "DependencyGraph.hpp"

#define VHDLMAKE_VERSION "0.1.2"

static void help() {
    std::cout << "List of commands:" << std::endl;
    std::cout << "vhdlmake build [entity] - builds project and optionaly elaborates <entity>" << std::endl;
    std::cout << "vhdlmake run <entity>   - builds project and runs <entity>"  << std::endl;
    std::cout << "vhdlmake info <entity>  - show info for <entity>"  << std::endl;
    std::cout << "vhdlmake graph          - get dependency graph as mermaid url"  << std::endl;
    std::cout << "vhdlmake graph*         - get partial dependency graph as mermaid url (only updated files and deps)"  << std::endl;
    std::cout << "vhdlmake subset         - get list of changed files and their dependencies"  << std::endl;
}


int main(int argc, char *argv[]) {
    std::cerr << "vhdlmake v" << VHDLMAKE_VERSION;

    #ifdef DEBUG
        std::cerr << " DEBUG BUILD " << std::endl;
    #else
        std::cerr << std::endl;
    #endif

    if(argc < 2) {
        help();
        return EXIT_FAILURE;
    }

    std::string command = argv[1];
    std::string entity;

    if(argc > 2) {
        entity = argv[2];
    }


    vm::Builder builder;
    vm::DependencyGraph graph;

    if(command == "build") {
        if(builder.build(entity, graph.get_update_list())) {
            return EXIT_FAILURE;
        }

        graph.save_cache();
    } else if (command == "run") {
        if(argc != 3) {
            std::cout << "Please provide an entity to run" << std::endl;
            return EXIT_FAILURE;
        }

        if(builder.build(entity, graph.get_update_list())) {
            return EXIT_FAILURE;
        }

        if(builder.run(entity)) {
            return EXIT_FAILURE;
        }

        graph.save_cache();
    } else if(command == "clean") {
        return builder.clean();
    } else if (command == "info") {
        if(argc != 3) {
            std::cout << "Please provide a file to show info for" << std::endl;
            return EXIT_FAILURE;
        }

        vm::Unit unit = vm::Unit::from_file(entity);
        std::cout << unit << std::endl;

    } else if(command == "graph") {
       std::cout << graph.get_mermaid_url(false) << std::endl;
    } else if(command == "graph*") {
       std::cout << graph.get_mermaid_url(true) << std::endl;
    } else if(command == "subset") {
        for(const auto& changed : graph.get_minimal_subset()) {
            std::cout << changed << " ";
        } std::cout << std::endl;
    } else {
        help();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}  