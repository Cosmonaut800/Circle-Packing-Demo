#include <stdio.h>
#include <vector>
#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

using namespace std;

typedef struct Circle {
	unsigned int id;
	Vector2 position = Vector2Zero();
	float radius = 0.0f;
	Color color = BLACK;
} Circle_t;

enum STATE {IDLE = 0, PLACING_FREE = 1, PLACING_RESTR_1 = 2, PLACING_RESTR_2 = 3};

void FindIntersectionPoints(Circle_t c1, Circle_t c2, float margin, Vector2 *p1, Vector2 *p2);

int main(int argc, char **argv) {
	Circle_t tempCircle;
	vector<Circle_t> circles;
	int circleCount = 0;
	int state = IDLE;
	Vector2 intersection1 = Vector2Zero();
	Vector2 intersection2 = Vector2Zero();


	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Circle Packing Demonstration");
	SetTargetFPS(60);

	while(!WindowShouldClose()) {
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (state == IDLE) {
				tempCircle.id = circleCount;
				tempCircle.color = ColorFromHSV((float)GetRandomValue(0, 360), 1.0f, 1.0f);
				tempCircle.color.a = 64;
				tempCircle.radius = (float)GetRandomValue(100, 1000)/10.0f;

				if ((circleCount % 3) < 1) {
					state = PLACING_FREE;
				}
				else if ((circleCount % 3) == 1) {
					state = PLACING_RESTR_1;
				}
				else if ((circleCount % 3) > 1) {
					state = PLACING_RESTR_2;
				}
			}
			else {
				tempCircle.color.a = 255;
				circleCount++;
				circles.push_back(tempCircle);
				state = IDLE;
			}
		}
		
		if (state == PLACING_FREE) {
			tempCircle.position = GetMousePosition();
		}
		else if (state == PLACING_RESTR_1) {
			tempCircle.position = Vector2Add(circles[circles.size()-1].position, Vector2Scale(Vector2Normalize(Vector2Subtract(GetMousePosition(), circles[circles.size()-1].position)), tempCircle.radius + circles[circles.size()-1].radius));
		}
		else if (state == PLACING_RESTR_2) {
			FindIntersectionPoints(circles[circles.size()-2], circles[circles.size()-1], tempCircle.radius, &intersection1, &intersection2);
			if (Vector2DistanceSqr(intersection1, GetMousePosition()) > Vector2DistanceSqr(intersection2, GetMousePosition())) {
				tempCircle.position = intersection2;
			}
			else {
				tempCircle.position = intersection1;
			}
		}

		BeginDrawing();
		ClearBackground(WHITE);
        
		for (int i=0; i < circles.size(); i++) {
			DrawCircleV(circles[i].position, circles[i].radius, circles[i].color);
			DrawCircleLinesV(circles[i].position, circles[i].radius, BLACK);
		}
		
		if (state != IDLE) {
			DrawCircleV(tempCircle.position, tempCircle.radius, tempCircle.color);
			if (state == PLACING_RESTR_1) {
				DrawCircleLinesV(circles[circles.size()-1].position, circles[circles.size()-1].radius + tempCircle.radius, RED);
			}
			else if (state == PLACING_RESTR_2) {
				DrawCircleLinesV(circles[circles.size()-2].position, circles[circles.size()-2].radius + tempCircle.radius, RED);
				DrawCircleLinesV(circles[circles.size()-1].position, circles[circles.size()-1].radius + tempCircle.radius, BLUE);
				DrawCircleV(intersection1, 10.0f, ORANGE);
				DrawCircleV(intersection2, 10.0f, ORANGE);
			}
		}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}

void FindIntersectionPoints(Circle_t c1, Circle_t c2, float margin, Vector2 *p1, Vector2 *p2) {
	float d, l, h;
	
	c1.radius += margin;
	c2.radius += margin;
	
	d = sqrt(((c1.position.x - c2.position.x)*(c1.position.x - c2.position.x)) + ((c1.position.y - c2.position.y)*(c1.position.y - c2.position.y)));
	l = ((c1.radius*c1.radius) - (c2.radius*c2.radius) + (d*d))/(2*d);
	h = sqrt((c1.radius*c1.radius) - (l*l));
	
	p1->x = ((l/d) * (c2.position.x - c1.position.x)) + ((h/d) * (c2.position.y - c1.position.y)) + c1.position.x;
	p1->y = ((l/d) * (c2.position.y - c1.position.y)) - ((h/d) * (c2.position.x - c1.position.x)) + c1.position.y;
	
	p2->x = ((l/d) * (c2.position.x - c1.position.x)) - ((h/d) * (c2.position.y - c1.position.y)) + c1.position.x;
	p2->y = ((l/d) * (c2.position.y - c1.position.y)) + ((h/d) * (c2.position.x - c1.position.x)) + c1.position.y;
}