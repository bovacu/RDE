// Created by borja on 23/2/22.

#include "projectManager/code/include/ProjectViewer.h"

namespace engine {


    void ProjectViewer::init(ProjectSelector* _projectSelector) {
        projectSelector = _projectSelector;
    }

    void ProjectViewer::render() {
        if(ImGui::BeginChild("RightPanel", {-1, -1}, true)) {
            ImGui::EndChild();
        }
    }

    void ProjectViewer::setShow(bool _show) {

    }
}
