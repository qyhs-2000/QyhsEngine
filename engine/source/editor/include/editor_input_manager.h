#pragma once
//
namespace QYHS
{
	class EditorInputManager
	{
	public:
		//EditorInputManager();
		void onKeyInEditorMode(int key, int scancode, int action, int mods);
		void registerInput();
		void initialize();
	public:
		void onScroll(double xoffset, double yoffset);
		void onKey(int key, int scancode, int action, int mods);
		void onCursorPos(float x_pos, float y_pos);
	private:

	};
}