/*
 * show rendered images 
 *
 */
#include "the_renderer.h"
//-----------------------------------------------------------------------------
// DEMO CODE
//-----------------------------------------------------------------------------

#if !defined(IMGUI_DISABLE_OBSOLETE_FUNCTIONS) && defined(IMGUI_DISABLE_TEST_WINDOWS) && !defined(IMGUI_DISABLE_RENDERER_WINDOWS)   // Obsolete name since 1.53, TEST->DEMO
#define IMGUI_DISABLE_RENDERER_WINDOWS
#endif

#if !defined(IMGUI_DISABLE_RENDERER_WINDOWS)


void ImGui::ShowRendererWindow(bool* p_open)
{

	static bool show_app_main_menu_bar = true;

	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = false;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;


	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_close) p_open = NULL;
	
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("The Renderer", p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
	ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

	ImGui::Text("the renderer. ImGui version is (%s)", IMGUI_VERSION);

	// Menu
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			ShowMenuFile();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (g_ShowLogger) ShowLogger(&g_ShowLogger);
	if (g_ShowImage) ShowImage(g_IsLoadImage);
	ImGui::End();
}

static void ShowMenuFile() 
{
	if (ImGui::MenuItem("show log"))		g_ShowLogger = true;
	if (ImGui::MenuItem("load image"))		LoadingImageRGB();
	if (ImGui::MenuItem("show image"))		g_ShowImage ^= 1;
}

// Demonstrate creating a simple log window with basic filtering.
static void ShowLogger(bool* p_open)
{
	g_Logger.Draw("Render Logger", p_open);
}

static void LoadingImageRGB()
{
	// Build texture atlas
	//ImDrawList drawList;
	int width, height, nrChannels;
	unsigned char *pixels = stbi_load("../data/images/example.jpg", &width, &height, &nrChannels, 0);
	//unsigned char *pixels = stbi_load("../data/images/dog.jpg", &width, &height, &nrChannels, 0);
	if (pixels == NULL)
	{
		Logger("pixels", "image loading error!");
		return;
	}
	g_Logger.AddLog("[Loading Image] image width is : %d, image height is : %d, Channels is : %d, length is : %d\n", width, height, nrChannels, strlen((char *)pixels));

	g_IsLoadImage = true;

	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	//io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
	g_Logger.AddLog("texture id %d\n", g_FontTexture);

	ImFontAtlas * texImAtlas = new ImFontAtlas;
	texImAtlas->TexID = (void *)(intptr_t)g_FontTexture;
	texImAtlas->TexHeight = height;
	texImAtlas->TexWidth = width;
	
	g_ImageID.push_back(texImAtlas);

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	g_Logger.AddLog("last_texture id %d\n", last_texture);
}

static void ShowImage(bool isLoaded)
{
	if (!isLoaded) return;

	ImFontAtlas *tex = g_ImageID[0];
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 maxPos = ImVec2(pos.x + ImGui::GetWindowSize().x, pos.y + ImGui::GetWindowSize().y);

	ImGui::Image(tex->TexID, ImVec2((float)tex->TexWidth, (float)tex->TexHeight));
}

static void Logger(const char * logPattern, const char * content)
{
	g_Logger.AddLog("[%s]  %s\n", logPattern, content);
}

static void Logger(const char * logPattern, int content)
{
	g_Logger.AddLog("[%s]  %d\n", logPattern, content);
}
#else

#endif