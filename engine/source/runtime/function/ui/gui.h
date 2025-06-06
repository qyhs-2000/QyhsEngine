#pragma once
#include <vector>
#include <functional>
#include <string>
#include "sprite.h"
#include "core/math/math.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/render/rhi/rhi.h"
#include "function/ui/sprite_font.h"
#include "function/render/primitive.h"
namespace qyhs::gui
{
	enum WIDGET_STATE
	{
		IDLE,			// widget is doing nothing
		FOCUS,			// widget got pointer dragged on or selected
		ACTIVE,			// widget is interacted with right now
		DEACTIVATING,	// widget has last been activate but no more interactions are occuring
		WIDGET_STATE_COUNT,
	};

	struct EventArgs
	{
		uint64_t userdata;
	};

	class Widget :public TransformComponent
	{
	public:
		virtual void setName(const std::string name);
		virtual void render(const Canvas& rhi, CommandList cmd) {};
		virtual void update(const Canvas& canvas, float delta_time);
		virtual void setText(const char* text);
		virtual void setColor(const Color& color, int id = -1);
		Sprite sprites[WIDGET_STATE_COUNT];
		WIDGET_STATE state = IDLE;
		void setSize(const Vector2& size);
		void setPosition(const Vector2& position);
		primitive::HitBox2D getPointerHitbox()const;
	protected:
		std::string name;
		float shadow = 1;
		SpriteFont font;
		Color shadow_color = Color::shadow();
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

	class Button :public Widget
	{
	public:
		virtual void create(const std::string& name);
		virtual void render(const Canvas& canvas, CommandList cmd) override;
		virtual void update(const Canvas& canvas, float delta_time) override;
		void activate();
		void deactivate();
		primitive::HitBox2D hitbox;

		void onClickFunc(std::function<void(gui::EventArgs)> func) { on_click_func = func; }
	private:
		std::function<void(gui::EventArgs)> on_click_func;
	};

	class ComboBox :public Widget
	{
	public:
		void create(const std::string name);
		void addItem(const std::string name, const int value);
		void setSelected(int index);
		inline uint64_t getItemUserData(int index)const
		{
			if (index >= 0 && index < items.size())
			{
				return items[index].userdata;
			}
			return 0;
		}
		virtual void update(const Canvas& canvas, float delta_time) override;
		virtual void render(const Canvas& rhi, CommandList cmd) override;
		struct Item
		{
			std::string name;
			int value;
			uint64_t userdata;
		};
		std::vector<Item> items;
		void onSelect(std::function<void(EventArgs args)> function);

		int selected = -1;
	protected:
		std::function<void(EventArgs args)> on_select;
		bool drop_arrow = true;
		SpriteFont selected_font;
	};
}