#include "ZHookBase.cpp"
#include "INI.h"
#include "bass.h"
#include <filesystem>

#pragma region Compiler stuff
enum class AudioPlayMode;
enum class SoundType;
void PlayRandomSound(SoundType);
void FadeStreamOut(HSTREAM, AudioPlayMode, int);
void FadeStreamIn(HSTREAM, float, int);
bool ChangeStreamPlayMode(HSTREAM, AudioPlayMode);
HSTREAM LoadAudioFile(const void*, bool, bool);
bool SetStreamVolume(HSTREAM, float);
AudioPlayMode GetStreamPlayMode(HSTREAM);
bool FreeStream(HSTREAM);
int GetBassErrorCode();
bool InRange(float, float, float);
bool InRange(int, int, int);
int GetFileCountInDirectory(std::string);
std::string CombineStringForSoundFilePath(std::string, uint, std::string);
void InitDVLocations();
#pragma endregion

#pragma region Classes

class AVector2 {
public:
	float X, Y;
	AVector2::AVector2() {
		X = 0;
		Y = 0;
	}
	AVector2::AVector2(float x, float y) {
		X = x;
		Y = y;
	}
};

#pragma endregion

#pragma region Variables and Enums
Ped playerPed;
Interior currentInterior;
HSTREAM streamHandle;
AudioPlayMode FadeOutAfterPlayMode;
uint32_t sfxVolume;

bool tempBool1;
bool isHandleCurrentlyFadingOut;
float pX, pY, pZ;

// Settings
int randomNumberGeneratorMaximum = 1000000;
int soundMultiplier = 0;

int alderneyNum1 = 735;
int alderneyNum2 = 750;

int algonquinNum1 = 320;
int algonquinNum2 = 340;

int brokerDukesNum1 = 100;
int brokerDukesNum2 = 115;

int bohanNum1 = 900;
int bohanNum2 = 905;

#pragma region Locations
AVector2 DV_ALDERNEY_POS_1;
AVector2 DV_ALDERNEY_POS_2;

AVector2 DV_ALGONQUIN_POS_1;
AVector2 DV_ALGONQUIN_POS_2;
AVector2 DV_ALGONQUIN_POS_3;
AVector2 DV_ALGONQUIN_POS_4;

AVector2 DV_BOHAN_POS_1;
AVector2 DV_BOHAN_POS_2;

AVector2 DV_BROKER_DUKES_POS_1;
AVector2 DV_BROKER_DUKES_POS_2;
AVector2 DV_BROKER_DUKES_POS_3;
AVector2 DV_BROKER_DUKES_POS_4;
#pragma endregion

#pragma region Enums
enum class AudioPlayMode {
	APM_Play,
	APM_Pause,
	APM_Stop,
	APM_None
};
enum class SoundType {
	DVehicle
};
#pragma endregion

#pragma endregion

#pragma region Audio Methods

void FadeStreamOut(HSTREAM stream, AudioPlayMode after, int fadingSpeed = 1000) {
	if (!isHandleCurrentlyFadingOut) {
		isHandleCurrentlyFadingOut = true;
		BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, 0, fadingSpeed);
		FadeOutAfterPlayMode = after;
	}
}
void FadeStreamIn(HSTREAM stream, float fadeToVolumeLevel, int fadingSpeed = 1000) {
	BASS_ChannelPlay(stream, false);
	BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, fadeToVolumeLevel / 100.0f, fadingSpeed);
}

bool ChangeStreamPlayMode(HSTREAM stream, AudioPlayMode newState) {
	if (stream != 0) {
		switch (newState) {
			case AudioPlayMode::APM_Play:
				return BASS_ChannelPlay(stream, false);
			case AudioPlayMode::APM_Pause:
				return BASS_ChannelPause(stream);
			case AudioPlayMode::APM_Stop:
				return BASS_ChannelStop(stream);
			default:
				return false;
		}
	}
	else {
		return false;
	}
}

