#include <GL/gl3w.h>
#include <GLFW/glfw3.h> // GLFW helper library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp> // for glm::ortho

// IMGUI
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <iostream> // Used for 'cout'
#include <stdio.h>  // Used for 'printf'
#include <SOIL.h>

#include <chrono>

#include "Shaders.h"
#include "ai.h"
//#include "tictactoe.h"

/*---------------------------- Variables ----------------------------*/
// GLFW window
GLFWwindow* window;
int width = 1280;
int halfwidth = 1280 / 2;
int height = 720;
int halfheight = 720 / 2;

// Uniform locations
GLuint mvp_loc, col_loc;

// OpenGl stuff
GLuint shader_program;
GLuint quad_vbo; // vertex buffer object
GLuint quad_vao; // vertex array object

// Matrices
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

// Textures
GLuint boardtex;
GLuint xtex;
GLuint otex;

// Functions
void DrawQuad(glm::vec2, glm::vec2, glm::vec3 = glm::vec3(1.0f));

Board ttt;
AI ai;
char playerChar = 'X';
char aiChar = 'O';
char winner;

bool playerIsAi = false;

//timer
std::chrono::high_resolution_clock::time_point start;
void TimerReset() { start = std::chrono::high_resolution_clock::now(); }
int TimerElapsed() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count(); }
std::vector<float> timerHistory;
int timerMax = 0;
//


void RestartGame()
{
	ttt = Board();
	winner = ' ';
}

void Initialize()
{
	srand(time(NULL));
    // Create a shader for the lab
    GLuint vs = buildShader(GL_VERTEX_SHADER, ASSETS"primitive.vs");
    GLuint fs = buildShader(GL_FRAGMENT_SHADER, ASSETS"primitive.fs");
    shader_program = buildProgram(vs, fs, 0);
    dumpProgram(shader_program, "TicTacToe");

	boardtex = SOIL_load_OGL_texture(
		ASSETS"Images/board.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	xtex = SOIL_load_OGL_texture(
		ASSETS"Images/x.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	otex = SOIL_load_OGL_texture(
		ASSETS"Images/o.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

    // Create all 4 vertices of the quad
    glm::vec3 p0 = glm::vec3(-1.0f, -1.0f, 0.0f);
    glm::vec3 p1 = glm::vec3(-1.0f, 1.0f, 0.0f);
    glm::vec3 p2 = glm::vec3(1.0f, -1.0f, 0.0f);
    glm::vec3 p3 = glm::vec3(1.0f, 1.0f, 0.0f);

    // Create a list of vertices
    glm::vec3 vertices[12] =
    {
        // Bottom face
        p0, p1, p2, p3,
    };

    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glUseProgram(shader_program);
    GLuint vPosition = glGetAttribLocation(shader_program, "vPosition");
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    mvp_loc = glGetUniformLocation(shader_program, "modelViewProjMat");
	
	RestartGame();
}

int CheckClickedCell(float cx, float cy)
{
	if (cy > 75 && cy < 225)
	{
		if (cx > 75 && cx < 225)
			return 8;
		else if (cx <= 75 && cx >= -75)
			return 7;
		else if (cx < -75 && cx > -225)
			return 6;
	}
	else if (cy <= 75 && cy >= -75)
	{
		if (cx > 75 && cx < 225)
			return 5;
		else if (cx <= 75 && cx >= -75)
			return 4;
		else if (cx < -75 && cx > -225)
			return 3;
	}
	else if (cy < -75 && cy > -225)
	{
		if (cx > 75 && cx < 225)
			return 2;
		else if (cx <= 75 && cx >= -75)
			return 1;
		else if (cx < -75 && cx > -225)
			return 0;
	}
	return 9;
}

void Update(float a_deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);

	if (ttt.CheckWinner() != 'N')
	{
		winner = ttt.victor;
	}
	else
	{
		if (ttt.currentTurn == playerChar)
		{
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
			{
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				float cx = x - halfwidth;
				float cy = y - halfheight;
				//150 width per cell
				int cell = CheckClickedCell(cx, cy);
				if (cell != 9) ttt.MakeMove(playerChar, cell);
			}
			if (playerIsAi)
			{
				TimerReset();
				int play = ai.BestMove(ttt);
				timerHistory.push_back(TimerElapsed());
				timerMax = timerHistory.back() > timerMax ? timerHistory.back() : timerMax;
				ttt.MakeMove(playerChar, play);
			}
		}
		else if (ttt.currentTurn == aiChar)
		{
			TimerReset();
			int play = ai.BestMove(ttt);
			timerHistory.push_back(TimerElapsed());
			timerMax = timerHistory.back() > timerMax ? timerHistory.back() : timerMax;
			ttt.MakeMove(aiChar, play);
		}
	}
}

