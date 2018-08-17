//
//  CollisionSensor.h
//  rvm
//

#ifndef CollisionSensor_h
#define CollisionSensor_h

struct CollisionSensor {
    int xPos;
    int yPos;
    int angle;
    uint8_t collided;
    uint8_t flags;
};

#endif /* CollisionSensor_h */
