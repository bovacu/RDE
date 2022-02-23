// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_MODULES_H
#define ENGINE_PROJECT_MODULES_H

#include <string>
#include <vector>

namespace engine {

    struct Module {
        std::string name;
        bool installed;
    };

    class ProjectModules {
        private:
            bool show;
            std::vector<Module> availableModules;
            std::string GDEPath;

        public:
            void init(const std::string& _gdePath);
            void render();
            void setShow(bool _show);
            const std::vector<Module>& getModules();
    };

}

#endif //ENGINE_PROJECT_MODULES_H
