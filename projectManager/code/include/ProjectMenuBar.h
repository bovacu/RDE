// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_MENUBAR_H
#define ENGINE_PROJECT_MENUBAR_H

#include "ProjectModules.h"

namespace engine {

    class ProjectMenuBar {
        private:
            bool show;
            float mainMenuHeight;
            ProjectModules* projectModules;

        public:
            void init(ProjectModules* _projectModules);
            void render();
            void setShow(bool _show);

            float getHeight();
    };

}

#endif //ENGINE_PROJECT_MENUBAR_H
