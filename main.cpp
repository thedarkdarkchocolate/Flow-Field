#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <sstream>
#include <string>
#include "Solver.hpp"
#include "Render.hpp"
#include "FastNoise.h"
#include <SFML/graphics.hpp>


#define WIDTH 1201
#define HEIGHT 800
#define FPS 144


void imguiMenu(Solver& sol, Render& render, sf::RenderWindow& window, int framesPerSecond, bool& drawVectors, bool& clearScreen, float val[])
{

    //Varieble to change

    ImGui::Begin("Flow Menu");

    // if (ImGui::SliderAngle("Angle ", &angle, 0, 360))
    //     sol.rotateVectors(angle);

    std::string fpsTag = "FPS: " + std::to_string(framesPerSecond);
    std::string objectCounter = "Object Counter: " + std::to_string(sol.getObjectCounter());
    int itemsToAdd = 0;
    
    ImGui::Text(fpsTag.c_str());
    ImGui::Text(objectCounter.c_str());
    

    if (ImGui::SmallButton("Add object"))
        for (int i{0}; i < 80; i++)
            sol.addObject();

    
    if (ImGui::InputInt("Items to add: ", &itemsToAdd))
        for (int i{0}; i < 500; i++)
            sol.addObject();

    if (ImGui::SmallButton("Clear objects"))
        sol.clearObjects();

    if (ImGui::SmallButton("Toggle Vectors")){
        drawVectors = !drawVectors;
        window.clear();
    }

    if(ImGui::InputFloat("Initial X value", &val[0], 0, 0, "%.4f", 0))
        sol.setInitialX(val[0]);

    if(ImGui::InputFloat("Increment X", &val[1], 0, 0, "%.4f", 0))
        sol.setIncX(val[1]);

    if(ImGui::InputFloat("Increment Y", &val[2], 0, 0, "%.4f", 0))
        sol.setIncY(val[2]);

    if(ImGui::InputFloat("Increment Z", &val[3], 0, 0, "%.4f", 0))
        sol.setIncZ(val[3]);
    
    if(ImGui::InputFloat("Magnitude ", &val[4], 0, 0, "%.4f", 0))
        sol.setMagnitude(val[4]);

    if (ImGui::SmallButton("Reset Vectors"))
        sol.resetVectors();

    if (ImGui::SmallButton("Toggle clear screen"))
        clearScreen = !clearScreen;

    sol.resetVectors();

    ImGui::End();

}


int main(){

    bool drawVectors = !true;
    bool clearScreen = !true;
    float angle = 0;
    float val [5];

    float time = 0.f;

    int framesPerSecond;
    
    sf::RenderWindow window{sf::VideoMode(WIDTH, HEIGHT), "ImGUI SFML"};
    ImGui::SFML::Init(window);

    sf::Vector2i windowDimensions{WIDTH, HEIGHT};

    window.setFramerateLimit(FPS);

    sf::Clock deltaClock;

    Render render(window);
    Solver solver(WIDTH, HEIGHT, window);
    solver.intiValues(val);

    while(window.isOpen()){
        
        sf::Event event;


        if (window.pollEvent(event)){

            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !ImGui::GetIO().WantCaptureMouse){
                
                sf::Vector2i mousePos = sf::Mouse::getPosition(window); 
                solver.aimVectors({float(mousePos.x), float(mousePos.y)});
            }


                
        }

        float dt = deltaClock.getElapsedTime().asSeconds();
        time += dt;

        framesPerSecond = int(1/dt);

        ImGui::SFML::Update(window, deltaClock.restart());

        imguiMenu(solver, render, window, framesPerSecond, drawVectors, clearScreen, val);

        if (clearScreen)
            window.clear();

        solver.update(dt);

        render.renderScreen(solver, drawVectors, time);

        ImGui::SFML::Render(window);

        window.display();

    }

    ImGui::SFML::Shutdown();
    return 0;
}


