#pragma once
#include <imgui.h>

//���
#define ZERO 0
#define XYZERO { 0.0f,0.0f }
#define MYThreadCount 6


//ImVec(color,size)
#define ButtonSize ImVec2{130,20}
#define PORTBOXCOLOR ImVec4(0.3f, 0.2f, 0.4f, 0.4f)
#define REDCOLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define YELLOWCOLOR ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define BLUECOLOR ImVec4(0.0f, 1.0f, 1.0f, 1.0f)

#define PORTVIEWBACKGROUND IM_COL32(205, 205, 205, 128);


//View Size
#define MINI_PORTVIEWSIZE_X 1200.0f
#define MINI_PORTVIEWSIZE_Y 780.0f
#define NORMAL_PORTVIEWSIZE_X 1500.0f
#define NORMAL_PORTVIEWSIZE_Y 1011.0f

#define WINDOWSIZE_SMALL_X 1500.0f
#define WINDOWSIZE_NORMAL_X 1800.0f
#define WINDOWSIZE_SMALL_Y 820.0f
#define WINDOWSIZE_NORMAL_Y 1050.0f

#define WINDOW_CHECK_SIZE 1700.0f





#define Line_1 1
#define Line_3 3
#define Line_4 4

#define PACKET_ID 2

//Red �迭
#define Im4Red         ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define Im4VividRed    ImVec4(0.95f, 0.15f, 0.15f, 1.0f)

#define Im4Orange      ImVec4(1.0f, 0.55f, 0.0f, 1.0f)
#define Im4LightOrange ImVec4(1.0f, 0.733f, 0.0f, 1.0f)
#define Im4PastelOrange ImVec4(1.0f, 0.85f, 0.7f, 1.0f)
#define Im4VividOrange ImVec4(1.0f, 0.45f, 0.1f, 1.0f)


//Green �迭
#define Im4Green       ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define Im4LimeGreen	   ImVec4(0.239f, 0.820f, 0.325f, 1.0f)
#define Im4ForestGreen	   ImVec4(0.102f, 0.576f, 0.173f, 1.0f)
#define Im4PastelGreen    ImVec4(0.6f, 0.98f, 0.6f, 1.0f)
#define Im4VividGreen     ImVec4(0.157f, 0.698f, 0.239f, 1.0f)






//Blue�迭
#define Im4Blue        ImVec4(0.0f, 0.0f, 1.0f, 1.0f)
#define Im4Cyan        ImVec4(0.0f, 1.0f, 1.0f, 1.0f)
#define Im4Magenta     ImVec4(1.0f, 0.0f, 1.0f, 1.0f)
#define Im4Yellow      ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define Im4LightYellow ImVec4(1.0f, 0.776f, 0.161f, 1.0f)
#define Im4AlphaYellow ImVec4(1.0f, 0.876f, 0.861f, 0.95f)
#define Im4MusTard		   ImVec4(0.941f, 0.690f, 0.0f, 1.0f)
#define Im4Purple      ImVec4(0.5f, 0.0f, 0.5f, 1.0f)
#define Im4EdgePurple	   ImVec4(0.467f, 0.271f, 0.968f, 1.0f)
#define Im4DeepPurple	ImVec4(0.376f, 0.204f, 0.818f, 1.0f)


//��ä�� �迭
#define Im4White       ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define Im4Black       ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define Im4Gray        ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
#define Im4LightGray   ImVec4(0.75f, 0.75f, 0.75f, 1.0f)
#define Im4DarkGray    ImVec4(0.25f, 0.25f, 0.25f, 1.0f)
#define Im4WhiteGray   ImVec4(0.90f, 0.90f, 0.90f, 1.0f)


