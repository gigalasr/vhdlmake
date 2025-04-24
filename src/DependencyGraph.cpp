#include "DependencyGraph.hpp"
#include "Unit.hpp"
#include "Constants.hpp"
#include "Utility.hpp"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <queue>
#include <fstream>
#include <stack>
#include <random>
#include <algorithm>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace vm {
    Node::Node(const Unit& unit) : data(unit) { }

    DependencyGraph::DependencyGraph() {
        build_dag(fs::current_path());
    }

    DependencyGraph::~DependencyGraph() {
        save_cache();
    }

    void DependencyGraph::build_dag(const std::string& directory) {
        // Load cache
        json cache;
        if(fs::exists(C_CACHE_FILE)) {
            std::string cache_file(C_CACHE_FILE);
            std::ifstream file (cache_file);
            file >> cache;
        }

        // Iterate over all vhdl files
        fs::recursive_directory_iterator working_dir (directory);
        for(const auto& file_path : working_dir) {
            if(file_path.path().extension() != ".vhdl") {
                continue;
            }

            // Directory iterator uses absolute paths, so we convert them to relative here
            std::string relative_path = fs::relative(file_path, directory).string();

            Unit unit = Unit::from_file(relative_path);

            // Associate the file path with the defined entities
            for(const auto& entity : unit.definitions)  {
                this->ident_to_file[entity] = relative_path;
            }

            // Add Node to DAG
            dag[relative_path] = std::make_shared<Node>(unit);

            // Add file to change list, if hashes don't match
            if(unit.hash != cache[relative_path]) {
                std::cout << relative_path << " changed" << std::endl;
                this->changed_units.emplace(dag[relative_path]);
            }
        }

        // Resolve dependants
        for(const auto& [path, unit] : dag) {
            for(const auto& dependency : unit->data.references) {
                if(ident_to_file.find(dependency) == ident_to_file.end()) {
                    std::cout << "[WARN] Unresolved Dependency '" << dependency << "' in file " << path << std::endl;
                    continue;
                }

                const std::string& dep_file_name = this->ident_to_file[dependency];
                dag[dep_file_name]->dependants.push_back(unit);
                unit->in++;
            }

        }
    }
    
    std::vector<std::string> DependencyGraph::get_update_list() {
        std::vector<std::string> list;
        std::unordered_map<std::string, bool> visited;
        std::stack<std::shared_ptr<Node>> to_visit;
        
        // Enqueue all node with no incoming endges
        for(const auto& node : changed_units) {
            if(node->in == 0) {
                to_visit.push(node);
            }
        }

        // Perform Topological Sort using Kahn's Algorithm
        while(!to_visit.empty()) {
            std::shared_ptr<Node> node = to_visit.top();
            to_visit.pop();

            list.push_back(node->data.path);

            for(const auto& dep : node->dependants) {
                dep->in--;
                if(dep->in == 0) {
                    to_visit.push(dep);
                }
            }
        }

        return list;
    }

    void DependencyGraph::save_cache() const {
        std::string cache_file(C_CACHE_FILE);
        std::ofstream file(cache_file);

        if(!file.is_open()) {
            std::cerr << "Could not open cache file" << std::endl;
            return;
        }

        json data;
        for(const auto& unit : this->dag) {
            data[unit.first] = unit.second->data.hash;
        }

        file << data;
    }

    void DependencyGraph::debug_print() const {
        std::cout << "Ident to File: " << std::endl;
        for(const auto& node : this->ident_to_file) {
            std::cout << node.first << " -> " << node.second << std::endl;
        }

        std::cout << std::endl << "Dag Data: " << std::endl;
        for(const auto& node : this->dag) {
            std::cout << std::setw(4) << "Path: "<< node.second->data.path << std::endl;
            std::cout << std::setw(4) << "Dependants: " << std::endl;
            for(const auto& dependant : node.second->dependants) {
                std::cout << "  " << dependant->data.path << std::endl;
            }
            std::cout << std::setw(4) << "In: " << node.second->in << std::endl;

            std::cout << std::endl;
        }
    }

    std::string DependencyGraph::get_mermaid_url() const {
        std::stringstream d;
        d << "{ \"code\": \"%%{init: {\\\"flowchart\\\": {\\\"defaultRenderer\\\": \\\"elk\\\"}} }%%\\n";
        d << "flowchart BT\\n";
        d << "classDef p stroke:green\\n";
        d << "classDef c stroke:cyan\\n";
        d << "classDef t stroke:orange\\n";

        for(const auto& [path, unit] : dag) {
            d << unit->data.path;
            if(unit->data.path.starts_with("components")) {
                d << ":::c";
            } else if(unit->data.path.starts_with("testbenches")) {
                d << ":::t";
            } else if(unit->data.path.starts_with("packages")) {
                d << ":::p";
            }
            d << "\\n";

            for(const auto& dep : unit->dependants) {
                d << unit->data.path << "-->" << dep->data.path << "\\n";
            }
        }

        d << "\"}";

        std::stringstream url;
        url << "https://mermaid.live/view#base64:";
        url << base64_encode(d.str());

        return url.str();
    } 

    void DependencyGraph::invalidate() {
        this->dag.clear();
    }
    
} // namespace vm
