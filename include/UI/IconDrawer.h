#pragma once

class Texture;
class GLTexture2D;
class IconDrawer
{
	inline constexpr static float ICON_SIZE = 40;

	inline static Texture* _lightIconTex = nullptr;

	static void init();

	static void draw();

public:
	friend class WindowDrawer;
	friend class ImGuiHandler;
};
