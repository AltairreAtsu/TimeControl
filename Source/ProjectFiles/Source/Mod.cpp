#include "GameAPI.h"
#include <string>
#include <iostream>
#include <fstream>

/************************************************************
	Config Variables (Set these to whatever you need. They are automatically read by the game.)
*************************************************************/
float TickRate = 1;

const int Hour_Glass_Block = 3042;
const int Pause_Block = 3043;
const int Fast_Forward_Block = 3044;
const int Rewind_Block = 3045;
const int Sunrise_Block = 3046;
const int Noon_Block = 3047;
const int Sunset_Block = 3048;
const int Midnight_Block = 3049;
const int Watch_Block = 3050;

const float Sunrise = 600;
const float Noon = 1200;
const float Sunset = 1800;
const float Midnight = 0;

UniqueID ThisModUniqueIDs[] = { Hour_Glass_Block, Pause_Block, Fast_Forward_Block, Rewind_Block, Sunrise_Block, Noon_Block, Sunset_Block, Midnight_Block, Watch_Block };

bool timeIsPaused = false;
float pauseTime = 0;

void GenerateHourGlassMenu(CoordinateInBlocks At) {
	bool areaIsClear = true;
	for (int i = 1; i <= 4; i++) {
		for (int ii = 0; ii <= 2; ii++) {
			if (GetBlock(At + (CoordinateInBlocks(i, 0, ii))).Type != EBlockType::Air) {
				areaIsClear = false;
			}
		}
	}
	if (areaIsClear) {
		SetBlock(At + CoordinateInBlocks(1, 0, 0), Rewind_Block);
		SetBlock(At + CoordinateInBlocks(2, 0, 0), Pause_Block);
		SetBlock(At + CoordinateInBlocks(3, 0, 0), Fast_Forward_Block);
		SetBlock(At + CoordinateInBlocks(4, 0, 0), Watch_Block);
		
		SetBlock(At + CoordinateInBlocks(1, 0, 2), Sunrise_Block);
		SetBlock(At + CoordinateInBlocks(2, 0, 2), Noon_Block);
		SetBlock(At + CoordinateInBlocks(3, 0, 2), Sunset_Block);
		SetBlock(At + CoordinateInBlocks(4, 0, 2), Midnight_Block);
	}
	else {
		SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"No Space for Hourglass here!", 1, 1);
	}
}

void RemoveHourGlassMenu(CoordinateInBlocks At) {
	if (GetBlock(At + CoordinateInBlocks(1, 0, 0)).CustomBlockID == Rewind_Block) {
		SetBlock(At + CoordinateInBlocks(1, 0, 0), EBlockType::Air);
		SetBlock(At + CoordinateInBlocks(2, 0, 0), EBlockType::Air);
		SetBlock(At + CoordinateInBlocks(3, 0, 0), EBlockType::Air);
		SetBlock(At + CoordinateInBlocks(4, 0, 0), EBlockType::Air);

		SetBlock(At + CoordinateInBlocks(1, 0, 2), EBlockType::Air);
		SetBlock(At + CoordinateInBlocks(2, 0, 2), EBlockType::Air);
		SetBlock(At + CoordinateInBlocks(3, 0, 2), EBlockType::Air);
		SetBlock(At + CoordinateInBlocks(4, 0, 2), EBlockType::Air);
	}
}

void SaveData() {
	std::wstring path = GetThisModFolderPath();
	path = path + GetWorldName() + std::wstring(L".txt");

	std::fstream saveFile(path, std::ios::out);
	if (saveFile.is_open()) {
		if (timeIsPaused) {
			saveFile << "1" << std::endl;
		}
		else {
			saveFile << "0" << std::endl;
		}

		saveFile.close();
	}
}
void LoadData() {
	std::wstring path = GetThisModFolderPath();
	path = path + GetWorldName() + std::wstring(L".txt");

	std::fstream saveFile(path, std::ios::in);
	if (saveFile.is_open()) {
		std::string line;
		std::getline(saveFile, line);
		if (line == "1") {
			timeIsPaused = true;
			pauseTime = GetTimeOfDay();
		}
		else {
			timeIsPaused = false;
		}
		saveFile.close();
	}
}

