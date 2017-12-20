#include <iostream>
#define CIMGGIP_MAIN
#include "CImgGIP05.h"
using namespace std;

struct Box
{
	int x;
	int y;
	int delta_y; // aktuelle Fallgeschwindigkeit dieses Kaestchens
	bool draw = true;
};
bool links = false;
const int box_max = 55, box_size = 25, fighterLength = 30, fighterHeight = 10, fighterIncrement = 20, bulletRad = 4;
double delta_x = 8, delta_wait = 3;

void draw_boxes(Box boxes[])
{
	//	gip_white_background();
	gip_background(0);
	// Vor dem Malen updates stoppen, danach starten, um Flackern zu vermeiden
	gip_stop_updates();
	// Alle Boxen durchlaufen und an Position x/y zeichnen
	for (int i = 0; i < box_max; i++)
	{
		if (boxes[i].draw)
		{
			gip_draw_rectangle(boxes[i].x, boxes[i].y, (boxes[i].x + box_size), (boxes[i].y + box_size), white);
		}
	}
	gip_start_updates();
}

// Zeichnet die Spielfigur.
void draw_fighter(Box fighter)
{
	gip_draw_rectangle(fighter.x,
		fighter.y,
		(fighter.x + fighterLength),
		(fighter.y + fighterHeight),
		white);
}

// Kugeln zeichnen
void draw_bullets(vector<Box> bullets)
{
	for (Box bullet : bullets)
	{
		gip_draw_circle(bullet.x, bullet.y, bulletRad, white);
	}
}

bool update_boxes(Box boxes[])
{
	// Alle Boxen durchlaufen und delta_y auf y aufaddieren. Falls y dann größer als Fenstergroeße wird, "false" zurückgeben. Falls nicht, malen.
	for (int i = 0; i < box_max; i++)
	{
		if (boxes[i].x + box_size >= gip_win_size_x - box_size - 10 || boxes[i].x <= 30)
		{
			links = !links;
			for (int i = 0; i < box_max; i++)
			{
				if ((boxes[i].y + boxes[i].delta_y) > gip_win_size_y - 50) return false;
				boxes[i].y += boxes[i].delta_y;
			}
			//			delta_x += 0.3;
			if (delta_wait > 1.2) delta_wait -= 0.035;
			draw_boxes(boxes);
			gip_wait(180);
		}
	}
	for (int i = 0; i < box_max; i++)
	{
		if (links)
		{
			boxes[i].x -= delta_x;
		}
		else
		{
			boxes[i].x += delta_x;
		}
	}
	return true;
}

void CheckCollisions(Box boxes[], vector<Box> &bullets)
{
	int bulletCount = 0, boxCount = 0;
	// Ist die Kugel oben angekommen => löschen
	vector<Box> delBullets;
	for (Box bullet : bullets)
	{
		if (bullet.y <= 0)
		{
			bullets.erase(bullets.begin() + bulletCount);
		}
		for (int i = 0; i < box_max; i++)
		{
			if (boxes[i].x <= bullet.x - bulletRad && boxes[i].x + box_size >= bullet.x)
			{
				if (boxes[i].y <= bullet.y && boxes[i].y + box_size >= bullet.y)
				{
					bullets.erase(bullets.begin() + bulletCount);
					boxes[i].draw = false;
					boxCount = i;
					break;
				}
			}
		}
		if (!boxes[boxCount].draw) break;
		bulletCount++;
	}

	//for (Box delBullet : delBullets)
	//{
	//	// Kugeln beim verlassen d. Spielfeldes / Kollision loeschen
	//	//bullets.erase(std::remove(bullets.begin(), bullets.end(), delBullet), bullets.end());
	//}
	// trifft die Kugel das Quadrat?
	// => beides entfernen
}

void update_bullets(vector<Box> &bullets)
{
	/*for each (Box bullet in bullets)
	{
	bullet.y = (bullet.y - bullet.delta_y);
	}*/
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i].y = (bullets[i].y - bullets[i].delta_y);
	}
}

int main()
{
	Box boxes[box_max] = { 0 };
	bool keep_going = true;
	int tick = 0;

	// Geschosse
	vector<Box> bullets;

	//gip_win_size_y und gip_win_size_x sind die Fensterkoordinaten ganz unten
	Box fighter = { 0, (gip_win_size_y - fighterHeight), fighterIncrement };

	// Initialisiere alle Kaestchen ...
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 11; i++)
		{
			// Die "+20" sorgen fuer einen Zwischenraum von 20 zwischen den Kaestchen ...
			boxes[j * 11 + i].x = i * (box_size + 15) + 50;
			boxes[j * 11 + i].y = j * 40 + 10;
			// Startgeschwindigkeit ist ganzzahlige Zufallszahl zwischen 10 und 30 ...
			boxes[j * 11 + i].delta_y = 5;//gip_random(10, 30);
			boxes[j * 11 + i].draw = true;
		}
	}
	draw_boxes(boxes);
	while (keep_going && gip_window_not_closed())
	{
		tick++;
		if (tick % 50 == 0)
		{
			keep_going = update_boxes(boxes);//}
		}
			//gip_wait(delta_wait); // warte 5 Milli-Sekunden
		draw_boxes(boxes);
		draw_bullets(bullets);
		draw_fighter(fighter);
		// Berechne die neue Fall-Position aller Kaestchen und pruefe,
		// ob eines der Kaestchen unten aus dem Fenster "herausgefallen" ist
		// (falls ja, wird der Wert false zurueckgegeben) ...

		if (GetAsyncKeyState('A') && 0x8000) //|| GetAsyncKeyState(VK_LEFT)) && 0x8000)
		{
			// nach links bewegen
			fighter.x = ((fighter.x - fighter.delta_y) < 0) ? 0 : (fighter.x - fighter.delta_y);
		}
		if (GetAsyncKeyState('D') && 0x8000) //|| GetAsyncKeyState(VK_RIGHT)) && 0x8000)
		{
			// nach rechts bewegen
			fighter.x = ((fighter.x + fighter.delta_y) > gip_win_size_x) ? gip_win_size_x : (fighter.x + fighter.delta_y);
		}
		if (GetAsyncKeyState(VK_SPACE) && 0x8000)
		{
			if (tick % 25 == 0)
			{
			// schießen (neues Objekt erstellen)
			Box bullet{ fighter.x + (fighterLength / 2), fighter.y , 10 };
			bullets.push_back(bullet);
			}
		}

		update_bullets(bullets);

		CheckCollisions(boxes, bullets);
	}
	return 0;
}