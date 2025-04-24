#include "DependencyGraph.hpp"
#include "Unit.hpp"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <queue>
#include <fstream>
#include <stack>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace vm {
    Node::Node(const Unit& unit) : data(unit) { }

    DependencyGraph::DependencyGraph(const std::string& directory, const std::string& cache_file) {
        build_dag(directory, cache_file);
    }

    void DependencyGraph::build_dag(const std::string& directory, const std::string& cache_file) {
        // Load cache
        json cache;
        if(fs::exists(cache_file)) {
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
            }

        }
    }

    static void insert_into_list(std::vector<std::shared_ptr<Node>>&list, const std::shared_ptr<Node>& to_insert) {
        for(int i = 0; i < list.size(); i++) {

            // Insert before the first dependant
            for(int j = 0; j < to_insert->dependants.size(); j++) {
                if(list[i] == to_insert->dependants[j]) {
                    list.insert(list.begin() + i, to_insert);
                    return;
                }
            }
        }

        list.push_back(to_insert);
    }

    std::vector<std::string> DependencyGraph::get_update_list() {
        // Remove all units from the changed_list that appear as a dependant
        // in a unit that was changed
        // We do this to avoid compiling a in the wrong order
        std::unordered_set<std::shared_ptr<Node>> cu_tmp(changed_units);
        for(const auto& node : cu_tmp) {
            for(const auto& dep : node->dependants) {
                changed_units.erase(dep);
            }
        }
        
        std::vector<std::shared_ptr<Node>> list;
        std::unordered_map<std::string, bool> visited;
        std::stack<std::shared_ptr<Node>> to_visit;

        for(const auto& unit : changed_units) {
            to_visit.push(unit);
        }

        while(!to_visit.empty()) {
            std::shared_ptr<Node> unit = to_visit.top();
            to_visit.pop();

            if(visited[unit->data.path]) {
                continue;
            }

            for(const auto& dep : unit->dependants) {
                if(!visited[dep->data.path]) {
                    to_visit.push(dep);
                }
            }

            insert_into_list(list, unit);
            visited[unit->data.path] = true;
        }

        std::vector<std::string> final_list;
        final_list.reserve(list.size());
        std::transform(list.begin(), list.end(), std::back_inserter(final_list), [](const std::shared_ptr<Node>& n) {
            return n->data.path;
        });

        return final_list;
    }

    void DependencyGraph::save_cache(const std::string& cache_file) const {
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
            std::cout << std::setw(4) << " path: "<< node.second->data.path << std::endl;
            std::cout << std::setw(4) << "dependants: " << std::endl;
            for(const auto& dependant : node.second->dependants) {
                std::cout << "  " << dependant->data.path << std::endl;
            }

            std::cout << std::endl;
        }
    }
    
} // namespace vm
