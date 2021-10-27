/*****************************************************************************
* Copyright (c) Bridgetek Pte Ltd.
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by Bridgetek.
* BRIDGETEK accept no responsibility for any issues resulting from its use.
* The developer of the final application incorporating any parts of this
* sample project is responsible for ensuring its safe and correct operation
* and for any consequences resulting from its use.
*****************************************************************************/
/**
* @file  Lift_Control.c
* @brief Lift application to demonstrate EVE primitives and widgets.
*/
#include <time.h>
#include "Platform.h"
#include "DemoLift2.h"
#include "Lift_Control.h"
#include "Common.h"

#define ENABLE_FAST_MOVE 1

#define CLOSING_TIME_MS		3	//timeout to simulate closing the door
#define OPENING_TIME_MS		3	//timeout to simulate opening the door
#define WAIT_TO_GO_MS		1	//timeout to wait to go after closing the door
#define FLOOR_MOVE_MS		1	//timeout unit to move up/down to next floor
#define WAIT_TO_CLOSE_MS	1

#define GROUND_FLOOR_NUM	2		//floor_num = 1 for B1 and floor_num = 0 for B2
floor_pad_t FloorPads[MAX_PAD_NUMBER] = {
							{.floor_num = 0,.tag = TAG_FLOOR_0,.is_touched = false, .is_active = false,},
							{.floor_num = 1,.tag = TAG_FLOOR_1,.is_touched = false, .is_active = false,},
							{.floor_num = 2,.tag = TAG_FLOOR_2,.is_touched = false, .is_active = true,},
							{.floor_num = 3,.tag = TAG_FLOOR_3,.is_touched = false, .is_active = false,},
							{.floor_num = 4,.tag = TAG_FLOOR_4,.is_touched = false, .is_active = false,},
							{.floor_num = 5,.tag = TAG_FLOOR_5,.is_touched = false, .is_active = true,},
						};

static lift_control_t Lift;
static bool Is_WaitToGo_Timeout;
uint32_t Start_Millis;
uint8_t Move_Unit_Count;
uint32_t Start_Move_Time;
void LiftControl_Init()
{
	Lift.current_floor = LIFT_START_STATE;
	Lift.move_dir = LIFT_MOVE_NONE;
	Lift.next_dst_floor = INVALID_FLOOR;
	printf("LiftControl: LIFT_START_STATE\n");
}

bool LiftControl_IsOpenActive()
{
	return (Lift.current_state == LIFT_OPENING_STATE);
}

bool LiftControl_IsCloseActive()
{
	return (Lift.current_state == LIFT_CLOSING_STATE || Lift.current_state == LIFT_WAIT_TO_CLOSE_STATE);
}

int LiftControl_GetCurrentFloor()
{
	return Lift.current_floor;
}

uint8_t LiftControl_GetCurrentState()
{
	return Lift.current_state;
}

void printLiftState(lift_state_t state) {

	switch (state)
	{
	case LIFT_OPENED_STATE:
		APP_DBG("Lift state: LIFT_OPENED_STATE");
		break;
	case LIFT_CLOSED_STATE:
		APP_DBG("Lift state: LIFT_CLOSED_STATE");
		break;
	case LIFT_OPENING_STATE:
		APP_DBG("Lift state: LIFT_OPENING_STATE");
		break;
	case LIFT_CLOSING_STATE:
		APP_DBG("Lift state: LIFT_CLOSING_STATE");
		break;
	case LIFT_MOVE_UP_STATE:
		APP_DBG("Lift state: LIFT_MOVE_UP_STATE");
		break;
	case LIFT_MOVE_DOWN_STATE:
		APP_DBG("Lift state: LIFT_MOVE_DOWN_STATE");
		break;
	case LIFT_WAIT_TO_MOVE_STATE:
		APP_DBG("Lift state: LIFT_WAIT_TO_MOVE_STATE");
		break;
	case LIFT_WAIT_TO_CLOSE_STATE:
		APP_DBG("Lift state: LIFT_WAIT_TO_CLOSE_STATE");
		break;
	default:
		APP_DBG("Lift state: Unknow");
		break;
	}
}

void UpdateDestFloor()
{
	Lift.next_dst_floor = INVALID_FLOOR;
	if (Lift.move_dir == LIFT_MOVE_UP)
	{
		for (int i = 0; i < MAX_PAD_NUMBER; i++)
		{
			//find next destination floor
			if (FloorPads[i].floor_num > Lift.current_floor && FloorPads[i].is_active)
			{
				printf("LiftControl: New next floor %d\n", FloorPads[i].floor_num);
				Lift.next_dst_floor = FloorPads[i].floor_num;
				break;
			}
		}
	}
	else if (Lift.move_dir == LIFT_MOVE_DOWN)
	{
		for (int i = MAX_PAD_NUMBER - 1; i >= 0; i--)
		{
			//find next destination floor
			if (FloorPads[i].floor_num < Lift.current_floor && FloorPads[i].is_active)
			{
				printf("LiftControl: New next floor %d\n", FloorPads[i].floor_num);
				Lift.next_dst_floor = FloorPads[i].floor_num;
				break;
			}
		}
	}

	if (Lift.next_dst_floor == INVALID_FLOOR)
	{
		Lift.move_dir = LIFT_MOVE_NONE;
	}
}

