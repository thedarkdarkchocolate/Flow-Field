#include <iostream>
#include <math.h>
#include <SFML/graphics.hpp>
#include "FastNoise.h"

#define PI 3.14159265358979323846264


struct circleObj {
    
    sf::Vector2f pos, prevPos, acc;
    float radius;
    static constexpr float maxSpeed = 170.f;


    circleObj(sf::Vector2f position, float& radius):
        pos{position},
        prevPos{position},
        acc{0.f, 0.f},
        radius{radius}
    {}

    ~circleObj(){}

    void update(float& dt){

        sf::Vector2f diff = pos - prevPos;

        if (getDistance(diff) > maxSpeed * dt)
            normalizeDiff(diff, maxSpeed * dt);

        prevPos = pos;

        pos += diff + (dt * dt) * acc;

        acc = {}; // Reseting acceleration 

    }

    void accelerateObj(sf::Vector2f acc_){

        acc += acc_;
    }


    float getDistance(sf::Vector2f pos1){
        return sqrtf(powf(pos1.x, 2) + powf(pos1.y, 2));
    }

    void normalizeDiff(sf::Vector2f& diff, float speed){
        
        float currSpeed = getDistance(diff);

        diff.x = diff.x / currSpeed * speed;
        diff.y = diff.y / currSpeed * speed;
    }

    void rotatePos(float angle){
        
        float x_ = prevPos.x;
        float y_ = prevPos.y;

        prevPos.x = cosf(angle)* x_ - sinf(angle) * y_;
        prevPos.y = sinf(angle)* x_ + cosf(angle) * y_;
    }


    

};

struct flowVector {


    sf::Vector2f position;
    sf::Vector2f acceleration;
    const float radius;


    flowVector(sf::Vector2f position_, sf::Vector2f acceleration_, float radius_)
    : position{position_},
      acceleration{acceleration_},
      radius{radius_}
    {} 

    void rotate(float angle){
        
        float x_ = acceleration.x;
        float y_ = acceleration.y;

        acceleration.x = cosf(angle)* x_ - sinf(angle) * y_;
        acceleration.y = sinf(angle)* x_ + cosf(angle) * y_;
    }

    

};


class Solver {

    // screen dimensions
    const int WIDTH, HEIGHT;

    // Perlin Noise 2
    FastNoise perlin;

    // perlin veriables
    //x, y init value
    // float initialX = 2;
    // float incX = 0.0010;
    // float incY = 0.07;
    // float incZ = 0.2;
    
    //x, y init value
    float initialX = 2;
    float incX = 0.0010;
    float incY = 0.08;
    float incZ = 0.05;
    // Flow Vector force
    float magnintude {550};
    
    // Flow Vector force
    // float magnintude {1000};


    int vecListLenght;
    
    // list with particles
    std::vector<circleObj> objects;

    //Object Counter
    int objectCounter;

    // list with flowVectors
    std::vector<flowVector> flowVecs;

    // Gravity
    sf::Vector2f gravity{0.f, 0.f};

    // Spacing between vectors
    const float spacing = 20.f;

    // FlowVectors radius
    float flowVecRadius = spacing * 3/4;

    float circleRadius = 10.f;

    // z
    double z = 0.0001;

    sf::RenderTarget& window;
    
    public:

    Solver(int width_, int height_, sf::RenderTarget& w_) : WIDTH{width_}, HEIGHT{height_}, window{w_}
    {
        perlin.SetSeed(rand() % 10000);
        initFlowMatrix();
        objectCounter = 0;

    } 


    void update(float& dt){

        for(circleObj& point: objects){
            point.accelerateObj(gravity);
            updateParticle(point);
            point.update(dt);
            applyConstraints(point);
        }

    }

    void updateParticle(circleObj& point){
        for(flowVector& vec: getNeighboorVectors(point))
            if(getDistance(point.pos, vec.position) < flowVecRadius){
                point.accelerateObj(vec.acceleration);
                break;
            }

    }