/************************************************************* 
//	Functions (Run automatically by the game, you can put any code you want into them)
*************************************************************/
void Event_BlockPlaced(CoordinateInBlocks At, UniqueID CustomBlockID, bool Moved)
{}

void Event_BlockDestroyed(CoordinateInBlocks At, UniqueID CustomBlockID, bool Moved)
{
	if (CustomBlockID == Hour_Glass_Block) {
		RemoveHourGlassMenu(At);
	}
}

void Event_BlockHitByTool(CoordinateInBlocks At, UniqueID CustomBlockID, wString ToolName)
{
	if (ToolName == L"T_Stick") {
		if      (CustomBlockID == Hour_Glass_Block) {
			if (GetBlock(At + CoordinateInBlocks(1, 0, 0)).CustomBlockID == Rewind_Block) {
				RemoveHourGlassMenu(At);
			}
			else {
				GenerateHourGlassMenu(At);
			}
		}
		else if (CustomBlockID == Pause_Block) {
			if (timeIsPaused) {
				timeIsPaused = false;
				SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Resuming Daycycle.", 1, 1);
			}
			else {
				pauseTime = GetTimeOfDay();
				timeIsPaused = true;
				SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Pausing Daycycle.", 1, 1);
			}
			SaveData();
		}
		else if (CustomBlockID == Fast_Forward_Block) {
			float currentTime = GetTimeOfDay();
			if (currentTime + 100 > 2400) {
				float newTime = 0 + ((currentTime + 100) - 2400);
				SetTimeOfDay(newTime);
				pauseTime = newTime;
			}
			else {
				float newTime = currentTime + 100;
				SetTimeOfDay(newTime);
				pauseTime = newTime;
			}
			SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Fast Forwarding 1 hour.", 1, 1);
		}
		else if (CustomBlockID == Rewind_Block) {
			float currentTime = GetTimeOfDay();
			if (currentTime - 100 < 0) {
				float newTime = 2400 + (currentTime - 100);
				SetTimeOfDay(newTime);
				pauseTime = newTime;
			}
			else {
				float newTime = currentTime - 100;
				SetTimeOfDay(newTime);
				pauseTime = newTime;
			}
			SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Rewinding 1 hour.", 1, 1);
		}
		else if (CustomBlockID == Watch_Block) {
			SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Current Time is: " + std::to_wstring(GetTimeOfDay()), 1, 1);
		}
		else if (CustomBlockID == Sunrise_Block) {
			SetTimeOfDay(Sunrise);
			pauseTime = Sunrise;
			SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Setting time to 06:00.", 1, 1);
		}
		else if (CustomBlockID == Noon_Block) {
			SetTimeOfDay(Noon);
			pauseTime = Noon;
			SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Setting time to 12:00.", 1, 1);
		}
		else if (CustomBlockID == Sunset_Block) {
			SetTimeOfDay(Sunset);
			pauseTime = Sunset;
			SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Setting time to 18:00.", 1, 1);
		}
		else if (CustomBlockID == Midnight_Block) {
			SetTimeOfDay(Midnight);
			pauseTime = Midnight;
			SpawnHintText(At + CoordinateInBlocks(0, 0, 1), L"Setting time to 24:00.", 1, 1);
		}
	}
}


void Event_Tick()
{
	if (timeIsPaused) {
		SetTimeOfDay(pauseTime);
	}
}

void Event_OnLoad()
{
	LoadData();
}

void Event_OnExit()
{}

/*******************************************************

	Advanced functions

*******************************************************/
void Event_AnyBlockPlaced(CoordinateInBlocks At, BlockInfo Type, bool Moved)
{}

void Event_AnyBlockDestroyed(CoordinateInBlocks At, BlockInfo Type, bool Moved)
{}

void Event_AnyBlockHitByTool(CoordinateInBlocks At, BlockInfo Type, wString ToolName)
{}
/*******************************************************

	For all the available game functions you can call, look at the GameAPI.h file

*******************************************************/