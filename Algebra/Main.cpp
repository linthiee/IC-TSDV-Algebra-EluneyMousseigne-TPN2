#include <ctime>
#include <iostream>
#include <cmath>
#include "raylib.h"
#include "Vectors.h"
#include <vector>

struct Step
{
	float side;
	float bottomY;
	float height;
	bool inverted;
};

const int magnitudeMax = 10;
const int magnitudeMin = 5;
const int magnitudeRange = (magnitudeMax - magnitudeMin + 1);

const int screenWidth = 1080;
const int screenHeight = 625;

void buildPyramidRecursive(int currentStep, int totalSteps, float baseSide, float stepHeight, std::vector<Step>& steps);
void buildMirrored(int mirrors, int stepsNum, float stepHeight, float currentTopY, const std::vector<Step>& baseSteps, std::vector<Step>& steps);
void calculateGeometry(const std::vector<Step>& steps);

void main()
{
	srand(time(nullptr));

	float currentTopY = 0;

	Camera3D camera = { 0 };

	Vector3 vectorA;
	Vector3 vectorB;
	Vector3 vectorC;

	camera.position = { 5.0f, 5.0f, 5.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	float cameraSpeed = 0.2f;

	vectorA.x = (float)(rand() % magnitudeRange + magnitudeMin);
	vectorA.y = (float)(rand() % magnitudeRange + magnitudeMin);
	vectorA.z = (float)(rand() % magnitudeRange + magnitudeMin);

	vectorB.y = -vectorA.x;
	vectorB.x = vectorA.y;
	vectorB.z = 0.0f;

	int n = 0;
	std::cout << "Enter N (number of steps): ";
	std::cin >> n;

	if (n <= 0)
	{
		n = 1;
	}

	vectorC.x = vectorA.y * vectorB.z - vectorA.z * vectorB.y;
	vectorC.y = vectorA.z * vectorB.x - vectorA.x * vectorB.z;
	vectorC.z = vectorA.x * vectorB.y - vectorA.y * vectorB.x;

	float normalizeA = sqrtf(vectorA.x * vectorA.x + vectorA.y * vectorA.y + vectorA.z * vectorA.z);
	float normalizeB = sqrtf(vectorB.x * vectorB.x + vectorB.y * vectorB.y + vectorB.z * vectorB.z);
	float normalizeC = sqrtf(vectorC.x * vectorC.x + vectorC.y * vectorC.y + vectorC.z * vectorC.z);

	if (normalizeA == 0)
	{
		normalizeA = 1.0f;
	}
	if (normalizeB == 0)
	{
		normalizeB = 1.0f;
	}
	if (normalizeC == 0)
	{
		normalizeC = 1.0f;
	}

	vectorA.x /= normalizeA;
	vectorA.y /= normalizeA;
	vectorA.z /= normalizeA;

	vectorB.x /= normalizeB;
	vectorB.y /= normalizeB;
	vectorB.z /= normalizeB;

	vectorC.x /= normalizeC;
	vectorC.y /= normalizeC;
	vectorC.z /= normalizeC;

	float magnitude = (float)(rand() % magnitudeRange + magnitudeMin);

	vectorA.x *= magnitude;
	vectorA.y *= magnitude;
	vectorA.z *= magnitude;

	vectorB.x *= magnitude;
	vectorB.y *= magnitude;
	vectorB.z *= magnitude;

	float magnitudeC = ((1.0f / (float)n) * magnitude);

	vectorC.x *= magnitudeC;
	vectorC.y *= magnitudeC;
	vectorC.z *= magnitudeC;

	int stepsNum = n;
	float stepHeight = magnitudeC;
	float baseSide = magnitudeC * 10.0f; // lado de la base

	int mirrors = 0;
	std::cout << "Enter how many times you want your pyramid mirrored: ";
	std::cin >> mirrors;

	if (mirrors < 0)
	{
		mirrors = 0;
	}

	InitWindow(screenWidth, screenHeight, "Algebra - Stepped Pyramid");
	SetTargetFPS(60);

	std::vector<Step> steps;
	steps.reserve(stepsNum * (1 + mirrors) * 2);

	//construir piramide original y guardarla en baseSteps
	std::vector<Step> baseSteps;
	buildPyramidRecursive(0, stepsNum, baseSide, stepHeight, baseSteps);

	//poner la piramide original al final del vector
	steps.insert(steps.end(), baseSteps.begin(), baseSteps.end());

	//currentTopY altura total de la piramide original
	currentTopY = stepsNum * stepHeight;
	buildMirrored(mirrors, stepsNum, stepHeight, currentTopY, baseSteps, steps);

	while (!WindowShouldClose())
	{
		UpdateCamera(&camera, CAMERA_FREE);
		DisableCursor();

		if (IsKeyDown(KEY_RIGHT))
		{
			camera.position.x += cameraSpeed;
		}
		if (IsKeyDown(KEY_LEFT))
		{
			camera.position.x -= cameraSpeed;
		}
		if (IsKeyDown(KEY_UP))
		{
			camera.position.z -= cameraSpeed;
		}
		if (IsKeyDown(KEY_DOWN))
		{
			camera.position.z += cameraSpeed;
		}
		if (IsKeyDown(KEY_Q))
		{
			camera.position.y += cameraSpeed;
		}
		if (IsKeyDown(KEY_E))
		{
			camera.position.y -= cameraSpeed;
		}

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);

		DrawGrid(20, 5.0f);

		for (int step = 0; step < steps.size(); step++)
		{
			Vector3 center = { 0.0f, steps[step].bottomY + steps[step].height / 2.0f, 0.0f };
			Color col;

			if (steps[step].inverted)
			{
				col = BLUE;
			}
			else
			{
				col = RED;
			}

			DrawCube(center, steps[step].side, steps[step].height, steps[step].side, col);
			DrawCubeWires(center, steps[step].side, steps[step].height, steps[step].side, WHITE);
		}

		EndMode3D();

		EndDrawing();
	}

	CloseWindow();

	calculateGeometry(steps);
}