void Render()
{
    glUseProgram(shader_program);
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-640.0f, 640.0f, -360.0f, 360.0f, -1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, boardtex);

	DrawQuad(glm::vec2(75.f, 0.f), glm::vec2(10.0f, 450.0f));
	DrawQuad(glm::vec2(-75.f, 0.f), glm::vec2(10.0f, 450.0f));
	DrawQuad(glm::vec2(0.f, 75.f), glm::vec2(450.0f, 10.0f));
	DrawQuad(glm::vec2(0.f, -75.f), glm::vec2(450.0f, 10.0f));

	for (int i = 0; i < 9; i++)
	{
		if (ttt.board[i] == ' ') continue;
		glBindTexture(GL_TEXTURE_2D, ttt.board[i] == 'X' ? xtex : otex);
		DrawQuad(glm::vec2(-150 + 150 * (i%3), 150 - 150 * (i/3)), glm::vec2(150.f, 150.f));
	}

	if (winner == 'X')
	{
		glBindTexture(GL_TEXTURE_2D, xtex);
		DrawQuad(glm::vec2(350, 0), glm::vec2(150.f, 150.f));
	}
	else if (winner == 'O')
	{
		glBindTexture(GL_TEXTURE_2D, otex);
		DrawQuad(glm::vec2(350, 0), glm::vec2(150.f, 150.f));
	}

    glUseProgram(GL_NONE);
}

void GUI()
{
    ImGui::Begin("Settings", 0, ImVec2(100, 50), 0.4f, ImGuiWindowFlags_AlwaysAutoResize);
    {
        // Show some basic stats in the settings window 
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Checkbox("Ai vs Ai", &playerIsAi);
		if (ImGui::Button("Restart Game")) RestartGame();
		if (ImGui::Button("Swap Turn Order & Reset"))
		{
			playerChar = playerChar == 'X' ? 'O' : 'X';
			aiChar = aiChar == 'X' ? 'O' : 'X';
			RestartGame();
		}

		ImGui::Text("Decision Time (microseconds)");
		ImGui::PlotHistogram("", timerHistory.data(), timerHistory.size(), 0, "", 0, timerMax, ImVec2(250, 100), sizeof(int));
		ImGui::Text("Slowest Move: %i", timerMax);
		
		ImGui::Text("\nOptimization Toggles");
		ImGui::Checkbox("Mirror Dropout", &ai.useMirrorDropout);
		ImGui::Checkbox("AlphaBeta Dropout", &ai.useAlphaBeta);
    }
    ImGui::End();
}

void Cleanup()
{
    glDeleteBuffers(1, &quad_vbo);
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteProgram(shader_program);
}

void DrawQuad(glm::vec2 a_position, glm::vec2 a_size, glm::vec3 a_color)
{
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(a_position.x, a_position.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(a_size.x * 0.5f, a_size.y * 0.5f, 1.0f));

    glm::mat4 modelViewProjMat = projectionMatrix * viewMatrix * modelMatrix;

    glUniformMatrix4fv(mvp_loc, 1, 0, &modelViewProjMat[0][0]);
    glUniform3fv(col_loc, 1, &a_color[0]);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void ResizeEvent(GLFWwindow* a_window, int a_width, int a_height)
{
    // Set the viewport incase the window size changed
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

int main()
{
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    window = glfwCreateWindow(width, height, "James Conrad 100559437 - Tic Tac Toe", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, ResizeEvent);
    glfwSwapInterval(0);

    // start GL3W
    gl3wInit();

    // Setup ImGui binding. This is for any parameters you want to control in runtime
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsLight();

    // Get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    Initialize();

    float oldTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - oldTime;
        oldTime = currentTime;

        // update other events like input handling 
        glfwPollEvents();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplGlfwGL3_NewFrame();

        // Call the helper functions
        Update(deltaTime);
        Render();
        GUI();

        // Finish by drawing the GUI on top of everything
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    ImGui_ImplGlfwGL3_Shutdown();
    Cleanup();
    return 0;
}