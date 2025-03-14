#include <DebugPortBox.h>







DebugPortBox::DebugPortBox()
{

}

DebugPortBox::~DebugPortBox()
{

}



void DebugPortBox::Instance(std::string _PortName, std::deque<std::string>& _RawDataLog)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.1f, 0.1f, 0.9f));
	ImGui::Begin(_PortName.c_str(), nullptr , ImGuiWindowFlags_None || ImGuiWindowFlags_NoCollapse);
	ImGui::Text(_PortName.c_str());
	ImGui::SameLine();
	if (ImGui::Button("X"))
	{
		ImGui::End();
		ImGui::PopStyleColor(1);
		Trigger_X_Event();
		return;
	}
	if (isSizeInitialized)
	{
		ImGui::SetWindowSize(ImVec2{ 500.0f,500.0f });
		isSizeInitialized = false;
	}
	ImGui::BeginChild("Row Data", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	for (auto V : _RawDataLog)
	{
		ImGui::Text(V.c_str());
	}
	ScrollSetting();
	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleColor(1);
}



void DebugPortBox::ScrollSetting()
{
	float scrollY = ImGui::GetScrollY();
	float scrollMaxY = ImGui::GetScrollMaxY();
	bool isAtBottom = (scrollY >= scrollMaxY);

	if (isAtBottom || (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Enter)))
	{
		ImGui::SetScrollHereY(1.0f);
	}
}