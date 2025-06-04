#include "gui.h"
#include <atomic>
#include <cassert>
#include <iostream>
#include "core/math/vector2.h"
namespace qyhs::gui
{
	void ComboBox::create(const std::string name)
	{
		setName(name);

		font.params.h_align = font::FONT_ALIGN_RIGHT;
		font.params.v_align = font::FONT_ALIGN_CENTER;
		// Create a ComboBox

		selected_font = font;
		selected_font.params.h_align = font::FONT_ALIGN_CENTER;
		selected_font.params.v_align = font::FONT_ALIGN_CENTER;
	}

	void ComboBox::addItem(const std::string name, const int value)
	{
		items.emplace_back();
		ComboBox::Item& item = items.back();
		item.name = name;
		item.value = value;
		item.userdata = value;
		if (selected = -1)
		{
			selected = 0;
		}
	}

	void ComboBox::update(const Canvas& canvas, float delta_time)
	{
		Widget::update(canvas, delta_time);
		font.params.posY = translation.y + sprites[state].params.size.y * 0.5f;

		if (selected >= 0)
		{
			selected_font.setText(items[selected].name);
		}
		else
		{
			assert(0);
		}

		selected_font.params.posX = translation.x + scale.x * 0.5f;
		selected_font.params.posY = translation.y + scale.y * 0.5f;
		//selected_font.update(delta_time);
	}

	void Widget::setText(const char* text)
	{
		font.setText(text);
	}

	void ComboBox::render(const Canvas& rhi, CommandList cmd)
	{
		Widget::render(rhi, cmd);

		if (shadow > 0)
		{
			qyhs::image::Params params = sprites[state].params;
			params.pos.x -= shadow;
			params.pos.y -= shadow;
			params.size.x += shadow * 2;
			if (drop_arrow)
			{
				params.size.x += 1 + scale.y;
			}
			params.size.y += shadow * 2;
			params.color = shadow_color;
			params._flags |= image::Params::BACKGROUND;
			image::draw(nullptr, params, cmd);
		}
		font.setShadowColor(Color(0, 0, 0, 0));
		font.setColorRGBA(4294635680);
		font.draw(cmd);

		selected_font.draw(cmd);
	}

	void gui::ComboBox::onSelect(std::function<void(EventArgs args)> function)
	{
		on_select = function;
	}

	void gui::ComboBox::setSelected(int index)
	{
		selected = index;
		if (on_select != nullptr)
		{
			EventArgs args;
			args.userdata = getItemUserData(selected);
			on_select(args);
		}
	}

	void Widget::setName(const std::string value)
	{
		if (value.length() <= 0)
		{
			static std::atomic<uint32_t> widget_id{ 0 };
			name = "widget_" + std::to_string(widget_id.fetch_add(1));
			//setSize(Vector2(100, 20));
		}
		else
		{
			name = value;
			// Set the name
		}
	}

	void Widget::setSize(const Vector2& size)
	{
		setDirty(true);
		m_transform.m_scale.x = size.x;
		m_transform.m_scale.y = size.y;
		m_transform.m_scale = m_transform.m_scale;

		updateWorldMatrix();
		scale = m_transform.m_scale;
	}

	void Widget::setPosition(const Vector2& position)
	{
		setDirty(true);
		m_transform.m_position.x = position.x;
		m_transform.m_position.y = position.y;
		updateWorldMatrix();
		translation = m_transform.m_position;
	}

	void Widget::update(const Canvas& canvas, float delta_time)
	{

		for (int i = IDLE; i < WIDGET_STATE_COUNT; ++i)
		{
			sprites[i].params.pos.x = translation.x;
			sprites[i].params.pos.y = translation.y;
			sprites[i].params.size.x = scale.x;
			sprites[i].params.size.y = scale.y;
		}

		font.params.posX = translation.x;
		font.params.posY = translation.y;
	}

	void Gui::addWeights(Widget* widget)
	{
		if (!widget)
		{
			return;
		}
		assert(std::find(widgets.begin(), widgets.end(), widget) == widgets.end());
		widgets.push_back(widget);
	}

	void Gui::render(const Canvas& canvas, CommandList cmd) const
	{
		RHI* rhi = rhi::getRHI();
		Rect scissorRect;
		scissorRect.bottom = (int32_t)(canvas.getPhysicalHeight());
		scissorRect.left = (int32_t)(0);
		scissorRect.right = (int32_t)(canvas.getPhysicalWidth());
		scissorRect.top = (int32_t)(0);
		rhi->beginEvent("GUI", cmd);
		for (int i = 0; i < widgets.size(); i++)
		{
			rhi->bindScissorRects(1, &scissorRect, cmd);
			widgets[i]->render(canvas, cmd);
		}
		rhi->bindScissorRects(1, &scissorRect, cmd);
		rhi->endEvent(cmd);
	}

	void Gui::update(const Canvas& canvas, float delta_time)
	{

		for (int i = 0; i < widgets.size(); ++i)
		{
			Widget* widget = widgets[i];
			widget->update(canvas, delta_time);
		}
	}

}