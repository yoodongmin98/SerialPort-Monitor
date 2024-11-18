#include <iostream>
#include "Core.h"





int main()
{
	std::shared_ptr<EngineCore> Cores = std::make_shared<EngineCore>();
	Cores->Instance();
	return 0;
}