void buildPyramidRecursive(int currentStep, int totalSteps, float baseSide, float stepHeight, std::vector<Step>& steps)
{
	if (currentStep >= totalSteps)
	{
		return;
	}

	float stepLength = baseSide * (float)(totalSteps - currentStep) / (float)totalSteps;
	float bottomY = currentStep * stepHeight;
	steps.push_back({ stepLength, bottomY, stepHeight, false });

	buildPyramidRecursive(currentStep + 1, totalSteps, baseSide, stepHeight, steps);
}

void buildMirrored(int mirrors, int stepsNum, float stepHeight, float currentTopY, const std::vector<Step>& baseSteps, std::vector<Step>& steps)
{
	float topY = currentTopY;
	int timesMirrored = 1;
	bool inverted = false;

	for (int m = 1; m <= mirrors; m++)
	{
		if (!inverted)
		{
			for (int k = stepsNum - 2; k >= 0; k--)
			{
				Step orig = baseSteps[k];
				float bottomY = topY;
				steps.push_back({ orig.side, bottomY, stepHeight, !inverted });
				topY += stepHeight;
			}
		}
		else
		{
			for (int k = 1; k <= stepsNum - 1; k++)
			{
				Step orig = baseSteps[k];
				float bottomY = topY;
				steps.push_back({ orig.side, bottomY, stepHeight, !inverted });
				topY += stepHeight;
			}
		}

		inverted = !inverted;
		timesMirrored++;
	}
}

void calculateGeometry(const std::vector<Step>& steps)
{
	double totalPerimeter = 0.0;
	double totalArea = 0.0;
	double totalVolume = 0.0;

	for (int step = 0; step < steps.size(); step++)
	{
		double side = steps[step].side;
		double height = steps[step].height;

		double perimeterTopFace = 4.0 * side;
		double perimeterBtmFace = 4.0 * side;
		double perimeterEachLateral = 2.0 * (side + height);
		double perimeterTotalLateral = 4.0 * perimeterEachLateral;

		double perimeterStep = perimeterTopFace + perimeterBtmFace + perimeterTotalLateral;
		totalPerimeter += perimeterStep;

		double areaStep = 2.0 * (side * side) + 4.0 * (side * height);
		totalArea += areaStep;

		double volStep = side * side * height;
		totalVolume += volStep;
	}

	std::cout << "\n Geometry of the figure: \n";
	std::cout << "Total perimeter (sum of perimeters of all faces): " << totalPerimeter << " cm" << "\n";
	std::cout << "Total surface area (sum of areas of all faces): " << totalArea << " cm2" << "\n";
	std::cout << "Total volume: " << totalVolume << " cm3" << "\n";
}