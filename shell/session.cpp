
#include "session.hpp"


Session::Session(unsigned int secondsValid) :
	m_secondsValid(secondsValid)
{
	
}

int Session::getState()
{
	return m_state;
}
