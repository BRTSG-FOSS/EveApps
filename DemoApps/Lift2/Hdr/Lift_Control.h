#ifndef LIFT_CONTROL_H
#define LIFT_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

#define INVALID_TAG			255
#define INVALID_FLOOR		255

enum
{
	TAG_FLOOR_0=1,	TAG_FLOOR_1,	TAG_FLOOR_2,
	TAG_FLOOR_3,	TAG_FLOOR_4,	TAG_FLOOR_5,
	TAG_BELL,		TAG_CALL, 		TAG_FAN,
	TAG_OPEN, 		TAG_CLOSE,
};

typedef struct
{
	int floor_num;
	uint8_t tag;
	bool is_touched;
	bool is_active;
	uint8_t sound;
}floor_pad_t;

typedef enum
{
	LIFT_START_STATE,
	LIFT_OPENED_STATE,
	LIFT_CLOSED_STATE,
	LIFT_OPENING_STATE,
	LIFT_CLOSING_STATE,
	LIFT_MOVE_UP_STATE,
	LIFT_MOVE_DOWN_STATE,
	LIFT_WAIT_TO_MOVE_STATE,
	LIFT_WAIT_TO_CLOSE_STATE,
	LIFT_ARRIVE_FLOOR_STATE,
}lift_state_t;

enum
{
	LIFT_MOVE_NONE,
	LIFT_MOVE_UP,
	LIFT_MOVE_DOWN,
};

typedef struct
{
	lift_state_t current_state;
	uint8_t move_dir;
	int current_floor;
	int next_dst_floor;
}lift_control_t;

#define MAX_PAD_NUMBER 		6

void LiftControl_SetGoUp();

#endif	//LIFT_CONTROL_H