void Start_NextSimulation()
{
	static uint8_t next_move = LIFT_MOVE_DOWN;
	if (next_move == LIFT_MOVE_DOWN)
	{
		next_move = LIFT_MOVE_UP;
		for (int i = Lift.current_floor; i < MAX_PAD_NUMBER; i++)
		{
			FloorPads[i].is_active = (rand() % 2) == 1;
		}

	}
	else
	{
		next_move = LIFT_MOVE_DOWN;
		for (int i = 0; i < Lift.current_floor; i++)
		{
			FloorPads[i].is_active = (rand() % 2) == 1;
		}
	}
	Lift.move_dir = next_move;
}

#if !ENABLE_FAST_MOVE
void LiftControl_FloorTransition()
{
	enum {
		IDLE_STATE,
		MOVING_STATE,
		PLAY_BELL_STATE,
		PLAY_FLOOR_STATE,
		PLAY_MOVE_STATE,
	};
	static uint8_t move_state = IDLE_STATE;
	static uint32_t current_time;
	switch(move_state)
	{
		case IDLE_STATE:
		{
			if(AppLift2_IsAudioFinished() != true) break;
			if(Lift.current_state != LIFT_MOVE_UP_STATE && Lift.current_state != LIFT_MOVE_DOWN_STATE) break;
			time((time_t *)&Start_Move_Time);
			Move_Unit_Count = 0;
			move_state = PLAY_MOVE_STATE;
			if(Lift.move_dir == LIFT_MOVE_UP)
			{
				//TODO:Play GOING UP sound
			}
			else
			{
				//TODO:Play GOING DOWN sound
			}
		}
		break;
		case PLAY_MOVE_STATE:
			if(AppLift2_IsAudioFinished() == true) move_state = MOVING_STATE;
			break;
		case MOVING_STATE:
		{
			time((time_t *)&current_time);
			if(current_time - Start_Move_Time < FLOOR_MOVE_MS) break;
			Move_Unit_Count++;
			printf("LiftControl:Moving...\n");
			Start_Move_Time = current_time;
			if(Move_Unit_Count == 3)
			{
				if(Lift.move_dir == LIFT_MOVE_UP)
				{
					Lift.current_floor++;
				}
				else
				{
					Lift.current_floor--;
				}
				printf("LiftControl: Arrive floor %d\n", Lift.current_floor);
				if(Lift.current_floor == Lift.next_dst_floor)
				{
					//clear active state
					for(int i=0; i<MAX_PAD_NUMBER; i++)
					{
						if(FloorPads[i].floor_num == Lift.current_floor)
						{
							FloorPads[i].is_active = 0;
							break;
						}
					}
					//play bell sound to indicate destination floor
					move_state = PLAY_BELL_STATE;
					//TODO:Play BELL sound
				}
				else
				{
					//just play floor number
					for(int i=0; i<MAX_PAD_NUMBER; i++)
					{
						if(FloorPads[i].floor_num == Lift.current_floor)
						{
							//TODO:If we want to play FLOOR NUMBER sound
							break;
						}
					}
					move_state = PLAY_FLOOR_STATE;
				}
			}
		}
		break;
		case PLAY_BELL_STATE:
		{
			if(AppLift2_IsAudioFinished() == true)
			{
				for(int i=0; i<MAX_PAD_NUMBER; i++)
				{
					if(FloorPads[i].floor_num == Lift.current_floor)
					{
						//TODO:Play FLOOR NUMBER sound
						break;
					}
				}
				move_state = PLAY_FLOOR_STATE;
			}
		}
		break;
		case PLAY_FLOOR_STATE:
		{
			if(AppLift2_IsAudioFinished() == true)
			{
					move_state = IDLE_STATE;
			}

		}
		break;
		default: break;
	}
}

