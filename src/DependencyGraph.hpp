#include "Unit.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <unordered_set>

namespace vm {
    struct Node {
        explicit Node(const Unit& unit);
        std::vector<std::shared_ptr<Node>> dependants;
        Unit data;
        int in = 0;
    };


    class DependencyGraph {
    public:
        DependencyGraph();

        std::vector<std::string> get_update_list();
        std::vector<std::string> get_minimal_subset();

        void save_cache() const;
        void debug_print() const;
        std::string get_mermaid_url(bool partial) const;

    private:
        void build_dag(const std::string& directory);
        void build_partial_dag(const std::shared_ptr<Node>& node);

        std::unordered_map<std::string, std::shared_ptr<Node>> dag;
        std::unordered_map<std::string, std::shared_ptr<Node>> partial_dag;

        std::unordered_map<std::string, std::string> ident_to_file;
        std::unordered_set<std::shared_ptr<Node>> changed_units;
    };
} // namespace vm
