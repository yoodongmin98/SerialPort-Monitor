#include <DebugPortBox.h>







DebugPortBox::DebugPortBox()
{

}

DebugPortBox::~DebugPortBox()
{

}

static bool isSizeInitialized = true;

void DebugPortBox::Instance(std::string _PortName, std::deque<std::string>& _RawDataLog)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.4f, 0.3f, 0.3f, 0.9f));
	ImGui::Begin(_PortName.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(_PortName.c_str());
	ImGui::BeginChild("Row Data", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	if (isSizeInitialized)
	{
		ImGui::SetWindowSize(ImVec2{ 500.0f,500.0f },1);
		isSizeInitialized = false;
	}
	for (auto V : _RawDataLog)
	{
		ImGui::Text(V.c_str());
	}
	float scrollY = ImGui::GetScrollY();      
	float scrollMaxY = ImGui::GetScrollMaxY();
	bool isAtBottom = (scrollY >= scrollMaxY);

	if (isAtBottom || (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)))
	{
		ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleColor(1);
}