    std::vector<flowVector> getNeighboorVectors(circleObj& point){
        int x = ceil(int(point.pos.x)/spacing);
        int y = ceil(int(point.pos.y)/spacing);

        int numOfXElements = ceil(WIDTH/spacing);

        std::vector<flowVector> tmpV;


            // x, y
            if (0 <= x + y * numOfXElements - 1 && x + y * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[x + y * numOfXElements - 1]);

            // x + 1, y
            if (0 <= (x + 1) + y * numOfXElements - 1 && (x + 1) + y * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[(x + 1) + y * numOfXElements - 1]);

            // x - 1, y
            if (0 <= (x - 1) + y * numOfXElements - 1 && (x - 1) + y * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[(x - 1) + y * numOfXElements - 1]);

            // x, y + 1
            if (0 <= x + (y + 1) * numOfXElements - 1 && x + (y + 1) * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[x + (y + 1) * numOfXElements - 1]);

            // x, y - 1
            if (0 <= x + (y - 1) * numOfXElements - 1 && x + (y - 1) * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[x + (y - 1) * numOfXElements - 1]);

            // x + 1, y + 1
            if (0 <= (x + 1) + (y + 1)  * numOfXElements - 1 && (x + 1) + (y + 1)  * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[(x + 1) + (y + 1)  * numOfXElements - 1]);

            // x - 1, y - 1
            if (0 <= (x - 1) + (y - 1)  * numOfXElements - 1 && (x - 1) + (y - 1)  * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[(x - 1) + (y - 1)  * numOfXElements - 1]);

            // x - 1, y + 1
            if (0 <= (x - 1) + (y + 1)  * numOfXElements - 1 && (x - 1) + (y + 1)  * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[(x - 1) + (y + 1)  * numOfXElements - 1]);

            // x + 1, y - 1
            if (0 <= (x + 1) + (y - 1)  * numOfXElements - 1 && (x + 1) + (y - 1)  * numOfXElements - 1 < vecListLenght)
                tmpV.push_back(flowVecs[(x + 1) + (y - 1)  * numOfXElements - 1]);



        

        return tmpV;
    }

    void initFlowMatrix(){
        
        double x = initialX;
        double y = x;

        for(int i = 0; i * spacing < HEIGHT + spacing; i++){
            for(int j = 0; j * spacing < WIDTH; j++){
                flowVector f({j * spacing, i * spacing}, {magnintude, 0.f} , flowVecRadius);
                // f.rotate(perlin.GetValue(y * i/600, y + x * j/400, z) * 2 * PI);
                f.rotate(perlin.GetPerlin(j * (4 + x), i * (4 + y), z) * 2 * PI);
                flowVecs.push_back(f);
                x += incX;
            }
            y += incY; 
        }
        z += incZ;
        vecListLenght = flowVecs.size();
    }

    void setInitialX(float& val){
        initialX = val;
    }

    void setIncX(float& val){
        incX = val;
    }

    void setIncY(float& val){
        incY = val;
    }

    void setIncZ(float& val){
        incZ = val;
    }

    void setMagnitude(float& val){
        magnintude = val;
    }

    float getDistance(sf::Vector2f& pos1, sf::Vector2f& pos2){
        return sqrtf(powf(pos2.x - pos1.x, 2) + powf(pos2.y - pos1.y, 2));
    }


    void aimVectors(sf::Vector2f pos){
        for(flowVector& vec: flowVecs){
            vec.acceleration = pos - vec.position;
        }
    }

    void rotateVectors(float angle){
        
        for(flowVector& vec: flowVecs){
            vec.acceleration = {magnintude, 0.f};
            vec.rotate(angle);
        }
    }


    void resetVectors(){
        for(flowVector& vec: flowVecs){
            vec.~flowVector();
        }
        flowVecs.clear();
        initFlowMatrix();
    }

    void clearObjects(){
        for(circleObj& obj: objects)
            obj.~circleObj();
        objects.clear();
        objectCounter = 0;
    }

    float getRadius(){
        return this->flowVecRadius;
    }

    int getObjectCounter(){
        return this->objectCounter;
    }

    std::vector<circleObj>& getObjects(){
        return this->objects;
    }

    std::vector<flowVector>& getVectors(){
        return this->flowVecs;
    }

    void addObject(){

        // circleObj tmp {{abs(float(rand() % 100)), float(rand() % HEIGHT)}, circleRadius};
        circleObj tmp {{float(rand() % WIDTH), float(rand() % HEIGHT)}, circleRadius};

        // tmp.setVelocity({100.f, 0.f}, .01f);
        objectCounter++;
        objects.push_back(tmp);
        
    }
    

    void applyConstraints(circleObj& point){

        if(point.pos.x - circleRadius > WIDTH){
            point.pos.x -= WIDTH;
            point.prevPos.x -= WIDTH;
        }
        if(point.pos.x - circleRadius < 0){
            point.pos.x += WIDTH;
            point.prevPos.x += WIDTH;
        }
        if(point.pos.y - circleRadius > HEIGHT){
            point.pos.y -= HEIGHT;
            point.prevPos.y -= HEIGHT;
        }
        if(point.pos.y - circleRadius < 0){
            point.pos.y += HEIGHT;
            point.prevPos.y += HEIGHT;
        }

    }

    void intiValues(float* vals){
        vals[0] = initialX;
        vals[1] = incX;
        vals[2] = incY;
        vals[3] = incZ;
        vals[4] = magnintude;
    }


};



