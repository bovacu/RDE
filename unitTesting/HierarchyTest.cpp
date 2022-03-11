// Created by borja on 11/3/22.

#ifdef RUN_TESTS

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "core/scene/Scene.h"
#include "unitTesting/doctest.h"
#include "core/util/Logger.h"

using namespace engine;

TEST_CASE("Scene Node Creation") {
    Scene scene("SceneTest0");
    CHECK_EQ(scene.toString(), R"(* SceneTest0)");
    scene.createNode("A");
    CHECK_EQ(scene.toString(), R"(* SceneTest0
|__ * A)");
    scene.createNode("B");
    scene.createNode("C");
    scene.createNode("D");
    scene.createNode("E");
    CHECK_EQ(scene.toString(), R"(* SceneTest0
|__ * A
|__ * B
|__ * C
|__ * D
|__ * E)");

    scene.createNode("F", scene.getNode("B"));
    CHECK_EQ(scene.toString(), R"(* SceneTest0
|__ * A
|__ * B
	|__ * F
|__ * C
|__ * D
|__ * E)");
    scene.createNode("G", scene.getNode("D"));
    scene.createNode("H", scene.getNode("B"));
    scene.createNode("I", scene.getNode("F"));
    CHECK_EQ(scene.toString(), R"(* SceneTest0
|__ * A
|__ * B
	|__ * F
		|__ * I
	|__ * H
|__ * C
|__ * D
	|__ * G
|__ * E)");
    scene.createNode("J", scene.getNode("E"));
    scene.createNode("K", scene.getNode("E"));
    scene.createNode("L", scene.getNode("K"));
    CHECK_EQ(scene.toString(), R"(* SceneTest0
|__ * A
|__ * B
	|__ * F
		|__ * I
	|__ * H
|__ * C
|__ * D
	|__ * G
|__ * E
	|__ * J
	|__ * K
		|__ * L)");
}

#endif