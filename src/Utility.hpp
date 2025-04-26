#include <string>
#include <vector>
#include <iostream>

namespace vm
{
    typedef unsigned char uchar;
    static const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    static std::string base64_encode(const std::string &in) {
        std::string out;

        int val=0, valb=-6;
        for (uchar c : in) {
            val = (val<<8) + c;
            valb += 8;
            while (valb>=0) {
                out.push_back(b[(val>>valb)&0x3F]);
                valb-=6;
            }
        }
        
        if (valb>-6) out.push_back(b[((val<<8)>>(valb+8))&0x3F]);
        while (out.size()%4) out.push_back('=');
        return out;
    }

    static std::vector<std::string> command_get_lines(const std::string& command) {
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }

        char* line = nullptr;
        size_t len = 0;
        ssize_t nread = 0;
        std::vector<std::string> lines;

        while((nread = getline(&line, &len, pipe) != EOF)) {
            std::string tmp(line);
            tmp = tmp.substr(0, tmp.find('\n'));
            lines.emplace_back(tmp);
            
        }

        fclose(pipe);
        free(line);
        return lines;
    }
} // namespace vm
