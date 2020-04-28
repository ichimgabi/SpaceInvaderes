#include<stdio.h>
#include<allegro5/allegro.h>
#include<allegro5/allegro_image.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_ttf.h>

struct enemy{
	int location_x, location_y;
	bool live;
	ALLEGRO_BITMAP *image;
};

void init()
{
	al_init();
	al_init_image_addon();
	al_install_keyboard();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
}

int collision(int x1, int y1, int h1, int w1, int x2, int y2, int h2, int w2)
{
	if(x1 >= x2 + w2 || y1 >= y2 + h2 || x2 >= x1 + w1 || y2 >= y1 + h1)
		return 0;
	return 1;
}

enum KEY_ENUM
{
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

int main(int argc, char *argv[])
{
	int  display_w = 800, display_h = 600, time_elapsed = 0;
	int player_x = display_w / 2, player_y = display_h - 50;
	int nr_enemies = 30, enemies_left = nr_enemies, nr_rows = 3, i, j, f, b, front = 9, back = 0;
	bool redraw = true, forward = true, end = false;
	int live = 0, speed = -10, bullet_y, bullet_x;
	bool key[4] = {false, false, false, false};
	struct enemy enemies[nr_enemies];

	init();

	ALLEGRO_DISPLAY	*display;
	ALLEGRO_BITMAP *player_img = NULL;
	ALLEGRO_BITMAP *enemy_img1 = NULL;
	ALLEGRO_BITMAP *enemy_img2 = NULL;
	ALLEGRO_BITMAP *enemy_img3 = NULL;
	ALLEGRO_EVENT_QUEUE *queue = NULL;
	ALLEGRO_TIMER *fps_timer = NULL;
	ALLEGRO_TIMER *alien_timer = NULL;
	ALLEGRO_TIMER *game_timer = NULL;
	ALLEGRO_FONT *font = NULL;

	display = al_create_display(display_w, display_h);
	font = al_load_font("OpenSans-Regular.ttf", 12, 0);
	player_img = al_load_bitmap("player.png");
	enemy_img1 = al_load_bitmap("alien1.png");
	enemy_img2 = al_load_bitmap("alien2.png");
	enemy_img3 = al_load_bitmap("alien3.png");

	fps_timer = al_create_timer(1.0 / 60);
	alien_timer = al_create_timer(1.0 / 5);
	game_timer = al_create_timer(1.0);

	queue = al_create_event_queue();

	for(i=0; i < nr_enemies / nr_rows; i++)
	{
		enemies[i].location_x = 60 + i * 70;
		enemies[i].location_y = 40;
		enemies[i].live = true;
		enemies[i].image = enemy_img1;
	}
	for(i = nr_enemies / nr_rows; i < nr_enemies / nr_rows * 2; i++)
	{
		enemies[i].location_x = 60 + (i - nr_enemies / nr_rows) * 70;
		enemies[i].location_y = 100;
		enemies[i].live = true;
		enemies[i].image = enemy_img2;
	}
	for(i = nr_enemies / nr_rows * 2; i < nr_enemies / nr_rows * 3; i++)
	{
		enemies[i].location_x = 60 + (i - nr_enemies / nr_rows * 2) * 70;
		enemies[i].location_y = 160;
		enemies[i].live = true;
		enemies[i].image = enemy_img3;
	}

	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(fps_timer));
	al_register_event_source(queue, al_get_timer_event_source(alien_timer));
	al_register_event_source(queue, al_get_timer_event_source(game_timer));
	al_register_event_source(queue, al_get_keyboard_event_source());

	al_start_timer(fps_timer);
	al_start_timer(alien_timer);
	al_start_timer(game_timer);

