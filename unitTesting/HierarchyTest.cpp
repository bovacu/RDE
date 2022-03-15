// Created by borja on 11/3/22.

#ifdef RUN_TESTS

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "core/scene/Scene.h"
#include "unitTesting/doctest.h"
#include "core/util/Logger.h"

using namespace engine;

TEST_CASE("Scene Node Creation") {
    Scene scene("SceneCreateTest");
    CHECK_EQ(scene.toString(), R"(* SceneCreateTest)");
    scene.createNode("A");
    CHECK_EQ(scene.toString(), R"(* SceneCreateTest
|__ * A)");
    scene.createNode("B");
    scene.createNode("C");
    scene.createNode("D");
    scene.createNode("E");
    CHECK_EQ(scene.toString(), R"(* SceneCreateTest
|__ * A
|__ * B
|__ * C
|__ * D
|__ * E)");

    scene.createNode("F", scene.getNode("B"));
    CHECK_EQ(scene.toString(), R"(* SceneCreateTest
|__ * A
|__ * B
	|__ * F
|__ * C
|__ * D
|__ * E)");
    scene.createNode("G", scene.getNode("D"));
    scene.createNode("H", scene.getNode("B"));
    scene.createNode("I", scene.getNode("F"));
    CHECK_EQ(scene.toString(), R"(* SceneCreateTest
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
    CHECK_EQ(scene.toString(), R"(* SceneCreateTest
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

TEST_CASE("Scene Node Removal 0") {
    Scene scene("SceneRemoveTest");
    scene.createNode("A");
    scene.createNode("B");
    scene.createNode("C");
    scene.createNode("D");
    scene.removeNode(scene.getNode("C"));

    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * A
|__ * B
|__ * D)");

    scene.removeNode(scene.getNode("D"));

    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * A
|__ * B)");

    scene.removeNode(scene.getNode("A"));

    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * B)");

    scene.removeNode(scene.getNode("B"));

    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest)");

}

TEST_CASE("Scene Node Removal 1") {
    Scene scene("SceneRemoveTest");
    scene.createNode("A");
    scene.createNode("B");
    scene.createNode("C");
    scene.createNode("T");

    scene.createNode("D", scene.getNode("A"));
    scene.createNode("E", scene.getNode("A"));
    scene.createNode("F", scene.getNode("C"));
    scene.createNode("G", scene.getNode("C"));
    scene.createNode("H", scene.getNode("C"));
    scene.createNode("I", scene.getNode("G"));
    scene.createNode("J", scene.getNode("T"));
    scene.createNode("K", scene.getNode("T"));
    scene.createNode("L", scene.getNode("K"));

    scene.removeNode(scene.getNode("K"));
    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * A
	|__ * D
	|__ * E
|__ * B
|__ * C
	|__ * F
	|__ * G
		|__ * I
	|__ * H
|__ * T
	|__ * J)");

    scene.removeNode(scene.getNode("C"));
    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * A
	|__ * D
	|__ * E
|__ * B
|__ * T
	|__ * J)");

    scene.removeNode(scene.getNode("T"));
    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * A
	|__ * D
	|__ * E
|__ * B)");

    scene.removeNode(scene.getNode("E"));
    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * A
	|__ * D
|__ * B)");

    scene.removeNode(scene.getNode("B"));
    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest
|__ * A
	|__ * D)");

    scene.removeNode(scene.getNode("A"));
    CHECK_EQ(scene.toString(), R"(* SceneRemoveTest)");

}

TEST_CASE("Get Inexistent Node") {
    Scene scene("Non-Existent Node");
    CHECK_THROWS(std::runtime_error("Tried to get node with tag 'A' but it was not found!"), scene.getNode("A"));
}

TEST_CASE("Creation Time") {
    Scene scene0("Timing 0");

    Clock _clock;
    for(int _i = 0; _i < 1000; _i++)
        scene0.createNode(std::to_string(_i));

    CHECK_LE(_clock.getElapsedTimeMs(), 10);

    _clock.restart();

    Scene scene1("Timing 1");

    for(int _i = 0; _i < 10000; _i++)
        scene1.createNode(std::to_string(_i));

    CHECK_LE(_clock.getElapsedTimeMs(), 80);

    _clock.restart();

    Scene scene2("Timing 2");

    for(int _i = 0; _i < 100000; _i++)
        scene2.createNode(std::to_string(_i));

    CHECK_LE(_clock.getElapsedTimeMs(), 600);

}

#endif