#pragma once

enum VehicleType { vtNone, vtSlowTurtle, vtFastTurtle, vtOwl, vtSlowUFO, vtFastUFO };

const int MAX_SPEED = 7;
const int MAX_POWER = 7;
const int VEHICLE_SPEED[] = { 0, 1, 7, 4, 1, 7 };

struct Ability
{
	int nPopoPower;
	int nPopoMax;
	int nUsedPopo;
	int nSpeed;
	bool bKick;
	VehicleType Vehicle; 
};