void LiftControl_RunStateMachine()
{
	lift_state_t next_state = Lift.current_state;
	static uint32_t current_time;
	time((time_t *)&current_time);

	printLiftState(Lift.current_state);

	switch(Lift.current_state)
	{
		case LIFT_START_STATE:
			Lift.current_floor = 0;
			next_state = LIFT_OPENED_STATE;
			break;
		case LIFT_OPENED_STATE:
			UpdateDestFloor();
			if(Lift.next_dst_floor != INVALID_FLOOR)
			{
				next_state = LIFT_WAIT_TO_CLOSE_STATE;
				Start_Millis = current_time;		//record start time to count timeout for closing the door
			}
			else
			{
				Start_NextSimulation();
			}
			break;
		case LIFT_WAIT_TO_CLOSE_STATE:
			if(current_time - Start_Millis > WAIT_TO_CLOSE_MS)
			{
				next_state = LIFT_CLOSING_STATE;
				//TODO:If we want to play DOOR CLOSE sound
				Start_Millis = current_time;
			}
			break;
		case LIFT_CLOSED_STATE:
			if(Lift.move_dir == LIFT_MOVE_NONE)
			{
				//TODO:If we want to play DOOR OPEN sound
				next_state = LIFT_OPENING_STATE;
			}
			else		//no floors to go
			{
				next_state = LIFT_WAIT_TO_MOVE_STATE;
			}
			Start_Millis = current_time;			//record start time to count timeout
			break;
		case LIFT_OPENING_STATE:
			if(current_time - Start_Millis > OPENING_TIME_MS)
			{
				next_state = LIFT_OPENED_STATE;
			}
			break;
		case LIFT_CLOSING_STATE:
			if(current_time - Start_Millis > CLOSING_TIME_MS)
			{
				next_state = LIFT_CLOSED_STATE;
			}
			break;
		case LIFT_WAIT_TO_MOVE_STATE:
			if(current_time - Start_Millis > WAIT_TO_GO_MS)
			{
				if(Lift.move_dir == LIFT_MOVE_UP)
				{
					next_state = LIFT_MOVE_UP_STATE;
				}
				else
				{
					next_state = LIFT_MOVE_DOWN_STATE;
				}
			}
			break;
		case LIFT_MOVE_UP_STATE:
		case LIFT_MOVE_DOWN_STATE:
			if(Lift.current_floor == Lift.next_dst_floor)
			{
				next_state = LIFT_ARRIVE_FLOOR_STATE;
				Start_Millis = current_time;
			}
			break;
		case LIFT_ARRIVE_FLOOR_STATE:
			if(current_time - Start_Millis > 3)
			{
				//TODO:If we want to play DOOR OPEN sound
				next_state = LIFT_OPENING_STATE;
				Start_Millis = current_time;
			}
			break;
		default: break;
	}
	if (Lift.current_state != next_state)
	{
		Lift.current_state = next_state;
	}

	printLiftState(Lift.current_state);

}
#else

void LiftControl_FloorTransition()
{
	if (Lift.current_state != LIFT_MOVE_UP_STATE && Lift.current_state != LIFT_MOVE_DOWN_STATE)
	{
		return;
	}

	if (Lift.move_dir == LIFT_MOVE_UP)
	{
		Lift.current_floor++;
	}
	else
	{
		Lift.current_floor--;
	}
	printf("LiftControl: Arrive floor %d\n", Lift.current_floor);

	if (Lift.current_floor == Lift.next_dst_floor)
	{
		//clear active state
		for (int i = 0; i < MAX_PAD_NUMBER; i++)
		{
			if (FloorPads[i].floor_num == Lift.current_floor)
			{
				FloorPads[i].is_active = 0;
				break;
			}
		}
	}
	else
	{
		//just play floor number
		for (int i = 0; i < MAX_PAD_NUMBER; i++)
		{
			if (FloorPads[i].floor_num == Lift.current_floor)
			{
				//TODO:If we want to play FLOOR NUMBER sound
				break;
			}
		}
	}
}

void LiftControl_RunStateMachine()
{
	lift_state_t next_state = Lift.current_state;

	printLiftState(Lift.current_state);

	switch (Lift.current_state)
	{
	case LIFT_START_STATE:
	case LIFT_OPENED_STATE:
	case LIFT_WAIT_TO_CLOSE_STATE:
	case LIFT_CLOSED_STATE:
	case LIFT_OPENING_STATE:
		UpdateDestFloor();
		while (Lift.next_dst_floor == INVALID_FLOOR)
		{
			Lift.move_dir = LIFT_MOVE_NONE;
			Start_NextSimulation();
			UpdateDestFloor();
		}

		if (Lift.move_dir == LIFT_MOVE_UP)
		{
			next_state = LIFT_MOVE_UP_STATE;
		}
		else
		{
			next_state = LIFT_MOVE_DOWN_STATE;
		}
		break;
	case LIFT_CLOSING_STATE:
	case LIFT_WAIT_TO_MOVE_STATE:
	case LIFT_MOVE_UP_STATE:
	case LIFT_ARRIVE_FLOOR_STATE:
	case LIFT_MOVE_DOWN_STATE:
		if (Lift.current_floor == Lift.next_dst_floor)
		{
			next_state = LIFT_OPENING_STATE;
		}
		break;
	default: break;
	}

	if (Lift.current_state != next_state)
	{
		Lift.current_state = next_state;
	}

	printLiftState(Lift.current_state);
}

#endif
