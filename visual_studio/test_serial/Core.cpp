#include "Core.h"
#include "MyImGui.h"





EngineCore::EngineCore()
{
	ImGui_s = std::make_shared<MyImGui>();
}

EngineCore::~EngineCore()
{

}

void EngineCore::Instance()
{
	ImGui_s->Instance();
}