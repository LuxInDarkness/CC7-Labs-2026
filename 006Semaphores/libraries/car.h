#ifndef CAR_H
#define CAR_H

#include "actions.h"
#include "t_args.h"

void arrived(int car_number, LineQueue *queue);
void parked(int car_number, LineQueue *queue, Semaphore *semaphore, float sleep_time, float *out_wait_time);
void left(int car_number, Semaphore *semaphore, LineQueue *queue);
void *car_work(void *arg);

#endif // CAR_H