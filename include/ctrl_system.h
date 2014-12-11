#ifndef CTRL_SYS
#define CTRL_SYS



class ControlSystem {
	protected:
	float m_goal;
	float m_val;
	public:
		ControlSystem(){}
		~ControlSystem(){}
		void goal(float goal){this->m_goal = goal;}
		float get(){return m_val;}
		virtual float update(float curr) = 0;
};

class PIDControlSystem: public ControlSystem{
	float int_x;
	float d_x;
	float x;
	bool hit_below;
	public:
	PIDControlSystem();
	float update(float curr);
};

#endif
