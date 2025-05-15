#pragma once
#include <vector>
#include <functional>
#include <string>
#include "sprite.h"
#include "core/math/math.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/render/rhi/rhi.h"
#include "function/ui/sprite_font.h"
namespace qyhs::gui
{
	enum WIDGET_STATE
	{
		IDLE,			// widget is doing nothing
		FOCUS,			// widget got pointer dragged on or selected
		ACTIVE,			// widget is interacted with right now
		DEACTIVATING,	// widget has last been active but no more interactions are occuring
		WIDGET_STATE_COUNT,
	};


	class Widget :public TransformComponent
	{
	public:
		virtual void setName(const std::string name);
		virtual void render(const Canvas& rhi, CommandList cmd) {};
		virtual void update(const Canvas & canvas,float delta_time);
		virtual void setText(const char* text);
		Sprite sprites[WIDGET_STATE_COUNT];
		WIDGET_STATE state = IDLE;
		void setSize(const Vector2& size);
		void setPosition(const Vector2& position);
	protected:
		std::string name;
		float shadow = 1;
		SpriteFont font;
	};

	class Gui
	{
	public:
		void addWeights(Widget* widget);
		void render(const Canvas& canvas, CommandList cmd) const;
		void update(const Canvas& canvas, float delta_time);
	private:
		std::vector<Widget*> widgets;
	};


	class ComboBox :public Widget
	{
	public:
		void create(const std::string name);
		void addItem(const std::string name, const int value);
		void setSelected(int index);
		virtual void update(const Canvas& canvas, float delta_time) override;
		virtual void render(const Canvas& rhi, CommandList cmd) override;
		struct Item
		{
			std::string name;
			int value;
		};
		std::vector<Item> items;
		void onSelect(std::function<void()> function);
		Color shadow_color = Color::shadow();
		int selected = -1;
	protected:
		std::function<void()> on_select;
		bool drop_arrow = true;
		SpriteFont selected_font;
	};
}