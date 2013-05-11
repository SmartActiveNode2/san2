/* ===============================================================  @
*  DragonSRP, C++ library implementing Secure Remote Protocol       *
*  Pavel Slechta <slechta@email.cz>                                 *
@  =============================================================== */

#include "mathinterface.hpp"

namespace DragonSRP
{
	MathInterface::MathInterface(HashInterface &hashInterface, Ng ngVal)
		: hash(hashInterface),
		  ng(ngVal)
	{
		
	}
	
	// Virtual destructor
	MathInterface::~MathInterface()
	{
		// emtpy
	}
}
