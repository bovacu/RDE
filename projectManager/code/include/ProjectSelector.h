// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_SELECTOR_H
#define ENGINE_PROJECT_SELECTOR_H

#include "ProjectCreator.h"

namespace engine {

    class ProjectList; class Project;
    class ProjectSelector {
        private:
            Project* project = nullptr;
            ProjectList* projectList = nullptr;
            ProjectCreator* projectCreator = nullptr;

        public:
            void init(ProjectList* _projectList, ProjectCreator* _projectCreator);
            void render();
            Project* getCurrentProject();
            void selectProject(const std::string& _projectName);
            void loadProjects();
    };

}

#endif //ENGINE_PROJECT_SELECTOR_H
