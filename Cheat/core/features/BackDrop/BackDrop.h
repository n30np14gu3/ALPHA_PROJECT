#pragma once



class Dot;
#include "../../../dependencies/common_includes.hpp"
extern std::vector<Dot*> Dots;

class Dot
{
public:
	Dot(vec2_t p, vec2_t v)
	{
		RelativePosition = p;
		Velocity = v;
		
	}

	virtual void Draw()
	{
		interfaces::surface->set_drawing_color(69, 140, 230, 255);

		interfaces::surface->draw_filled_rectangle(RelativePosition.x - 2, RelativePosition.y - 2, 4, 4);
		auto t = std::find(Dots.begin(), Dots.end(), this);
		if (t == Dots.end()) return;
		for (auto i = t; i != Dots.end(); i++)
		{
			if ((*i) == this) continue;
			vec2_t Pos = RelativePosition;
			
			float Dist = Pos.distance_to((*i)->RelativePosition);
			if (Dist < 128)
			{
				vec2_t Dir = ((*i)->RelativePosition - RelativePosition);
				Dir.normalize();
				int alpha = 128 / 100 * Dist * 255;
				interfaces::surface->set_drawing_color(69, 140, 230, alpha);
				interfaces::surface->draw_line(Pos.x, Pos.y, (Pos + Dir * Dist).x, (Pos + Dir * Dist).y);
			
			}
		}
	}

	vec2_t RelativePosition = vec2_t(0, 0);
	vec2_t Velocity;
};

namespace BackDrop
{
	void DrawBackDrop();

}


