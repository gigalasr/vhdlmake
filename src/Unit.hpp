#ifndef UNIT_HPP
#define UNIT_HPP

#include <vector>
#include <string>
#include <unordered_set>

namespace vm {
    struct Unit {
        std::unordered_set<std::string> references;
        std::vector<std::string> definitions;
        std::string path;
        size_t hash;

        static Unit from_file(const std::string& path);

        friend std::ostream& operator<< (std::ostream& stream, const Unit& unit);
    };
} // namespace vm

#endif
