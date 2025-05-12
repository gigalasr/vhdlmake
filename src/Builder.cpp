#include "Builder.hpp"
#include "DependencyGraph.hpp"
#include "Constants.hpp"

#include <filesystem>
#include <iostream>



namespace fs = std::filesystem;

namespace vm {
    static int execute_command(const std::string& command) {
        int ret = system(command.c_str());
        if(!WIFEXITED(ret)) {
            return 1;
        } else {
            return WEXITSTATUS(ret);
        }
    }

    Builder::Builder() {
        if(!fs::exists(C_VCD_DIRECTORY)) {
            fs::create_directory(C_VCD_DIRECTORY);
        }
    }

    std::string Builder::cmd_compile(const std::string& file) {
        std::stringstream stream;
        stream << "ghdl -a --std=08 " << file;
        return stream.str();
    }

    std::string Builder::cmd_link(const std::string& entity) {
        std::stringstream stream;
        stream << " ghdl -e --std=08 " << entity;
        return stream.str();
    }

    std::string Builder::cmd_run(const std::string& entity) {
        std::stringstream stream;
        stream << "ghdl -r --std=08 " << entity << " --vcd=" << C_VCD_DIRECTORY << "/" << entity << ".vcd";
        return stream.str();
    }

    int Builder::build(const std::string& entity, const std::vector<std::string> update_list) {
        // No need to build if no files were changed
        if(update_list.empty()) {
            std::cerr << "[INFO] No changes" << std::endl;
        }

        // Analyze all files
        for(const auto& unit : update_list) {
            std::cerr << "[COMPILE] " << unit << std::endl;
            auto command = cmd_compile(unit);
            int ret = execute_command(command);
            if(ret) {
                return ret;
            }
        }

        // Link final entity if needed 
        if(entity != "") {
            std::cerr << "[LINK] " << entity << std::endl;
            auto command = cmd_link(entity);
            int ret = execute_command(command);
            if(ret) {
                return ret;
            }
        }

        return 0;
    }

    int Builder::run(const std::string& entity) {
        std::cerr << "[RUN] " << entity << std::endl;
        auto command = cmd_run(entity);
        return execute_command(command);
    }

    int Builder::clean() {
        fs::recursive_directory_iterator working_dir (fs::current_path());
        
        for(const auto& file : working_dir) {
            const auto extension = file.path().extension();
            if(extension == ".vcd" || extension == ".cf" || extension == ".o") {
                fs::remove(file);
                std::cerr << "[DELETE] " << file << std::endl;
            }
        }

        if(fs::exists(C_CACHE_FILE)) {
            fs::remove(C_CACHE_FILE);
            std::cerr << "[DELETE] " << C_CACHE_FILE << std::endl;
        }

        std::cerr << "[INFO] Cleaned" << std::endl;

        return 0;
    }

} // namespace vm