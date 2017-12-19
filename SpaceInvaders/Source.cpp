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
const int box_max = 55, box_size = 25, delta_x = 10;

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

bool update_boxes(Box boxes[])
{
	// Alle Boxen durchlaufen und delta_y auf y aufaddieren. Falls y dann größer als Fenstergroeße wird, "false" zurückgeben. Falls nicht, malen.

	for (int i = 0; i < box_max; i++)
	{
		if (boxes[i].x + box_size >= gip_win_size_x - box_size || boxes[i].x <= 10)
		{
			links = !links;
			for (int i = 0; i < box_max; i++)
			{
				boxes[i].y += boxes[i].delta_y;
			}
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
		if ((boxes[i].y + boxes[i].delta_y) > gip_win_size_y) return false;
	}
	return true;
}

int main()
{
	Box boxes[box_max] = { 0 };
	bool keep_going = true;
	// Initialisiere alle Kaestchen ...
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 11; i++)
		{
			// Die "+20" sorgen fuer einen Zwischenraum von 20 zwischen den Kaestchen ...
			boxes[j * 11 + i].x = i * (box_size + 15) + 50;
			boxes[j * 11 + i].y = j * 50 + 10;
			// Startgeschwindigkeit ist ganzzahlige Zufallszahl zwischen 10 und 30 ...
			boxes[j * 11 + i].delta_y = 5;//gip_random(10, 30);
		}
	}
	while (keep_going && gip_window_not_closed())
	{
		draw_boxes(boxes);
		for (int loop_count = 0; loop_count < 200; loop_count++)
		{
			gip_wait(3); // warte 5 Milli-Sekunden
		}
		// Berechne die neue Fall-Position aller Kaestchen und pruefe,
		// ob eines der Kaestchen unten aus dem Fenster "herausgefallen" ist
		// (falls ja, wird der Wert false zurueckgegeben) ...
		keep_going = update_boxes(boxes);
	}
	return 0;
}