#pragma once
#include "ScriptBehavior.h"
#include "Grid.h"

#define E_PI 3.141592

struct WaveSolver : public ScriptBehavior
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	float SectionsX = 100;
	float SectionsY = 100;
	float SizeX = 1;
	float SizeY = 1;
	float SpacingX = 1.0f;
	float SpacingY = 1.0f;
	float WaveScaleX = 0.0005f;
	float WaveScaleY = 0.001f;
	float DeltaX = 1/SectionsX;
	float DeltaY = 1/SectionsY;
	float ** FourierCoefficientAmn = nullptr;
	float ConstantA = 1.0f;
	float ConstantB = 1.0f;
	float ConstantC = 0.5f;
	float ** WaveHeight = nullptr;
	float TotalTime = 0.0f;
	float TimeStep = 0.00016f;
	float Displacement = 5.0f;
	const float PI = E_PI;

	Grid WaveGrid = Grid(10, 10, 10, 10);
private:
	float lastInterval;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	virtual void Initialize() override;
	virtual void Behavior() override;
	virtual void Destroy() override;

	float WaveControlFunction(float x, float y);
	void CalculateFourierCoefficients();
	void CalculateHeightValues();
	void CreateGrid();
	void UpdateGrid();
	void DrawWaveSolverSettingsWidget();
};