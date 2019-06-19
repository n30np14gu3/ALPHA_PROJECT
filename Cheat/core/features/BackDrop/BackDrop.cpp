#include "backdrop.h"


std::vector<Dot*> Dots;


void BackDrop::DrawBackDrop()
{

	static int width, height;
	interfaces::engine->get_screen_size(width, height);
	interfaces::surface->set_drawing_color(0, 0, 0, 179);
	interfaces::surface->draw_filled_rectangle(0, 0, width, height);

	int s = rand() % 24;

	if (s == 0)
		Dots.push_back(new Dot(vec2_t(rand() % (int)width, -16), vec2_t((rand() % 7) - 3, rand() % 3 + 1)));
	else if (s == 1)
		Dots.push_back(new Dot(vec2_t(rand() % (int)width, (int)height + 16), vec2_t((rand() % 7) - 3, -1 * (rand() % 3 + 1))));
/*	else if (s == 2)
		Dots.push_back(new Dot(Vector2D(-16, rand() % (int)height), Vector2D(rand() % 3 + 1, (rand() % 7) - 3)));
	else if (s == 3)
		Dots.push_back(new Dot(Vector2D((int)width + 16, rand() % (int)height), Vector2D(-1 * (rand() % 3 + 1), (rand() % 7) - 3)));*/

	for (auto i = Dots.begin(); i < Dots.end();)
	{
		if ((*i)->RelativePosition.y < -20 || (*i)->RelativePosition.y > height + 20 || (*i)->RelativePosition.x < -20 || (*i)->RelativePosition.x > width + 20)
		{
			delete (*i);
			i = Dots.erase(i);
		}
		else
		{
			(*i)->RelativePosition = (*i)->RelativePosition + (*i)->Velocity * (0.3);
			i++;
		}
	}
	for (auto i = Dots.begin(); i < Dots.end(); i++)
		(*i)->Draw();
}