HSTREAM LoadAudioFile(const void* file, bool createWithZeroDecibals, bool dontDestroyOnStreamEnd = false) {
	if (createWithZeroDecibals) {
		if (dontDestroyOnStreamEnd) {
			HSTREAM handle = BASS_StreamCreateFile(false, file, 0, 0, BASS_STREAM_PRESCAN);
			SetStreamVolume(handle, 0);
			return handle;
		}
		else {
			HSTREAM handle = BASS_StreamCreateFile(false, file, 0, 0, BASS_STREAM_AUTOFREE);
			SetStreamVolume(handle, 0);
			return handle;
		}
	}
	else {
		if (dontDestroyOnStreamEnd) {
			HSTREAM handle = BASS_StreamCreateFile(false, file, 0, 0, BASS_STREAM_PRESCAN);
			SetStreamVolume(handle, (float)sfxVolume);
			return handle;
		}
		else {
			HSTREAM handle = BASS_StreamCreateFile(false, file, 0, 0, BASS_STREAM_AUTOFREE);
			SetStreamVolume(handle, (float)sfxVolume);
			return handle;
		}
	}
}

bool SetStreamVolume(HSTREAM stream, float volume) {
	if (stream != 0) {
		return BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume / 100.0f);
	}
	else {
		return false;
	}
}

AudioPlayMode GetStreamPlayMode(HSTREAM stream) {
	if (stream != 0) {
		switch (BASS_ChannelIsActive(stream)) {
			case BASS_ACTIVE_PLAYING:
				return AudioPlayMode::APM_Play;
			case BASS_ACTIVE_PAUSED:
				return AudioPlayMode::APM_Pause;
			case BASS_ACTIVE_STOPPED:
				return AudioPlayMode::APM_Stop;
			default:
				return AudioPlayMode::APM_None;
		}
	}
	else {
		return AudioPlayMode::APM_None;
	}
}

bool FreeStream(HSTREAM stream) {
	if (stream != 0) {
		BASS_ChannelStop(stream);
		return BASS_StreamFree(stream);
	}
	else {
		return false;
	}
}

int GetBassErrorCode() {
	return BASS_ErrorGetCode();
}

#pragma endregion

#pragma region Methods

void PlayRandomSound(SoundType type) {
	std::string path;
	uint rndNumber;
	if (GetStreamPlayMode(streamHandle) != AudioPlayMode::APM_Play) {
		switch (type) {
			case SoundType::DVehicle:
				GENERATE_RANDOM_INT_IN_RANGE(1, 11, &rndNumber); // Get random number for vehicle type
				path = ".\\VAmbiance\\vehicles\\";
				path += std::to_string(rndNumber);
				int filesInDir = GetFileCountInDirectory(path);
				GENERATE_RANDOM_INT_IN_RANGE(1, filesInDir, &rndNumber); // Get random number for sound file

				streamHandle = LoadAudioFile(CombineStringForSoundFilePath(path, rndNumber, ".wav").c_str(), false); // Load audio file
				ChangeStreamPlayMode(streamHandle, AudioPlayMode::APM_Play); // Play audio file
				break;
		}
	}
}

void InitDVLocations() {
	DV_ALDERNEY_POS_1 = AVector2(-726.9269f, 1566.952f);
	DV_ALDERNEY_POS_2 = AVector2(-2234.198f, -140.5777f);

	DV_ALGONQUIN_POS_1 = AVector2(-654.886f, 1771.657f);
	DV_ALGONQUIN_POS_2 = AVector2(503.8584f, -571.4658f);
	DV_ALGONQUIN_POS_3 = AVector2(503.8584f, -571.4658f);
	DV_ALGONQUIN_POS_4 = AVector2(-383.3239f, -837.8165f);

	DV_BOHAN_POS_1 = AVector2(293.9349f, 1986.573f);
	DV_BOHAN_POS_2 = AVector2(1508.645f, 1310.419f);

	DV_BROKER_DUKES_POS_1 = AVector2(427.072f, 1110.072f);
	DV_BROKER_DUKES_POS_2 = AVector2(2078.201f, 22.85286f);
	DV_BROKER_DUKES_POS_3 = AVector2(1554.846f, 18.28593f);
	DV_BROKER_DUKES_POS_4 = AVector2(697.1231f, -581.8057f);
}

