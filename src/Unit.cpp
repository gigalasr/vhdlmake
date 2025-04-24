#include "Unit.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace vm {

    enum class ParserState {
        TOP_LEVEL = 0,
        ENTITY_DECL, 
        PACKAGE_DECL,
        ARCH_DECL,
        ARCH_STATE,
    };

    static std::stringstream read_file(const std::string& path) {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer;
    }

    static std::vector<std::string> tokenize(std::stringstream& stream) {
        std::vector<std::string> tokens;
        std::string tmp;
        while(stream >> tmp) {
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
            tokens.emplace_back(tmp);
        }
        return tokens;
    }

    static std::string parse_reference(const std::string& ref) {
        size_t first = 0;
        size_t last = 0;
        for(size_t i = 0; i < ref.length(); i++) {
            if(ref[i] == '.' && first == 0) {
                first = i + 1;
            } else if(!isalpha(ref[i]) && ref[i] != '_') {
                last = i;
                break;
            }
        }

        if(first == 0 && last == 0) {
            return ref;
        } 

        return ref.substr(first, last - first);
    }

    Unit Unit::from_file(const std::string& path) {
        static std::hash<std::string> hasher; 
        std::stringstream buffer = read_file(path);
        
        Unit unit {
            .path = path,
            .hash = hasher(buffer.str())
        };

        std::vector<std::string> tokens = tokenize(buffer);

        ParserState state = ParserState::TOP_LEVEL;
        for(int i = 0; i < tokens.size() - 1; i++) {
            std::string& a = tokens[i];
            std::string& b = tokens[i+1];

            switch (state)
            {
                case ParserState::TOP_LEVEL:
                    if(a == "use") {
                        if(b.starts_with("iee")) { continue; } // ignore standard lib
                        unit.references. emplace(parse_reference(b));
                    } else if(a == "entity") {
                        unit.definitions.emplace_back(b);
                        state = ParserState::ENTITY_DECL;
                    } else if(a == "package") {
                        unit.definitions.emplace_back(b);
                        state = ParserState::PACKAGE_DECL;
                    } else if (a == "architecture") {
                        state = ParserState::ARCH_DECL;
                    }
                    break;
                case ParserState::PACKAGE_DECL:
                    if(a == "end") {
                        state = ParserState::TOP_LEVEL;
                        i++;
                    }
                    break;
                case ParserState::ENTITY_DECL:
                    if(a == "end") {
                        state = ParserState::TOP_LEVEL;
                        i++;
                    }
                    break;
                case ParserState::ARCH_DECL:
                    if(a == "begin") {
                        state = ParserState::ARCH_STATE;
                    } else if (a == "component") {
                        unit.references.emplace(parse_reference(b));
                    }
                    break;
                case ParserState::ARCH_STATE:
                    if(a == "end") {
                        state = ParserState::TOP_LEVEL;
                    } else if(a == "entity") {
                        unit.references.emplace(parse_reference(b));
                    }
                    break;
            }

        }

        return unit;
    }

    std::ostream& operator<< (std::ostream& stream, const Unit& unit) {
        stream << "File: " << unit.path << std::endl;

        stream << "References (" << unit.references.size() << ")" << std::endl;
        for(const auto& x : unit.references) {
            stream << "    " << x << std::endl;
        }

        stream << "Definitions (" << unit.definitions.size() << ")" << std::endl;
        for(const auto& x : unit.definitions) {
            stream << "    " << x << std::endl;
        }

        return stream;
    }


} //namespace vm