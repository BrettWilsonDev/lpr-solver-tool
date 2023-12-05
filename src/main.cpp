#include "raylib.h"
// #include "rlgl.h"


#include "imgui.h"
#include "rlImGui.h"

#include "PrimalSolver.hpp"

#include <iostream>
#include <sstream>

// Define a custom stream buffer to capture std::cout output
// Define a custom stream buffer to capture std::cout output
class ImGuiStreamBuffer : public std::streambuf
{
public:
	virtual int_type overflow(int_type c = traits_type::eof())
	{
		if (c != traits_type::eof())
		{
			buffer += static_cast<char>(c);
			if (c == '\n')
			{
				// Update ImGui window with captured content when a newline is encountered
				consoleBuffer += buffer + '\n';
				buffer.clear();
			}
		}
		return c;
	}

	const std::string &getConsoleBuffer() const
	{
		return consoleBuffer;
	}

	void clearConsoleBuffer()
	{
		consoleBuffer.clear();
	}

private:
	std::string buffer;
	std::string consoleBuffer;
};

int main(int argc, char *argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Liner Programming Solver Tool");
	// SetTargetFPS(144);
	rlImGuiSetup(true);

	// Create an instance of the custom stream buffer
	ImGuiStreamBuffer imguiStreamBuffer;

	// Redirect std::cout to the custom stream buffer
	std::streambuf *coutBuffer = std::cout.rdbuf(&imguiStreamBuffer);

	PrimalSolver ps;

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Clear the console buffer at the beginning of each frame
		// imguiStreamBuffer.clearConsoleBuffer();

		BeginDrawing();
		ClearBackground(DARKGRAY);

		// start ImGui Conent
		rlImGuiBegin();

		// ImGui Content fill the whole screen
		ImGui::SetNextWindowSize({static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, ImGuiCond_Always);
		ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);

		// ImGui Content stop resizing and remove title bar
		ImGui::Begin("My ImGui Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

		// Add a region to display the console output
		ImGui::Text("Console Output:");

		ImGui::SetWindowFontScale(1.5f);
		ImGui::Separator();
		ImGui::TextUnformatted(imguiStreamBuffer.getConsoleBuffer().c_str());

		ImGui::End();

		// end ImGui Content
		rlImGuiEnd();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	rlImGuiShutdown();

	std::cout.rdbuf(coutBuffer);

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
