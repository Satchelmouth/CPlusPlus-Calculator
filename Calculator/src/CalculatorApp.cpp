#pragma once
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Common.h"

/// <summary>
/// IMGUI initialization, callback linking of UI and calculation data (IO stream object)
/// as well as keyboard inputs
/// </summary>

// List of  Operations our calculator can perform
enum Operation { Add, Subtract, Divide, Multiply, Equals, Decimal, Clear, DelLast };

class CalculatorUI : public Walnut::Layer
{
private:
	//Button size is uniform currently
	const ImVec2 buttonSize = ImVec2(100, 100);

	// Current calculation stream (white)
	const char* val = "NAN";
	//Previous calculation streams (grey) This is an std:: string as we need to add it as single line entries for scrolling purposes
	std::vector<std::string> pastVal;
	// Flags for our IMGUI window behaviour
	const ImGuiWindowFlags wFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;


public:
	// Callback std::functions for buttons and keyboard input
	std::function<void(float)> onNumPressed;
	std::function<void(Operation)> onOperationPressed;

	// Set the value of our calculations on the UI
	// Updated val from a reference passed in externally
	void SetCalculatorValueString(std::tuple<const char*, std::vector<std::string>> t)
	{	
		val = std::get<0>(t);
		pastVal.clear();
		pastVal = std::get<1>(t);
	}

	// Called every tick
	virtual void OnUIRender() override {

		// Initialize UI 
		ImGui::Begin("Calculator", nullptr, wFlags);
		ImGui::SetWindowFontScale(1.5);
		ImGui::SetNextItemWidth(buttonSize.x * 4);

		//-------------------------------------------------------------------------------- Text Formatting
		static ImVec2 size(425.0f, 120.0f);

		int n = 0;
		// Only display past values if we have them
		if (pastVal.size() == 0) {
			n = 1; ImGui::InvisibleButton("##padding", size);
		}

		for (n; n < 2; n++)
		{
			const ImVec2 p0 = ImGui::GetItemRectMin();
			const ImVec2 p1 = ImGui::GetItemRectMax();
			const ImVec2 text_pos = ImVec2(p0.x, p0.y);
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			switch (n)
			{
				// Scrollable list of previous operations
				case 0:
				{
					const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)1);
					const bool child_is_visible = ImGui::BeginChild(child_id, size, true);
					{
						for (int i = 0; i < pastVal.size(); i++)
							ImGui::Text(pastVal[i].c_str());

						ImGui::SetScrollHereY(1.0f); // 0.0f:top, 0.5f:center, 1.0f:bottom
	
					}
					ImGui::EndChild();
					ImGui::InvisibleButton("##padding", size);
					break;
				}
				// Current operations
				case 1:
				{
					draw_list->PushClipRect(p0, p1, true);
					draw_list->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 255));
					draw_list->AddText(text_pos, IM_COL32_WHITE, val);
					draw_list->PopClipRect();
					break;
				}
			}
		}

		//-------------------------------------------------------------------------------- 

		// INMGUI Buttons and their callback values
		if (ImGui::Button("7", buttonSize)) { onNumPressed(7.0f); }							ImGui::SameLine();
		if (ImGui::Button("8", buttonSize)) { onNumPressed(8.0f); }							ImGui::SameLine();
		if (ImGui::Button("9", buttonSize)) { onNumPressed(9.0f); }							ImGui::SameLine();
		if (ImGui::Button("*", buttonSize)) { onOperationPressed(Operation::Multiply); }

		if (ImGui::Button("4", buttonSize)) { onNumPressed(4.0f); }							ImGui::SameLine();
		if (ImGui::Button("5", buttonSize)) { onNumPressed(5.0f); }							ImGui::SameLine();
		if (ImGui::Button("6", buttonSize)) { onNumPressed(6.0f); }							ImGui::SameLine();
		if (ImGui::Button("-", buttonSize)) { onOperationPressed(Operation::Subtract); }

		if (ImGui::Button("1", buttonSize)) { onNumPressed(1.0f); }							ImGui::SameLine();
		if (ImGui::Button("2", buttonSize)) { onNumPressed(2.0f); }							ImGui::SameLine();
		if (ImGui::Button("3", buttonSize)) { onNumPressed(3.0f); }							ImGui::SameLine();
		if (ImGui::Button("+", buttonSize)) { onOperationPressed(Operation::Add); }

		if (ImGui::Button(".", buttonSize)) { onOperationPressed(Operation::Decimal); }		ImGui::SameLine();
		if (ImGui::Button("0", buttonSize)) { onNumPressed(0.0f); }							ImGui::SameLine();
		if (ImGui::Button("=", buttonSize)) { onOperationPressed(Operation::Equals); }		ImGui::SameLine();
		if (ImGui::Button("/", buttonSize)) { onOperationPressed(Operation::Divide); }

		if (ImGui::Button("C", buttonSize))		{ onOperationPressed(Operation::Clear); }	ImGui::SameLine();
		if (ImGui::Button("DEL", buttonSize))	{ onOperationPressed(Operation::DelLast); }
		
		ImGui::End();	
		
		//Keyboard inputs and their callback values
		// Imguikey enum https://github.com/ocornut/imgui/blob/a8df192df022ed6ac447e7b7ada718c4c4824b41/imgui.h#L1353
		//Get pointer to current IMGUI Context so we can access io further down to check if shift is depressed
		ImGuiContext& g = *GImGui;
		// Shift keys for people without numpads (like me :-p)
		if (g.IO.KeyShift == true)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_8)) { onOperationPressed(Operation::Multiply); }
			if (ImGui::IsKeyPressed(ImGuiKey_Equal)) { onOperationPressed(Operation::Add); }
		}
		//All other inputs
		else
		{
			if (ImGui::IsKeyPressed(ImGuiKey_1)) { onNumPressed(1.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_2)) { onNumPressed(2.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_3)) { onNumPressed(3.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_4)) { onNumPressed(4.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_5)) { onNumPressed(5.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_6)) { onNumPressed(6.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_7)) { onNumPressed(7.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_8)) { onNumPressed(8.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_9)) { onNumPressed(9.0f); }
			if (ImGui::IsKeyPressed(ImGuiKey_0)) { onNumPressed(0.0f); }

			if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract) || ImGui::IsKeyPressed(ImGuiKey_Minus)) { onOperationPressed(Operation::Subtract); }
			if (ImGui::IsKeyPressed(ImGuiKey_KeypadDecimal) || ImGui::IsKeyPressed(ImGuiKey_Period)) { onOperationPressed(Operation::Decimal); }
			if (ImGui::IsKeyPressed(ImGuiKey_KeypadDivide) || ImGui::IsKeyPressed(ImGuiKey_Slash)) { onOperationPressed(Operation::Divide); }
			if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd)) { onOperationPressed(Operation::Add); }
			if (ImGui::IsKeyPressed(ImGuiKey_C)) { onOperationPressed(Operation::Clear); }
			if (ImGui::IsKeyPressed(ImGuiKey_KeypadMultiply)) { onOperationPressed(Operation::Multiply); }
			if (ImGui::IsKeyPressed(ImGuiKey_Equal)) { onOperationPressed(Operation::Equals); }
			if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace)) { onOperationPressed(Operation::DelLast); }
		}
	}
};

