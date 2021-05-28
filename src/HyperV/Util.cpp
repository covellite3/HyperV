#include "Util.hpp"
#include "cstdlib"
namespace HyperV {
	void custom_assert(bool predicat, const char* msg)
	{
		if(!predicat) { std::cout << msg << std::endl; std::abort(); }
	}
}