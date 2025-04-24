#include <string>
#include <vector>

namespace vm {
    class Builder {
    public: 
        Builder();

        void build(const std::string& entity, const std::vector<std::string> update_list);
        void run(const std::string& entity);
        void clean();

    private:
        std::string cmd_compile(const std::string& file);
        std::string cmd_link(const std::string& entity);
        std::string cmd_run(const std::string& entity);
    };


} // namespace vm