// UI Object Function Pointer
std::shared_ptr<CalculatorUI> calcUI;

// Pointer to our Calculator IO Stream Class
CalcIOStreamObj *calcStream = new CalcIOStreamObj();

//Request IO Stream tries to add an operation
void SetOperation(Operation o)
{
	// enum Operation { Add, Subtract, Divide, Multiply, Equals, Decimal};
	switch (o)
	{
		case Operation::Add:
			calcStream->AddOperation(OpAdd, '+');
			break;

		case Operation::Subtract:
			calcStream->AddOperation(OpSubtract, '-');
			break;

		case Operation::Divide:
			calcStream->AddOperation(OpDivide, '/');
			break;

		case Operation::Multiply:
			calcStream->AddOperation(OpMultiply, '*');
			break;

		case Operation::Equals:
			calcStream->Equals();
			break;

		case Operation::Decimal:
			calcStream->SetDecimalMode();
			break;

		case Operation::Clear:
			calcStream->ClearOperations();
			break;
		
		case Operation::DelLast:
			calcStream->DelLast();
			break;
	}
}

//Request IO Stream tries to add a number
void SetNum(float inF) { calcStream->AddNum(inF); }

// Update the value of the calculator based on a callback from the IO stream (linked in CreateApplication)
// Is there a better way to update the calculator UI than via a function here? Ideally I'd like to just have the callback directly call the function in &calcUI
void UpdateCalcUI(std::tuple<const char*, std::vector<std::string>> t)
{
	calcUI->SetCalculatorValueString(t);
}

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	// Get pointers for CalcFunc functions as std::function types
	std::function<void(float, float&)> add = OpAdd;
	std::function<void(float, float&)> subtract = OpSubtract;
	std::function<void(float, float&)> divide = OpDivide;
	std::function<void(float, float&)> multiply = OpMultiply;

	//Walnut app instantiation
	Walnut::ApplicationSpecification spec;
	spec.Name = "My Awesome Calculator";
	spec.Width = 500.0f;
	spec.Height = 900.0f;
	Walnut::Application* app = new Walnut::Application(spec);

	//CalculatorUI* calcUIObj = new CalculatorUI;
	calcUI = std::make_shared<CalculatorUI>();

	app->PushLayer(calcUI);


	//Link UI Callbacks to IO Stream
	// Numbers
	calcUI->onNumPressed = &SetNum;

	// Functions
	calcUI->onOperationPressed = &SetOperation;

	//Link callback from IO stream to UI
	// Is there a better way to update the calculator UI than via a function here? Ideally I'd like to just have the callback directly call the function in &calcUI
	calcStream->onValUpdated = &UpdateCalcUI;

	//Set the default value of our to zero
	SetNum(0.0f);

	//std::cin.get();
	return app;
}

