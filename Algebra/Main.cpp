#include <ctime>
#include <iostream>
#include <cmath>
#include "raylib.h"
#include "raymath.h"
#include "Vectors.h"
#include <vector>

struct Step
{
	float side;
	float bottomY;
	float height;
	bool inverted;
};

const int magnitudeRange = 20;

const int minAngle = 1;
const int maxAngle = 360;
const int angleRange = (maxAngle - minAngle + 1);

const int screenWidth = 1080;
const int screenHeight = 625;

void buildPyramidRecursive(int currentStep, int totalSteps, float baseSide, float stepHeight, std::vector<Step>& steps);
void buildMirrored(int mirrors, int stepsNum, float stepHeight, float currentTopY, const std::vector<Step>& baseSteps, std::vector<Step>& steps);
void calculateGeometry(const std::vector<Step>& steps);

float getVectorMagnitude(Vector3 vector);
void normalizeVector(Vector3& vector);
void scaleVector(Vector3& vector, float scalar);

void drawStep(Vector3 vectorA, Vector3 vectorB, Vector3 vectorC, float stepSide, float step, Color col);
void fillStep(Vector3 vectorA, Vector3 vectorB, Vector3 vectorC, Color col);

Vector3 operator+(Vector3& vector1, Vector3& vector2);

void main()
{
	srand(time(nullptr));

	float currentTopY = 0;

	float angle = rand() % (angleRange + minAngle);

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

	vectorA.x = (float)((rand()) / (float)(RAND_MAX / magnitudeRange)) - (magnitudeRange / 2.0f);
	vectorA.y = (float)((rand()) / (float)(RAND_MAX / magnitudeRange)) - (magnitudeRange / 2.0f);
	vectorA.z = (float)((rand()) / (float)(RAND_MAX / magnitudeRange)) - (magnitudeRange / 2.0f);

	float magnitude = getVectorMagnitude(vectorA);

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

	vectorC = Vector3CrossProduct(vectorA, vectorB);

	normalizeVector(vectorB);
	normalizeVector(vectorC);

	float magnitudeC = ((1.0f / (float)n) * magnitude);

	scaleVector(vectorB, magnitude);
	scaleVector(vectorC, magnitudeC);

	int stepsNum = n;
	float stepHeight = magnitudeC;
	float baseSide = magnitudeC * 2;// lado de la base

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
			Color col;

			if (steps[step].inverted)
			{
				col = BLUE;
			}
			else
			{
				col = RED;
			}

			drawStep(vectorA, vectorB, vectorC, steps[step].side, step, col);

			DrawLine3D(Vector3Zero(), vectorA, RED);
			DrawLine3D(Vector3Zero(), vectorB, GREEN);
			DrawLine3D(Vector3Zero(), vectorC, BLUE);
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

float getVectorMagnitude(Vector3 vector)
{
	return (sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z)));
}

void normalizeVector(Vector3& vector)
{
	float vectorMagnitude = getVectorMagnitude(vector);

	if (vectorMagnitude == 0)
	{
		vectorMagnitude = 1.0f;
	}

	vector.x /= vectorMagnitude;
	vector.y /= vectorMagnitude;
	vector.z /= vectorMagnitude;
}

void scaleVector(Vector3& vector, float scalar)
{
	vector.x *= scalar;
	vector.y *= scalar;
	vector.z *= scalar;
}

void drawStep(Vector3 vectorA, Vector3 vectorB, Vector3 vectorC, float stepSide, float step, Color col)
{
	Vector3 center = { vectorC.x * step, vectorC.y * step, vectorC.z * step };

	Vector3 getNormalizedVectorA = vectorA;
	normalizeVector(getNormalizedVectorA);
	scaleVector(getNormalizedVectorA, stepSide);

	Vector3 getNormalizedVectorB = vectorB;
	normalizeVector(getNormalizedVectorB);
	scaleVector(getNormalizedVectorB, stepSide);

	Vector3 corner1 = center - getNormalizedVectorA + getNormalizedVectorB;
	Vector3 corner2 = center + getNormalizedVectorA + getNormalizedVectorB;
	Vector3 corner3 = center - getNormalizedVectorA - getNormalizedVectorB;
	Vector3 corner4 = center + getNormalizedVectorA - getNormalizedVectorB;

	DrawLine3D(corner3, corner4, MAGENTA);
	DrawLine3D(corner4, corner2, MAGENTA);
	DrawLine3D(corner3, corner1, MAGENTA);
	DrawLine3D(corner1, corner2, MAGENTA);

	DrawLine3D(corner1, corner1 + vectorC, MAGENTA);
	DrawLine3D(corner2, corner2 + vectorC, MAGENTA);
	DrawLine3D(corner3, corner3 + vectorC, MAGENTA);
	DrawLine3D(corner4, corner4 + vectorC, MAGENTA);

	DrawSphere(center, 0.1f, MAGENTA);

	DrawSphere(corner1, 0.1f, GREEN);
	DrawSphere(corner2, 0.1f, BLUE);
	DrawSphere(corner3, 0.1f, RED);
	DrawSphere(corner4, 0.1f, YELLOW);

	DrawTriangle3D(corner1, corner2, corner3, col);
	DrawTriangle3D(corner2, corner4, corner3, col);

	center = { vectorC.x * (step + 1), vectorC.y * (step + 1), vectorC.z * (step + 1) };

	Vector3 corner5 = center - getNormalizedVectorA + getNormalizedVectorB;
	Vector3 corner6 = center + getNormalizedVectorA + getNormalizedVectorB;
	Vector3 corner7 = center - getNormalizedVectorA - getNormalizedVectorB;
	Vector3 corner8 = center + getNormalizedVectorA - getNormalizedVectorB;

	DrawLine3D(corner7, corner8, MAGENTA);
	DrawLine3D(corner8, corner6, MAGENTA);
	DrawLine3D(corner7, corner5, MAGENTA);
	DrawLine3D(corner5, corner6, MAGENTA);

	DrawTriangle3D(corner7, corner6, corner5, col);
	DrawTriangle3D(corner7, corner8, corner6, col);
	DrawTriangle3D(corner3, corner4, corner7, col);
	DrawTriangle3D(corner8, corner7, corner4, col);
	DrawTriangle3D(corner2, corner1, corner6, col);
	DrawTriangle3D(corner1, corner5, corner6, col);
	DrawTriangle3D(corner7, corner5, corner3, col);
	DrawTriangle3D(corner1, corner3, corner5, col);
	DrawTriangle3D(corner2, corner6, corner4, col);
	DrawTriangle3D(corner4, corner6, corner8, col);
}

void fillStep(Vector3 vectorA, Vector3 vectorB, Vector3 vectorC, Color col)
{
}

Vector3 operator+(Vector3& vector1, Vector3& vector2)
{
	return { vector1.x + vector2.x, vector1.y + vector2.y, vector1.z + vector2.z };
}