	while(1)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);

		if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;

		if(event.timer.source == game_timer)
			time_elapsed++;

		if(event.timer.source == alien_timer)
		{
			if(enemies[front].location_x < 780 && forward == true)
				for(i=0;i<nr_enemies;i++)
					if(enemies[i].live == true)
						enemies[i].location_x += 10;
			if(enemies[front].location_x >= 780)
			{
				forward = false;
				for(i=0;i<nr_enemies;i++)
					if(enemies[i].live == true)
						enemies[i].location_y += 30;
			}
			if(enemies[back].location_x > 0 && forward == false)
				for(i=0;i<nr_enemies;i++)
					if(enemies[i].live == true)
						enemies[i].location_x -= 10;
			if(enemies[back].location_x <= 0)
			{
				forward = true;
				for(i=0;i<nr_enemies;i++)
					if(enemies[i].live == true)
						enemies[i].location_y += 30;
			}

			for(j=0;j<nr_enemies;j++)
				if(collision(player_x,player_y,64,64,enemies[j].location_x,enemies[j].location_y,64,64))
					if(enemies[j].live == true)
					{
						end = true;
						break;
					}

			if(end == true)
				break;
		}

		if(event.timer.source == fps_timer)
		{
			redraw = true;
			if(live == 1)
			{
				if(bullet_y < 0)
					live = 0;
				else
					bullet_y += speed;
			}

			if(live == 1)
			{
				for(j=0;j<nr_enemies;j++)
				{
					if(enemies[j].live == true)
					{
						if(collision(bullet_x,bullet_y,2,2,enemies[j].location_x,enemies[j].location_y,32,32))
						{
							enemies[j].live = false;
							enemies_left --;;
							live = 0;
							if(j == front)
							{
								for(f=9;f>=0;f--)
									if(enemies[f].live == true)
									{
										front = f;
										break;
									}
							}
							if(j == back)
							{
								for(b=0;b<nr_enemies/nr_rows;b++)
									if(enemies[b].live == true)
									{
										back = b;
										break;
									}
							}
							break;
						}
					}
				}
			}

			if(enemies_left == 0)
				break;

			if(key[KEY_LEFT] && player_x > 0)
				player_x-=5;
			if(key[KEY_RIGHT] && player_x < 762)
				player_x+=5;
		}
		else
			if(event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch(event.keyboard.keycode)
				{
					case ALLEGRO_KEY_RIGHT:
						key[KEY_RIGHT] = true;
						break;
					case ALLEGRO_KEY_LEFT:
						key[KEY_LEFT] = true;
						break;
					case ALLEGRO_KEY_SPACE:
						if(live == 0)
						{
							live = 1;
							bullet_x = player_x + 32/2;
							bullet_y = player_y;
						}
						break;
				}
			}
		else 
			if(event.type == ALLEGRO_EVENT_KEY_UP)
			{
				switch(event.keyboard.keycode)
				{
					case ALLEGRO_KEY_RIGHT:
						key[KEY_RIGHT] = false;
						break;
					case ALLEGRO_KEY_LEFT:
						key[KEY_LEFT] = false;
						break;
				}
			}

		if(redraw && al_is_event_queue_empty(queue))
		{
			redraw = false;
			al_clear_to_color(al_map_rgb(0,0,0));
			al_draw_scaled_bitmap(player_img,0,0,32,32,player_x,player_y,32,32,0);
			al_draw_textf(font, al_map_rgb(255,255,255), 10, 10, 0, "Time elapsed: %d", time_elapsed);

			if(live == 1)
				al_draw_filled_circle(bullet_x,bullet_y,2,al_map_rgb(255,255,255));

			for(i=0;i<nr_enemies;i++)
			{
				if(enemies[i].live == true && i < 10)
					al_draw_scaled_bitmap(enemies[i].image,0,0,32,24,enemies[i].location_x, enemies[i].location_y,32,32,0);
				if(enemies[i].live == true && i > 9 && i < 20)
					al_draw_scaled_bitmap(enemies[i].image,0,0,32,24,enemies[i].location_x, enemies[i].location_y,32,32,0);
				if(enemies[i].live == true && i > 19)
					al_draw_scaled_bitmap(enemies[i].image,0,0,32,24,enemies[i].location_x, enemies[i].location_y,32,32,0);
			}
			al_flip_display();
		}
	}

	al_destroy_display(display);
	al_destroy_bitmap(player_img);
	al_destroy_event_queue(queue);
	al_destroy_timer(fps_timer);
	al_destroy_timer(game_timer);
	al_destroy_font(font);

	return 0;
}