//�Ľ��� ��
#define Im4PastelPink     ImVec4(1.0f, 0.8f, 0.86f, 1.0f)
#define Im4PastelBlue     ImVec4(0.68f, 0.85f, 0.9f, 1.0f)
#define Im4PastelPurple   ImVec4(0.8f, 0.7f, 0.9f, 1.0f)
#define Im4PastelYellow   ImVec4(1.0f, 1.0f, 0.8f, 1.0f)
#define Im4Lavender       ImVec4(0.902f, 0.902f, 0.980f, 1.0f)
#define Im4PastelLilac    ImVec4(0.78f, 0.64f, 0.78f, 1.0f) 
#define Im4Mint           ImVec4(0.74f, 0.98f, 0.78f, 1.0f)
#define Im4PastelMint     ImVec4(0.7f, 1.0f, 0.85f, 1.0f)   // ��Ʈ��
#define Im4SkyBlue        ImVec4(0.53f, 0.81f, 0.92f, 1.0f)
#define Im4RoyalBlue	  ImVec4(0.243f, 0.553f, 0.957f, 1.0f)
#define Im4LightRoyalBlue ImVec4(0.369f, 0.643f, 1.0f, 1.0f)
#define Im4DeepBlue		  ImVec4(0.181f, 0.475f, 0.865f, 1.0f)
#define Im4Peach          ImVec4(1.0f, 0.9f, 0.8f, 1.0f)
#define Im4PastelCoral    ImVec4(1.0f, 0.7f, 0.6f, 1.0f)    // ���� �ڶ�
#define Im4PastelLemon    ImVec4(1.0f, 1.0f, 0.75f, 1.0f)   // �����
#define Im4PastelAqua     ImVec4(0.64f, 0.94f, 0.96f, 1.0f) // �������

#define Im4LightPurple ImVec4(0.588f, 0.431f, 0.994f, 1.0f)


//����ImVec4(0.157f, 0.698f, 0.239f, 1.0f)

#define Im4Navy         ImVec4(0.0f, 0.0f, 0.5f, 1.0f)
#define Im4Maroon       ImVec4(0.5f, 0.0f, 0.0f, 1.0f)
#define Im4Olive        ImVec4(0.5f, 0.5f, 0.0f, 1.0f)
#define Im4Teal         ImVec4(0.0f, 0.5f, 0.5f, 1.0f)
#define Im4DarkCyan     ImVec4(0.0f, 0.55f, 0.55f, 1.0f)
#define Im4SteelBlue    ImVec4(0.27f, 0.51f, 0.71f, 1.0f)
#define Im4Indigo       ImVec4(0.29f, 0.0f, 0.51f, 1.0f)
#define Im4Crimson      ImVec4(0.86f, 0.08f, 0.24f, 1.0f)


//���� ������ �÷�


#define Im4VividBlue         ImVec4(0.2f, 0.4f, 1.0f, 1.0f)
#define Im4VividPurple       ImVec4(0.6f, 0.3f, 1.0f, 1.0f)
#define Im4VividPink         ImVec4(1.0f, 0.2f, 0.6f, 1.0f)
#define Im4VividTurquoise    ImVec4(0.0f, 0.85f, 0.8f, 1.0f)



struct MyWindow
{
	MyWindow() = default;

	MyWindow& Size(const ImVec2& size, ImGuiCond cond = 0)
	{
		ImGui::SetNextWindowSize(size, cond);
		return *this;
	}

	MyWindow& Pos(const ImVec2& pos, ImGuiCond cond = 0)
	{
		ImGui::SetNextWindowPos(pos, cond);
		return *this;
	}

	MyWindow& BgAlpha(float alpha)
	{
		ImGui::SetNextWindowBgAlpha(alpha);
		return *this;
	}
};



struct ImGuiBegin
{
	bool opened = false;

	ImGuiBegin(const char* name, bool movable = false, bool* p_open = nullptr)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

		if (movable)
			flags &= ~ImGuiWindowFlags_NoMove;

		opened = ImGui::Begin(name, p_open, flags);

	}

	~ImGuiBegin()
	{
		if (opened)
			ImGui::End();
	}

	operator bool() const { return opened; }
};

struct ImGuiBeginChild
{
	bool opened = false;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

