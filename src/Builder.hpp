#include <string>
#include <vector>

namespace vm {
    class Builder {
    public: 
        Builder();

        void build(const std::string& entity);
        void run(const std::string& entity);
        void clean();
        
    private:
        std::string cmd_compile(const std::string& file);
        std::string cmd_link(const std::string& entity);
        std::string cmd_run(const std::string& entity);

        std::string m_CacheFile;
        std::string m_SourceDirectory;
        std::string m_VcdDirectory;
    };


} // namespace vm