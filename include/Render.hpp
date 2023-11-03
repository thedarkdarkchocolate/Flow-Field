#pragma once

#include <iostream>
#include <math.h>
#include <SFML/graphics.hpp>

#define PI 3.14159265358979323846264

struct Render {

    private:

    sf::RenderTarget& window;
    float color[4] = {0, 0, 0};
    int opacity = 5;

    //Particle Shape
    sf::CircleShape c{1.f};

    //Vector Shape and Dimensions
    sf::Vector2f vecDimensions{2.f, 15.f};
    sf::RectangleShape vecShape{vecDimensions};
    sf::Vertex line[2]{};
    sf::Color particleColor {199, 24, 97, 1};

    

    public:

    Render(sf::RenderTarget& target): window{target}
    {
        vecShape.setFillColor({255, 255, 255, 50});
    }

    void renderScreen(Solver& sol, bool& drawVectors, float& t){

        //Retriving Constraints vectors and particles
        // sf::Vector3f constraints = sol.getConstraints();
        std::vector<flowVector>& vectors = sol.getVectors();
        std::vector<circleObj>& objects_ = sol.getObjects();


        //Drawing flowVectors

        if (drawVectors){

            
            
            for(flowVector& vec: vectors){
                vecShape.setOrigin(vecDimensions.x, vecDimensions.y);
                vecShape.setPosition(vec.position);
                vecShape.setRotation(getRotation(vec) + 90);
                window.draw(vecShape);
            }
        
        }

        // Drawing particles

        

        for(circleObj& obj: objects_){

            c.setFillColor(getRainbow(t/30));
            
            c.setPosition(obj.pos);
            c.setOrigin(obj.radius, obj.radius);
            // line[0] = obj.pos;
            // line[1] = obj.prevPos;
            // window.draw(line, 2, sf::Lines);
            window.draw(c);
        }

        
    }

    void changeParticleColor(sf::Color col){
        particleColor = col;
    }

    float getRotation(flowVector& vec){
        return atan2f(vec.acceleration.y, vec.acceleration.x) * 180/PI;
    }

    void changeOpacity(int& opacity_){
        opacity = opacity_;
    }

    void setParticleSize(int& size){
        c.setRadius(float(size));
    }

    void getOpactity(int& opacity_){
        opacity_ = opacity;
    }

    sf::Color getRainbow(float t)
    {
        const float r = sin(t);
        const float g = sin(t * 2.0f * PI);
        const float b = sin(t * 2.0f * PI);
        return {static_cast<uint8_t>(255.0f * r * r),
                static_cast<uint8_t>(255.0f * g * g),
                static_cast<uint8_t>(255.0f * b * b),
                static_cast<uint8_t>(opacity)};
    }

};