	ImGuiBeginChild(const char* str_id, ImVec2 size = ImVec2(0, 0), bool border = false)
	{
		opened = ImGui::BeginChild(str_id, size, border, flags);
	}

	~ImGuiBeginChild()
	{
		if (opened)
			ImGui::EndChild();
	}

	operator bool() const { return opened; }
};



//ImGuiWindowFlags_None								�÷��� ����(�⺻��)
//ImGuiWindowFlags_NoTitleBar						â Ÿ��Ʋ��(��� ������)�� ����
//ImGuiWindowFlags_NoResize							����ڰ� â�� ������ ������ �� ���� ��
//ImGuiWindowFlags_NoMove							����ڰ� â�� �巡�׷� �̵��� �� ���� ��
//ImGuiWindowFlags_NoScrollbar						��ũ�ѹٸ� ǥ������ ����(��ũ���� ������)
//ImGuiWindowFlags_NoScrollWithMouse				���콺 �ٷε� ��ũ���� ���ϰ� ��
//ImGuiWindowFlags_NoCollapse						â�� ��� / Ȯ�� ��ư�� ����(����Ŭ������ ���� �� ��Ȱ��ȭ��)
//ImGuiWindowFlags_AlwaysAutoResize					�� �����Ӹ��� ������ ũ�⿡ �°� â ũ�⸦ �ڵ� ������
//ImGuiWindowFlags_NoBackground						â ����� �׸��� ����(������ ����)
//ImGuiWindowFlags_NoSavedSettings.ini				���Ͽ� â ��ġ / ũ�� ���� �������� ����
//ImGuiWindowFlags_NoMouseInputs					â�� ���콺 �Է��� ���� ����(���콺 Ŭ�� / ȣ�� �� ����)
//ImGuiWindowFlags_MenuBar							�޴��� ������ �߰���(BeginMenuBar() ��� ����)
//ImGuiWindowFlags_HorizontalScrollbar				���� ��ũ�ѹٸ� Ȱ��ȭ��
//ImGuiWindowFlags_NoFocusOnAppearing				ó�� ���� �� ��Ŀ���� �������� ����
//ImGuiWindowFlags_NoBringToFrontOnFocus			Ŭ���ص� �ٸ� â ���� �ö���� ����
//ImGuiWindowFlags_AlwaysVerticalScrollbar			���� ��ũ�ѹٸ� �׻� ǥ��(�ʿ� ���)
//ImGuiWindowFlags_AlwaysHorizontalScrollbar		���� ��ũ�ѹٸ� �׻� ǥ��
//ImGuiWindowFlags_NoNavInputs						Ű���� / �����е� ������̼� �Է��� ����
//ImGuiWindowFlags_NoNavFocus						������̼� ��Ŀ���� ���� �� ����
//ImGuiWindowFlags_UnsavedDocument					���� ���� ��(dot)�� ǥ��(��: ���� �� �� ���� ǥ��)


struct MyStyle
{
	int colorCount = 0;
	int varCount = 0;
	int fontCount = 0;

	MyStyle() = default;

	MyStyle& PushColor(ImGuiCol idx, const ImVec4& col)
	{
		ImGui::PushStyleColor(idx, col);
		colorCount++;
		return *this;
	}

	MyStyle& PushVar(ImGuiStyleVar idx, float val)
	{
		ImGui::PushStyleVar(idx, val);
		varCount++;
		return *this;
	}

	MyStyle& PushVar(ImGuiStyleVar idx, const ImVec2& val)
	{
		ImGui::PushStyleVar(idx, val);
		varCount++;
		return *this;
	}

	MyStyle& PushFont(ImFont* font)
	{
		if (font)
		{
			ImGui::PushFont(font);
			fontCount++;
		}
		return *this;
	}

	void SetCur(const float _X, const float _Y)
	{
		ImGui::SetCursorPos(ImVec2{ _X,_Y });
	}

	~MyStyle()
	{
		if (colorCount) ImGui::PopStyleColor(colorCount);
		if (varCount) ImGui::PopStyleVar(varCount);
		if (fontCount) ImGui::PopFont();
	}
};


