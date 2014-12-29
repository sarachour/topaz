#include "ctrl_system.h"
#include "stdio.h"
#include "util.h"

PIDControlSystem::PIDControlSystem() : ControlSystem(){
	this->d_x = 0;
	this->int_x = 0;
	this->x = 0;
	this->m_val = 1;
	this->hit_below = false;
}
PIDControlSystem::~PIDControlSystem(){
	
}
float PIDControlSystem::update(float curr){
	const float K = 1;
	const float KD = 0.5;
	const float PHASE = 0.0001;
	const float KI = 0.01;
	
	if(curr < this->m_goal){
		this->hit_below = true;
	}
	
	if(isNaN(curr) || isNaN(this->m_val) || !this->hit_below){
		return this->m_val;
	}
	float g = this->m_goal;
	float e = curr - g; // max 1
	float de = e - this->x; // max 1
	float inte = e*(PHASE) + this->int_x*(1.0 - PHASE); // max 1*1000
	
	//large, higher re-execution rate, grow bounds
	//low, lower rexection rate, shrink bounds.
	float correction = K*e + KI*inte + KD*de;
	
	this->m_val += correction*0.001;
	this->d_x = de;
	this->int_x = inte;
	this->x = e;
	return this->m_val;
}
