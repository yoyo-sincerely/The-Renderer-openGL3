/*
 * show rendered images 
 *
 */
//#include "Tra"
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
	if (g_ShowImage) ShowImage();
	ImGui::End();
}

static void ShowMenuFile() 
{
	if (ImGui::MenuItem("show log"))		g_ShowLogger = true;
	if (ImGui::MenuItem("show/close image"))	
	{
		g_ShowImage ^= 1;
	}
	if (ImGui::MenuItem("render test"))		RenderTest();
	if (ImGui::MenuItem("render"))		Render();
}

static void RenderTest()
{
	ImFontAtlas * testBuffer = new ImFontAtlas;
	testBuffer->TexWidth = 100;
	testBuffer->TexHeight = 100;

	testBuffer->TexPixelsRGBA32 = (unsigned int *)malloc(sizeof(testBuffer->TexPixelsRGBA32) * testBuffer->TexWidth * testBuffer->TexHeight);
	for (int i = 0; i < testBuffer->TexHeight; i++)
	{
		auto prt = (unsigned int *)((char *)testBuffer->TexPixelsRGBA32 + i * sizeof(testBuffer->TexPixelsRGBA32));
		for (int j = 0; j < testBuffer->TexWidth; j++)
		{
			double r = 0.5;
			double g = 0.5;
			double b = 0.5;
			if (*prt == NULL)
			{
				g_Logger.AddLog("prt is null!");
				return;
			}
			*prt = (((unsigned int)255 * 256 + (unsigned int)(r * 255.0f)) * 256 + (unsigned int)(g * 255.0f)) * 256 + (unsigned int)(b * 255.0f);
		}
	}

	LoadingImageRGBA(testBuffer);
}
static void RenderBITMAP()
{

}

//test render
static void Render()
{
	//init
	SetDefaultScene(&scene);
	tracer = new RayTracer(&scene);
	ImFontAtlas * buffer = new ImFontAtlas;
	buffer->TexWidth = 800;
	buffer->TexHeight = 600;
	buffer->TexPixelsRGBA32 = (unsigned int *)malloc(sizeof(buffer->TexPixelsRGBA32) * buffer->TexWidth * buffer->TexHeight);
	g_Logger.AddLog("TexPixelsAlpha8 is %d \n", sizeof(buffer->TexPixelsRGBA32));

	tracer->Render(buffer->TexPixelsRGBA32, buffer->TexWidth, buffer->TexHeight, sizeof(buffer->TexPixelsRGBA32), 5);

	LoadingImageRGBA(buffer);
}

// Demonstrate creating a simple log window with basic filtering.
static void ShowLogger(bool* p_open)
{
	g_Logger.Draw("Render Logger", p_open);
}

static void LoadingImage(const char * imagePath)
{
	//ImDrawList drawList;
	int width, height, nrChannels;
	ImFontAtlas * buffer = new ImFontAtlas;
	buffer->TexPixelsAlpha8 = stbi_load(imagePath, &width, &height, &nrChannels, 0);
	buffer->TexWidth = width;
	buffer->TexHeight = height;
	g_Logger.AddLog("[Loading Image] image width is : %d, image height is : %d, length is : %d\n", width, height, strlen((char *)buffer->TexPixelsAlpha8));
	LoadingImageRGB(buffer);
}

static void LoadingImageRGBA(ImFontAtlas * texImAtlas)
{
	if (texImAtlas->TexPixelsRGBA32 == NULL)
	{
		return;
	}

	g_IsLoadImage = true;

	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImAtlas->TexWidth, texImAtlas->TexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImAtlas->TexPixelsRGBA32);

	// Store our identifier
	//io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
	g_Logger.AddLog("texture id %d\n", g_FontTexture);

	texImAtlas->TexID = (void *)(intptr_t)g_FontTexture;

	g_Image.push_back(texImAtlas);

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
}

static void LoadingImageRGB(ImFontAtlas * texImAtlas)
{
	// Build texture atlas
	//ImDrawList drawList;
	//unsigned char *pixels = stbi_load("../data/images/dog.jpg", &width, &height, &nrChannels, 0);
	if (texImAtlas->TexPixelsAlpha8 == NULL)
	{
		return;
	}

	g_IsLoadImage = true;

	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImAtlas->TexWidth, texImAtlas->TexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texImAtlas->TexPixelsAlpha8);

	// Store our identifier
	//io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
	g_Logger.AddLog("texture id %d\n", g_FontTexture);

	texImAtlas->TexID = (void *)(intptr_t)g_FontTexture;
	
	g_Image.push_back(texImAtlas);

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
}

static void ShowImage() 
{
	if (g_Image.empty()) return;

	if (g_FontTexture == NULL) return;

	ImFontAtlas *tex = g_Image.back();

	if(tex != NULL)
		ImGui::Image(tex->TexID, ImVec2((float)tex->TexWidth, (float)tex->TexHeight));
}
#else

#endif