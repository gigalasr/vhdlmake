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
    };

    class DependencyGraph {
    public:
        DependencyGraph(const std::string& directory, const std::string& cache_file);

        std::vector<std::string> get_update_list();

        void save_cache(const std::string& cache_file) const;
        void debug_print() const;

    private:
        void build_dag(const std::string& directory, const std::string& cache_file);

        std::unordered_map<std::string, std::shared_ptr<Node>> dag;
        std::unordered_map<std::string, std::string> ident_to_file;
        std::unordered_set<std::shared_ptr<Node>> changed_units;
    };
} // namespace vm