///////////////////////////////////////////////ImCol//////////////////////////////////////////////
//																								// 	
//		ImGuiCol_Text,                    // �Ϲ� �ؽ�Ʈ ����										//
//		ImGuiCol_TextDisabled,           // ��Ȱ��ȭ�� �ؽ�Ʈ ���� (��: üũ ���� ���� ��)			//
//																								//
//		ImGuiCol_WindowBg,               // �Ϲ� ������ ��� ����									//
//		ImGuiCol_ChildBg,                // �ڽ� ������ ��� ���� (ChildWindow, ChildFrame ��)	//
//		ImGuiCol_PopupBg,                // �˾�, �޴�, ���� ���� ��� ����					    //
//																								//
//		ImGuiCol_Border,                 // �׵θ� ����											//
//		ImGuiCol_BorderShadow,           // �׵θ� �׸��� ���� (���� ���� ������ ����)		    //
//																								//
//		ImGuiCol_FrameBg,                // ������ ��� ���� (üũ�ڽ�, �����̴�, �ؽ�Ʈ �Է� ��)	//
//		ImGuiCol_FrameBgHovered,         // ������ ���� ���콺�� �ö��� �� ����					//
//		ImGuiCol_FrameBgActive,          // �������� Ȱ�� ������ �� ����						    //
//																								//
//		ImGuiCol_TitleBg,                // ������ Ÿ��Ʋ�� �⺻ ���								//
//		ImGuiCol_TitleBgActive,          // Ÿ��Ʋ�ٰ� ��Ŀ���� ������ ��							//
//		ImGuiCol_TitleBgCollapsed,       // Ÿ��Ʋ�ٰ� ���� ���� ��								//
//																								//
//		ImGuiCol_MenuBarBg,              // �޴� �� ��� ����										//
//																								//
//		ImGuiCol_ScrollbarBg,            // ��ũ�ѹ� ���											//
//		ImGuiCol_ScrollbarGrab,          // ��ũ�ѹ� �ڵ� �⺻ ��									//
//		ImGuiCol_ScrollbarGrabHovered,   // ���콺 ���� ������ ��ũ�ѹ� �ڵ�					    //
//		ImGuiCol_ScrollbarGrabActive,    // Ŭ���� ������ ��ũ�ѹ� �ڵ�						    //
//																								//
//		ImGuiCol_CheckMark,              // üũ�ڽ� üũ ǥ�� ����								//
//		ImGuiCol_SliderGrab,             // �����̴� �ڵ� �⺻ ��									//
//		ImGuiCol_SliderGrabActive,       // Ŭ���� �����̴� �ڵ� ��								//
//																								//
//		ImGuiCol_Button,                 // ��ư �⺻ ����										//
//		ImGuiCol_ButtonHovered,          // ��ư ���� ���콺�� �ö��� ��							//
//		ImGuiCol_ButtonActive,           // ��ư Ŭ�� ����										//
//																								//
//		ImGuiCol_Header,                 // ��� ���� �⺻ ���� (CollapsingHeader, TreeNode ��)   //
//		ImGuiCol_HeaderHovered,          // ����� ���콺 ���� ����								//
//		ImGuiCol_HeaderActive,           // ��� Ŭ�� ����										//
//																								//
//		ImGuiCol_Separator,              // ���м� �⺻ ��										//
//		ImGuiCol_SeparatorHovered,       // ���м� ���콺 ����								    //
//		ImGuiCol_SeparatorActive,        // ���м� Ŭ�� ����										//
//																								//
//		ImGuiCol_ResizeGrip,             // �������� �ڵ� �⺻ �� (�ϴ�/���� �𼭸�)				//
//		ImGuiCol_ResizeGripHovered,      // ���콺 ���� ����										//
//		ImGuiCol_ResizeGripActive,       // Ŭ�� ����											//
//																								//
//		ImGuiCol_TabHovered,             // �� ���� ���콺 ���� ��								//
//		ImGuiCol_Tab,                    // �⺻ �� ���� (���õ��� ���� ��)					    //
//		ImGuiCol_TabSelected,            // ���õ� �� ����										//
//		ImGuiCol_TabSelectedOverline,    // ���õ� ���� ��� ������								//
//		ImGuiCol_TabDimmed,              // ��Ŀ���� ���� ������ ��								//
//		ImGuiCol_TabDimmedSelected,      // ��Ŀ���� ���� ���¿��� ���õ� ��					    //
//		ImGuiCol_TabDimmedSelectedOverline, // ���� ���� ���¿��� ������ ����						//
//																								//
//		ImGuiCol_PlotLines,              // �� �׷��� �⺻ �� ��									//
//		ImGuiCol_PlotLinesHovered,       // �� �׷����� ���콺 ����								//
//		ImGuiCol_PlotHistogram,          // ������׷� �� ����								    //
//		ImGuiCol_PlotHistogramHovered,   // ������׷� �� ���콺 ����								//
//																								//
//		ImGuiCol_TableHeaderBg,          // ���̺� ��� ���										//
//		ImGuiCol_TableBorderStrong,      // ���̺� �ܰ��� (�β��� �׵θ�)							//
//		ImGuiCol_TableBorderLight,       // ���̺� ���μ� (���� �׵θ�)							//
//		ImGuiCol_TableRowBg,             // ���̺� ¦�� �� ���								    //
//		ImGuiCol_TableRowBgAlt,          // ���̺� Ȧ�� �� ���								    //
//																								//
//		ImGuiCol_TextLink,               // �����۸�ũ ���� (��: URL ��)							//
//		ImGuiCol_TextSelectedBg,         // �ؽ�Ʈ ���� �� ��� ��								//
//																								//
//		ImGuiCol_DragDropTarget,         // �巡��&��� ��� ���� ��								//
//																								//
//		ImGuiCol_NavHighlight,           // Ű����/�����е� ������̼� ��Ŀ�� �׸� ����				//
//		ImGuiCol_NavWindowingHighlight,  // Ctrl+Tab �� ���õ� ������ ����						//
//		ImGuiCol_NavWindowingDimBg,      // Ctrl+Tab â ����Ʈ �ܰ� ��Ӱ� ó��					//
//																								//
//		ImGuiCol_ModalWindowDimBg,       // ��� ������ ǥ�� �� ��� ��Ӱ� ó��					//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////ImVar///////////////////////////////////////////////////////
//																												  //
//		ImGuiStyleVar_Alpha,                    // float     ��ü UI ���� (0.0~1.0)							  //	
//		ImGuiStyleVar_DisabledAlpha,            // float     ��Ȱ��ȭ�� ����� ���� (0.0~1.0)					  //
//		ImGuiStyleVar_WindowPadding,            // ImVec2    â ���� ���� (�������� â �׵θ� ������ ����)			  //
//		ImGuiStyleVar_WindowRounding,           // float     â �𼭸� �ձ۱� ������								  //
//		ImGuiStyleVar_WindowBorderSize,         // float     â �׵θ� �β�										  //
//		ImGuiStyleVar_WindowMinSize,            // ImVec2    â �ּ� ũ��										  //
//		ImGuiStyleVar_WindowTitleAlign,         // ImVec2    ���� ���� ��ġ (0.0: ����, 0.5: �߾�, 1.0: ������)	  //
//		ImGuiStyleVar_ChildRounding,            // float     Child ������ �𼭸� �ձ۱�							  //
//		ImGuiStyleVar_ChildBorderSize,          // float     Child ������ �׵θ� �β�								  //
//		ImGuiStyleVar_PopupRounding,            // float     �˾� â�� �𼭸� �ձ۱�								  //
//		ImGuiStyleVar_PopupBorderSize,          // float     �˾� â�� �׵θ� �β�								  //
//		ImGuiStyleVar_FramePadding,             // ImVec2    ������ ���� �е� (��ư, �Է�â ���� ����)				  //
//		ImGuiStyleVar_FrameRounding,            // float     ������ �𼭸� �ձ۱� (��ư ��)						  //
//		ImGuiStyleVar_FrameBorderSize,          // float     ������ �׵θ� �β�									  //
//		ImGuiStyleVar_ItemSpacing,              // ImVec2    ��� �� ���� (��ư �� �Ÿ� ��)						  //
//		ImGuiStyleVar_ItemInnerSpacing,         // ImVec2    ���� ��� �� ���� (�ؽ�Ʈ�� üũ�ڽ� ��)				  //
//		ImGuiStyleVar_IndentSpacing,            // float     �鿩���� ���� (TreeNode, Indent ��)					  //
//		ImGuiStyleVar_CellPadding,              // ImVec2    ���̺� �� ���� ����									  //
//		ImGuiStyleVar_ScrollbarSize,            // float     ��ũ�ѹ� �β�										  //
//		ImGuiStyleVar_ScrollbarRounding,        // float     ��ũ�ѹ� �𼭸� �ձ۱�								  //
//		ImGuiStyleVar_GrabMinSize,              // float     �����̴�, ��ũ�ѹ� ��� �巡�� ������ �ּ� ũ��		  //
//		ImGuiStyleVar_GrabRounding,             // float     �巡�� �ڵ��� �ձ۱�									  //
//		ImGuiStyleVar_TabRounding,              // float     �� �𼭸� �ձ۱�										  //
//		ImGuiStyleVar_TabBorderSize,            // float     �� �׵θ� �β�										  //
//		ImGuiStyleVar_TabBarBorderSize,         // float     �ǹ� ��ü�� �׵θ� �β�								  //
//		ImGuiStyleVar_TabBarOverlineSize,       // float     ���õ� ���� ���� �β� (������)						  //
//		ImGuiStyleVar_TableAngledHeadersAngle,  // float     ���� ���̺� ����� ���� ����						  //
//		ImGuiStyleVar_TableAngledHeadersTextAlign, // ImVec2 �ؽ�Ʈ ���� (���� ��� ���ο����� ��ġ)				  //
//		ImGuiStyleVar_ButtonTextAlign,          // ImVec2    ��ư �ȿ����� �ؽ�Ʈ ���� ��ġ						  //
//		ImGuiStyleVar_SelectableTextAlign,      // ImVec2    Selectable ��� ���� �ؽ�Ʈ ���� ��ġ				  //
//		ImGuiStyleVar_SeparatorTextBorderSize,  // float     ���м� �ؽ�Ʈ�� �ܰ��� ũ��							  //
//		ImGuiStyleVar_SeparatorTextAlign,       // ImVec2    ���м� �ؽ�Ʈ�� ���� (�¿�)							  //
//		ImGuiStyleVar_SeparatorTextPadding,     // ImVec2    ���м� �ؽ�Ʈ�� �е�									  //
//																												  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



struct TextLay
{
	ImVec2 pos;
	float spacing;


	TextLay(ImVec2 start) : pos(start)
	{
		spacing = ImGui::GetTextLineHeightWithSpacing();
	}

	void RowF(const char* label, ImVec4 color, float value = 0.0f)
	{
		ImGui::SetCursorPos(pos);
		ImGui::TextColored(color, "%s : %.2f", label, value);
		pos.y += spacing;
	}
	void Row(const char* label, ImVec4 color, int value = -1)
	{
		ImGui::SetCursorPos(pos);
		ImGui::TextColored(color, "%s : %d", label, value);
		pos.y += spacing;
	}
	void Row(const char* label, ImVec4 color, std::string value)
	{
		ImGui::SetCursorPos(pos);
		ImGui::TextColored(color, "%s : %s", label, value.c_str());
		pos.y += spacing;
	}
};




//Contents

#define MSG_GET_INFO 0x20
#define MSG_INFO 0x21
#define MSG_SET_CONFIG 0x30
#define MSG_CONFIG 0x31
#define MSG_GET_DATA 0x40
#define MSG_DATA 0x41