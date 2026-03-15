#ifndef ACTIONS_H
#define ACTIONS_H

typedef enum ParkingAction {
    ARRIVED,
    PARKED,
    LEFT
} ParkingAction;

typedef struct ParkingEvent {
    int car_number;
    ParkingAction action;
    float wait_time;  // only relevant for PARKED action
} ParkingEvent;

#endif // ACTIONS_H