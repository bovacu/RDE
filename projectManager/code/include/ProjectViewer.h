// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_VIEWER_H
#define ENGINE_PROJECT_VIEWER_H

#include "ProjectSelector.h"

namespace engine {

    class ProjectViewer {
        private:
            bool show;
            ProjectSelector* projectSelector;

        public:
            void init(ProjectSelector* _projectSelector);
            void render();
            void setShow(bool _show);
    };

}

#endif //ENGINE_PROJECT_VIEWER_H
