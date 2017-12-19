#include <iostream>
#define CIMGGIP_MAIN
#include "CImgGIP05.h"
using namespace std;

struct Box
{
	int x;
	int y;
	int delta_y; // aktuelle Fallgeschwindigkeit dieses Kaestchens
};
bool links = false;
const int box_max = 55, box_size = 25, fighterLength = 30, fighterHeight = 10, fighterIncrement = 20, bulletRad = 8;
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
		gip_draw_rectangle(boxes[i].x, boxes[i].y, (boxes[i].x + box_size), (boxes[i].y + box_size), white);
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
void draw_bulets(vector<Box> bullets)
{
  for each (Box bullet in bullets)
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
  // Ist die Kugel oben angekommen => löschen
  vector<Box> delBullets;
  for each (Box bullet in bullets)
  {
    if (bullet.y <= 0)
    {
      delBullets.push_back(bullet);
    }
  }

  for each (Box delBullet in delBullets)
  {
    //bullets.erase(std::remove(bullets.begin(), bullets.end(), delBullet), bullets.end());
  }


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
		}
	}
	while (keep_going && gip_window_not_closed())
	{
		draw_boxes(boxes);
    draw_bulets(bullets);
    draw_fighter(fighter);

		for (int loop_count = 0; loop_count < 200; loop_count++)
		{
			gip_wait(delta_wait); // warte 5 Milli-Sekunden
		}
		// Berechne die neue Fall-Position aller Kaestchen und pruefe,
		// ob eines der Kaestchen unten aus dem Fenster "herausgefallen" ist
		// (falls ja, wird der Wert false zurueckgegeben) ...

    if ((GetAsyncKeyState('a') || GetAsyncKeyState(VK_LEFT)) && 0x8000)
    {
      // nach links bewegen
      fighter.x = ((fighter.x - fighter.delta_y) < 0) ? 0 : (fighter.x - fighter.delta_y);
    }
    else
      if ((GetAsyncKeyState('d') || GetAsyncKeyState(VK_RIGHT)) && 0x8000)
      {
        // nach rechts bewegen
        fighter.x = ((fighter.x + fighter.delta_y) > gip_win_size_x) ? gip_win_size_x : (fighter.x + fighter.delta_y);
      }
      else
        if (GetAsyncKeyState(VK_SPACE) && 0x8000)
        {
          // schießen (neues Objekt erstellen)
          Box bullet{ fighter.x + (fighterLength / 2), fighter.y , 10 };
          bullets.push_back(bullet);
        }

    update_bullets(bullets);

    CheckCollisions(boxes, bullets);

		keep_going = update_boxes(boxes);
	}
	return 0;
}