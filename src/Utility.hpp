#include <string>

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
} // namespace vm