#pragma endregion

#pragma region Functions

bool InRange(float min, float max, float targetValue) {
	return (targetValue < max) && (targetValue > min);
}
bool InRange(int min, int max, int targetValue) {
	return ((targetValue-min) <= (max-min));
}

int GetFileCountInDirectory(std::string path) {
	auto dirIter = std::filesystem::directory_iterator(path);
	return std::count_if(begin(dirIter), end(dirIter), [](auto& entry) { return entry.is_regular_file(); });
}

std::string CombineStringForSoundFilePath(std::string p1, uint num1, std::string extension) {
	std::string fullPath = p1;
	fullPath += "\\";
	fullPath += std::to_string(num1);
	fullPath += extension;
	return fullPath;
}

#pragma endregion

void scriptLoad() {
	BASS_Init(-1, 44100, 0, 0, NULL);
	InitDVLocations(); // Init Map Locations For Distant Vehicles

	// Load settings
	INI<> ini("VAmbience.ini", true);
	if (ini.select("General")) {
		randomNumberGeneratorMaximum = std::stoi(ini.get("RandomNumberGeneratorMaximum", "1000000"));
	}
	if (ini.select("Sound")) {
		soundMultiplier = std::stoi(ini.get("Multiplier", "0"));
	}
	if (ini.select("VehicleRanges")) {
		alderneyNum1 = std::stoi(ini.get("AlderneyNum1", "735"));
		alderneyNum2 = std::stoi(ini.get("AlderneyNum2", "750"));
		algonquinNum1 = std::stoi(ini.get("AlgonquinNum1", "320"));
		algonquinNum2 = std::stoi(ini.get("AlgonquinNum2", "340"));
		brokerDukesNum1 = std::stoi(ini.get("BrokerDukesNum1", "100"));
		brokerDukesNum2 = std::stoi(ini.get("BrokerDukesNum2", "115"));
		bohanNum1 = std::stoi(ini.get("BohanNum1", "900"));
		bohanNum2 = std::stoi(ini.get("BohanNum2", "905"));
	}
}
void scriptUnload() {
	BASS_Free();
}

