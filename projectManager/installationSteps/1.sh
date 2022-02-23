GDE_PATH="$1"
cd "$GDE_PATH"/GDE
git submodule init
git submodule update
cd submodules/imgui && git checkout docking

cd ../.. && sed -i -e 's/add_library(jpeg STATIC)/add_library(jpeg SHARED)/g' submodules/SDL_image/external/jpeg-9d/CMakeLists.txt
sed -i -e 's/if (ImGui::IsClippedEx(m_WidgetRect, id, false))/if (ImGui::IsClippedEx(m_WidgetRect, id))/g' submodules/imguiNodeEditor/imgui_canvas.cpp