// Runs every frame
void scriptTick() {
	playerPed = GET_PLAYER_PED();
	GET_CHAR_COORDINATES(playerPed, &pX, &pY, &pZ);
	GET_INTERIOR_FROM_CHAR(playerPed, &currentInterior);
	
	// Get SFX Volume
	uint32_t rawSFXVolume = *(uint32_t*)(baseAddress + ADDRESS_SETTINGS + (SETTING_SFX_LEVEL));
	if (!(rawSFXVolume < 8)) {
		sfxVolume = (rawSFXVolume + soundMultiplier) - 4;
	}
	else {
		if (rawSFXVolume == 0) {
			sfxVolume = 0;
		}
		else {
			sfxVolume = rawSFXVolume + soundMultiplier;
		}
	}

	// FadeStreamOut Handler
	if (isHandleCurrentlyFadingOut) {
		if (GetStreamPlayMode(streamHandle) == AudioPlayMode::APM_Play) {
			float handleVolume;
			BASS_ChannelGetAttribute(streamHandle, BASS_ATTRIB_VOL, &handleVolume);
			if (handleVolume <= 0) {
				switch (FadeOutAfterPlayMode) {
					case AudioPlayMode::APM_Stop:
						BASS_ChannelStop(streamHandle);
						streamHandle = 0;
						isHandleCurrentlyFadingOut = false;
						break;
					case AudioPlayMode::APM_Pause:
						BASS_ChannelPause(streamHandle);
						isHandleCurrentlyFadingOut = false;
					break;
				}
			}
		}
	}
	
	uint rndNumber = 0;
	if (IS_PAUSE_MENU_ACTIVE()) {
		switch (GetStreamPlayMode(streamHandle)) {
			case AudioPlayMode::APM_Play:
				FadeStreamOut(streamHandle, AudioPlayMode::APM_Pause, 500);
				break;
		}
	}
	else {
		switch (GetStreamPlayMode(streamHandle)) {
			case AudioPlayMode::APM_Pause:
				FadeStreamIn(streamHandle, (float)sfxVolume, 500);
				break;
		}

		if (GetStreamPlayMode(streamHandle) == AudioPlayMode::APM_Stop || GetStreamPlayMode(streamHandle) == AudioPlayMode::APM_None) {
			GENERATE_RANDOM_INT_IN_RANGE(1, randomNumberGeneratorMaximum, &rndNumber);

			// Distance Vehicle Sound
			if (InRange(0.000000, 150.0000, pZ)) { // If char is in height range
				if (currentInterior == 0) { // Char is outside
					if (IS_CHAR_IN_AREA_2D(playerPed, DV_ALDERNEY_POS_1.X, DV_ALDERNEY_POS_1.Y, DV_ALDERNEY_POS_2.X, DV_ALDERNEY_POS_2.Y, false)) { // Alderney
						if (InRange(alderneyNum1, alderneyNum2, rndNumber)) {
							PlayRandomSound(SoundType::DVehicle);
						}
					}
					else if (IS_CHAR_IN_AREA_2D(playerPed, DV_ALGONQUIN_POS_1.X, DV_ALGONQUIN_POS_1.Y, DV_ALGONQUIN_POS_2.X, DV_ALGONQUIN_POS_2.Y, false) || // Algonquin
						IS_CHAR_IN_AREA_2D(playerPed, DV_ALGONQUIN_POS_3.X, DV_ALGONQUIN_POS_3.Y, DV_ALGONQUIN_POS_4.X, DV_ALGONQUIN_POS_4.Y, false)) {
						if (InRange(algonquinNum1, algonquinNum2, rndNumber)) {
							PlayRandomSound(SoundType::DVehicle);
						}
					}
					else if (IS_CHAR_IN_AREA_2D(playerPed, DV_BOHAN_POS_1.X, DV_BOHAN_POS_1.Y, DV_BOHAN_POS_2.X, DV_BOHAN_POS_2.Y, false)) { // Bohan
						if (InRange(bohanNum1, bohanNum2, rndNumber)) {
							PlayRandomSound(SoundType::DVehicle);
						}
					}
					else if (IS_CHAR_IN_AREA_2D(playerPed, DV_BROKER_DUKES_POS_1.X, DV_BROKER_DUKES_POS_1.Y, DV_BROKER_DUKES_POS_2.X, DV_BROKER_DUKES_POS_2.Y, false) || // Broker/Dukes
						IS_CHAR_IN_AREA_2D(playerPed, DV_BROKER_DUKES_POS_3.X, DV_BROKER_DUKES_POS_3.Y, DV_BROKER_DUKES_POS_4.X, DV_BROKER_DUKES_POS_4.Y, false)) {
						if (InRange(brokerDukesNum1, brokerDukesNum2, rndNumber)) {
							PlayRandomSound(SoundType::DVehicle);
						}
					}
				}
			}
		}
	}

	// - DEBUG -
	//std::string sRaw1 = "Error code: ";
	//sRaw1 += std::to_string(GetBassErrorCode());
	//sRaw1 += " - Volume: ";
	//sRaw1 += std::to_string(sfxVolume);
	//sRaw1 += " - RND Number: ";
	//sRaw1 += std::to_string(rndNumber);
	//sRaw1 += " - Current District: ";
	//sRaw1 += currentDistrict;
	//sRaw1 += " - bohan2: ";
	//sRaw1 += std::to_string(bohanNum2);
	//PRINT_STRING_WITH_LITERAL_STRING_NOW("STRING", sRaw1.c_str(), 1000, true);
}