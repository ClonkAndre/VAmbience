// VAmbience 1.0 by ItsClonkAndre

#include "ZHookBase.cpp"
#include "INI.h"
#include "bass.h"
#include <filesystem>

#pragma region Enums, Variables and Maps
// Enums
enum class AudioPlayMode
{
    APM_Unknown,
    APM_Play,
    APM_Pause,
    APM_Stop
};
enum class Zones
{
    Alderney,
    HappinessIsland,
    Algonquin,
    ChargeIsland,
    ColonyIsland,
    Bohan,
    BrokerDukes,
    LibertyCity
};

// Variables
Ped playerPed;
SDKVector3 playerPosition, playerVelocity;
Zones currentZone;
bool playInCutscenes;

#pragma region Sound
float gameSFXVolume;
int globalSoundMultiplier;
bool enable3D;
HSTREAM audioStreams[2];
#pragma endregion

#pragma region DistantVehicles
bool dv_Enabled;
int dv_GlobalMulitplier;
int dv_SoundMultiplier;
int dv_SoundDistance;
int dv_AlderneyPercentage;
int dv_AlgonquinPercentage;
int dv_BohanPercentage;
int dv_BrokerAndDukesPercentage;
int dv_ChargeIslandPercentage;
int dv_ColonyIslandPercentage;
int dv_HappinessIslandPercentage;
#pragma endregion

#pragma region DistantGunSounds
bool dgs_Enabled;
int dgs_GlobalMulitplier;
int dgs_SoundMultiplier;
int dgs_SoundDistance;
int dgs_AlderneyPercentage;
int dgs_AlgonquinPercentage;
int dgs_BohanPercentage;
int dgs_BrokerAndDukesPercentage;
int dgs_ChargeIslandPercentage;
int dgs_ColonyIslandPercentage;
int dgs_HappinessIslandPercentage;

bool dgs_threadRunning, dgs_abortThread;
int dgs_selectedGun = -1;
int dgs_shootsFired = -1, dgs_maxShootsToFire = -1;
SDKVector3 dgs_distanceVector = SDKVector3::Empty();
#pragma endregion

#pragma region Secrets
bool se_Enabled;
int se_SoundMultiplier;
#pragma endregion

#pragma region Aiden Stuff
SDKVector3 aidenPosition;
Ped aidenPed;
#pragma endregion

// Maps
std::map<std::string, Zones> GTAZoneNamesLocations{
    {"WESDY", Zones::Alderney},
    {"LEFWO", Zones::Alderney},
    {"ALDCI", Zones::Alderney},
    {"BERCH", Zones::Alderney},
    {"NORMY", Zones::Alderney},
    {"ACTRR", Zones::Alderney},
    {"PORTU", Zones::Alderney},
    {"TUDOR", Zones::Alderney},
    {"ACTIP", Zones::Alderney},
    {"ALSCF", Zones::Alderney},
    {"HAPIN", Zones::HappinessIsland},
    {"CASGR", Zones::Algonquin},
    {"THXCH", Zones::Algonquin},
    {"FISSO", Zones::Algonquin},
    {"CHITO", Zones::Algonquin},
    {"CITH", Zones::Algonquin},
    {"CASGC", Zones::Algonquin},
    {"SUFFO", Zones::Algonquin},
    {"LITAL", Zones::Algonquin},
    {"LOWEA", Zones::Algonquin},
    {"FISSN", Zones::Algonquin},
    {"THPRES", Zones::Algonquin},
    {"EASON", Zones::Algonquin},
    {"THTRI", Zones::Algonquin},
    {"TMEQU", Zones::Algonquin},
    {"WESMI", Zones::Algonquin},
    {"STARJ", Zones::Algonquin},
    {"LANCE", Zones::Algonquin},
    {"HATGA", Zones::Algonquin},
    {"PUGAT", Zones::Algonquin},
    {"MIDPW", Zones::Algonquin},
    {"MIDPA", Zones::Algonquin},
    {"MIDPE", Zones::Algonquin},
    {"LANCA", Zones::Algonquin},
    {"VASIH", Zones::Algonquin},
    {"NOHOL", Zones::Algonquin},
    {"EAHOL", Zones::Algonquin},
    {"NORWO", Zones::Algonquin},
    {"CHISL", Zones::ChargeIsland},
    {"COISL", Zones::ColonyIsland},
    {"STHBO", Zones::Bohan},
    {"CHAPO", Zones::Bohan},
    {"FORSI", Zones::Bohan},
    {"BOULE", Zones::Bohan},
    {"NRTGA", Zones::Bohan},
    {"INSTI", Zones::Bohan},
    {"LTBAY", Zones::Bohan},
    {"STEIN", Zones::BrokerDukes},
    {"MEADP", Zones::BrokerDukes},
    {"FRANI", Zones::BrokerDukes},
    {"WILLI", Zones::BrokerDukes},
    {"MEADH", Zones::BrokerDukes},
    {"EISLC", Zones::BrokerDukes},
    {"BOAB", Zones::BrokerDukes},
    {"CERHE", Zones::BrokerDukes},
    {"BEECW", Zones::BrokerDukes},
    {"SCHOL", Zones::BrokerDukes},
    {"DOWTW", Zones::BrokerDukes},
    {"ROTTH", Zones::BrokerDukes},
    {"ESHOO", Zones::BrokerDukes},
    {"OUTL", Zones::BrokerDukes},
    {"SUTHS", Zones::BrokerDukes},
    {"HOBEH", Zones::BrokerDukes},
    {"FIREP", Zones::BrokerDukes},
    {"FIISL", Zones::BrokerDukes},
    {"BEGGA", Zones::BrokerDukes},
    {"BRALG", Zones::LibertyCity},
    {"BRBRO", Zones::LibertyCity},
    {"BREBB", Zones::LibertyCity},
    {"BRDBB", Zones::LibertyCity},
    {"NOWOB", Zones::LibertyCity},
    {"HIBRG", Zones::LibertyCity},
    {"LEAPE", Zones::LibertyCity},
    {"BOTU", Zones::LibertyCity},
    {"LIBERTY", Zones::LibertyCity}
};
#pragma endregion

#pragma region Functions
inline int GetFileCount(std::string path)
{
    auto dirIter = std::filesystem::directory_iterator(path);
    return std::count_if(begin(dirIter), end(dirIter), [](auto& entry) { return entry.is_regular_file(); });
}
inline int GetDirectoryCount(std::string path)
{
    auto dirIter = std::filesystem::directory_iterator(path);
    return std::count_if(begin(dirIter), end(dirIter), [](auto& entry) { return entry.is_directory(); });
}
inline uint GetRandomIntInRange(uint min, uint max)
{
    uint value;
    GENERATE_RANDOM_INT_IN_RANGE(min, max, &value);
    return value;
}

inline bool InRange(uint from, uint to, uint targetValue)
{
    if (from <= to) {
        return (targetValue >= from && targetValue <= (to - 1));
    }
    else {
        return (targetValue >= from || targetValue <= (to - 1));
    }
}

inline float GetRandomFloat()
{
    float value = 0;
    GENERATE_RANDOM_FLOAT(&value);
    return value;
}
inline float GetRandomFloatInRange(float min, float max)
{
    float value = 0;
    GENERATE_RANDOM_FLOAT_IN_RANGE(min, max, &value);
    return value;
}
inline float Get3DDistance(SDKVector3 pos, SDKVector3 pos2)
{
    SDKVector3 vector3_2 = pos2 - pos;
    float x2 = vector3_2.x;
    float y2 = vector3_2.y;
    float z2 = vector3_2.z;
    float num1 = y2;
    float num2 = num1 * num1;
    float num3 = x2;
    float num4 = num3 * num3;
    float num5 = num2 + num4;
    float num6 = z2;
    float num7 = num6 * num6;
    return Sqrt(num5 + num7);
}

inline SDKVector3 GetPositionAroundPosition(SDKVector3 position, float distance)
{
    SDKVector3 rndVec{};
    rndVec.x = GetRandomFloat() * distance;
    rndVec.y = GetRandomFloat() * distance;
    rndVec.z = 0;

    SDKVector3 newVec{};
    newVec.x = position.x;
    newVec.y = position.y;
    newVec.z = position.z;

    return newVec + rndVec;
}
inline SDKVector3 GetNextPositionOnStreet(SDKVector3 pos)
{
    uint areaID = GET_MAP_AREA_FROM_COORDS(pos.x, pos.y, pos.z);

    float mX, mY, mZ, mHeading;
    uint unk;
    GET_NTH_CLOSEST_CAR_NODE_WITH_HEADING_ON_ISLAND(pos.x, pos.y, pos.z, 1, areaID, &mX, &mY, &mZ, &mHeading, &unk);

    SDKVector3 nextPos{};
    nextPos.x = mX;
    nextPos.y = mY;
    nextPos.z = mZ;
    return nextPos;
}
inline SDKVector3 HeadingToDirection(float heading)
{
    float h = heading * (M_PI / 180);
    SDKVector3 dir{};
    dir.x = -SIN(h);
    dir.y = COS(h);
    dir.z = 0;
    return dir;
}
inline SDKVector3 RotationToDirection(float rotX, float rotY, float rotZ)
{
    float num1 = rotZ * (M_PI / 180);
    float num2 = rotX * (M_PI / 180);
    float num3 = ABSF(COS(num2));
    SDKVector3 dir{};
    dir.x = -SIN(num1) * num3;
    dir.y = COS(num1) * num3;
    dir.z = SIN(num2);
    return dir;
}
#pragma endregion

#pragma region BASS
inline bool SetStreamVolume(HSTREAM stream, float volume) {
    if (stream != 0) return BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume / 100.0f);
    return false;
}
inline HSTREAM LoadAudioFile(const void* file, bool createWith3D = false) {
    HSTREAM handle = 0;

    if (createWith3D) {
        handle = BASS_StreamCreateFile(false, file, 0, 0, BASS_STREAM_PRESCAN | BASS_STREAM_AUTOFREE | BASS_SAMPLE_MONO | BASS_SAMPLE_3D);
    }
    else {
        handle = BASS_StreamCreateFile(false, file, 0, 0, BASS_STREAM_PRESCAN | BASS_STREAM_AUTOFREE);
    }

    return handle;
}

inline bool SetListener3DPosition(SDKVector3 pos, SDKVector3 vel, SDKVector3 front) {
    BASS_3DVECTOR* bassPosVec = new BASS_3DVECTOR(pos.x, pos.y, pos.z);
    BASS_3DVECTOR* bassVelVec = new BASS_3DVECTOR(vel.x, vel.y, vel.z);
    BASS_3DVECTOR* bassDirFrontVec = new BASS_3DVECTOR(front.x, front.y, front.z);
    BASS_3DVECTOR* bassDirTopVec = new BASS_3DVECTOR(0, 1, 0);
    return BASS_Set3DPosition(bassPosVec, bassVelVec, bassDirFrontVec, bassDirTopVec);
}
inline bool SetStream3DPosition(HSTREAM stream, SDKVector3 soundPos) {
    bool result = false;
    if (stream != 0) {
        BASS_3DVECTOR* bassSoundPosVec = new BASS_3DVECTOR(soundPos.x, soundPos.y, soundPos.z);
        result = BASS_ChannelSet3DPosition(stream, bassSoundPosVec, NULL, NULL);
        BASS_Apply3D();
    }
    return result;
}

inline bool ChangeStreamPlayMode(HSTREAM stream, AudioPlayMode newState, BOOL restart = false) {
    if (stream != 0) {
        switch (newState) {
            case AudioPlayMode::APM_Play:
                return BASS_ChannelPlay(stream, restart);
            case AudioPlayMode::APM_Pause:
                return BASS_ChannelPause(stream);
            case AudioPlayMode::APM_Stop:
                return BASS_ChannelStop(stream);
            default:
                return false;
        }
    }
    return false;
}
inline AudioPlayMode GetStreamPlayMode(HSTREAM stream)
{
    if (stream != 0) {
        switch (BASS_ChannelIsActive(stream)) {
            case BASS_ACTIVE_PLAYING:
                return AudioPlayMode::APM_Play;
            case BASS_ACTIVE_PAUSED:
                return AudioPlayMode::APM_Pause;
            case BASS_ACTIVE_STOPPED:
                return AudioPlayMode::APM_Stop;
            default:
                return AudioPlayMode::APM_Unknown;
        }
    }
    return AudioPlayMode::APM_Unknown;
}

inline bool FreeStream(HSTREAM stream)
{
    if (stream != 0) {
        BASS_ChannelStop(stream);
        return BASS_StreamFree(stream);
    }
    return false;
}

inline int GetBassErrorCode()
{
    return BASS_ErrorGetCode();
}

// Fading
inline bool FadeStreamOut(HSTREAM stream, AudioPlayMode after, int fadingSpeed = 1000)
{
    if (stream == 0)
        return false;

    if (!BASS_ChannelIsSliding(stream, BASS_ATTRIB_VOL)) {
        BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, 0, fadingSpeed);
        return true;
    }

    return false;
}
inline bool FadeStreamIn(HSTREAM stream, float fadeToVolumeLevel, int fadingSpeed = 1000)
{
    if (stream == 0)
        return false;

    if (!BASS_ChannelIsSliding(stream, BASS_ATTRIB_VOL)) {
        BASS_ChannelPlay(stream, false);
        BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, fadeToVolumeLevel / 100.0f, fadingSpeed);
        return true;
    }

    return false;
}
#pragma endregion

#pragma region Methods
inline HSTREAM PlayRandomAidenSound(SDKVector3 pos, int fileID = -1)
{
    uint rndNumber;
    std::string path = ".\\VAmbiance\\secret\\aiden";

    // Get random number for sound file
    GENERATE_RANDOM_INT_IN_RANGE(1, GetFileCount(path), &rndNumber);
    path += "\\";
    path += fileID == -1 ? std::to_string(rndNumber) : std::to_string(fileID);
    path += ".mp3";

    HSTREAM audioStream = LoadAudioFile(path.c_str(), enable3D); // Load audio file
    SetStreamVolume(audioStream, gameSFXVolume * se_SoundMultiplier); // Sets the volume of the sound
    if (enable3D) SetStream3DPosition(audioStream, pos); // Set 3D position of sound
    ChangeStreamPlayMode(audioStream, AudioPlayMode::APM_Play); // Play audio file
    return audioStream;
}
inline HSTREAM PlayRandomVehicleSound(SDKVector3 pos, int dirGroup = -1, int fileID = -1)
{
    uint rndNumber;
    std::string path = ".\\VAmbiance\\vehicle";

    // Get random number for directory group
    GENERATE_RANDOM_INT_IN_RANGE(1, GetDirectoryCount(path), &rndNumber);
    path += "\\";
    path += dirGroup == -1 ? std::to_string(rndNumber) : std::to_string(dirGroup);

    // Get random number for sound file in directory group
    GENERATE_RANDOM_INT_IN_RANGE(1, GetFileCount(path), &rndNumber);
    path += "\\";
    path += fileID == -1 ? std::to_string(rndNumber) : std::to_string(fileID);
    path += ".mp3";

    HSTREAM audioStream = LoadAudioFile(path.c_str(), enable3D); // Load audio file
    SetStreamVolume(audioStream, gameSFXVolume * dv_SoundMultiplier); // Sets the volume of the sound
    if (enable3D) SetStream3DPosition(audioStream, pos); // Set 3D position of sound
    ChangeStreamPlayMode(audioStream, AudioPlayMode::APM_Play); // Play audio file
    return audioStream;
}
inline HSTREAM PlayRandomGunSound(SDKVector3 pos, int dirGroup = -1)
{
    uint rndNumber;
    std::string path = ".\\VAmbiance\\gun";

    // Get random number for directory group
    GENERATE_RANDOM_INT_IN_RANGE(1, GetDirectoryCount(path), &rndNumber);
    path += "\\";
    path += dirGroup == -1 ? std::to_string(rndNumber) : std::to_string(dirGroup);

    // Get random number for sound file in directory group
    GENERATE_RANDOM_INT_IN_RANGE(1, GetFileCount(path), &rndNumber);
    path += "\\";
    path += std::to_string(rndNumber);
    path += ".mp3";

    HSTREAM audioStream = LoadAudioFile(path.c_str(), enable3D); // Load audio file
    SetStreamVolume(audioStream, gameSFXVolume * dgs_SoundMultiplier); // Sets the volume of the sound
    if (enable3D) SetStream3DPosition(audioStream, pos); // Set 3D position of sound
    ChangeStreamPlayMode(audioStream, AudioPlayMode::APM_Play); // Play audio file
    return audioStream;
}

inline void LoadModel(Hash model)
{
    if (!HAS_MODEL_LOADED(model)) {
        REQUEST_MODEL(model);
        LOAD_ALL_OBJECTS_NOW();
    }
}
inline void UnloadModel(Hash model)
{
    if (HAS_MODEL_LOADED(model)) MARK_MODEL_AS_NO_LONGER_NEEDED(model);
}
inline void SetUpPositions()
{
    // Aiden
    aidenPosition.x = -1561.919;
    aidenPosition.y = 1220.616;
    aidenPosition.z = 11.463;
}
#pragma endregion

#pragma region Threads
inline DWORD WINAPI GunShootingSoundsThread(void* args)
{
    do {
        if (dgs_abortThread) break;

        if (dgs_selectedGun == -1) { // Selects a random gun
            int gCount = GetDirectoryCount(".\\VAmbiance\\gun");
            dgs_selectedGun = GetRandomIntInRange(1, gCount);
        }
        if (dgs_distanceVector == SDKVector3::Empty()) dgs_distanceVector = GetPositionAroundPosition(playerPosition, dgs_SoundDistance); // Sets a random position around player
        if (dgs_maxShootsToFire == -1) dgs_maxShootsToFire = GetRandomIntInRange(3, 7); // Sets how many shoots will be fired

        PlayRandomGunSound(dgs_distanceVector, dgs_selectedGun);

        dgs_shootsFired++;
        Sleep(GetRandomIntInRange(290, 450));
    } while (!(dgs_shootsFired >= dgs_maxShootsToFire));

    // Reset
    dgs_selectedGun = -1;
    dgs_shootsFired = -1;
    dgs_maxShootsToFire = -1;
    dgs_distanceVector = SDKVector3::Empty();
    dgs_threadRunning = false;

    ExitThread(0);
}
#pragma endregion

void scriptLoad() {
    // Init Stuff
    BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
    SetUpPositions();

    // Load settings
    INI<> ini("VAmbience.ini", true);
    if (ini.select("Sound")) {
        globalSoundMultiplier = std::stoi(ini.get("GlobalMultiplier", "1"));
        enable3D = static_cast<bool>(std::stoi(ini.get("Enable3D", "1")));
        playInCutscenes = static_cast<bool>(std::stoi(ini.get("PlayInCutscenes", "1")));
    }
    if (ini.select("DistantVehicles")) {
        dv_Enabled = static_cast<bool>(std::stoi(ini.get("Enabled", "1")));
        dv_GlobalMulitplier = std::stoi(ini.get("GlobalMuliplier", "100"));
        dv_SoundMultiplier = std::stoi(ini.get("SoundMultiplier", "2"));
        dv_SoundDistance = std::stoi(ini.get("SoundDistance", "120"));
        dv_AlderneyPercentage = std::stoi(ini.get("Alderney", "4"));
        dv_AlgonquinPercentage = std::stoi(ini.get("Algonquin", "5"));
        dv_BohanPercentage = std::stoi(ini.get("Bohan", "2"));
        dv_BrokerAndDukesPercentage = std::stoi(ini.get("BrokerAndDukes", "2"));
        dv_ChargeIslandPercentage = std::stoi(ini.get("ChargeIsland", "1"));
        dv_ColonyIslandPercentage = std::stoi(ini.get("ColonyIsland", "1"));
        dv_HappinessIslandPercentage = std::stoi(ini.get("HappinessIsland", "0"));
    }
    if (ini.select("DistantGunSounds")) {
        dgs_Enabled = static_cast<bool>(std::stoi(ini.get("Enabled", "1")));
        dgs_GlobalMulitplier = std::stoi(ini.get("GlobalMulitplier", "100"));
        dgs_SoundMultiplier = std::stoi(ini.get("SoundMultiplier", "2"));
        dgs_SoundDistance = std::stoi(ini.get("SoundDistance", "300"));
        dgs_AlderneyPercentage = std::stoi(ini.get("Alderney", "4"));
        dgs_AlgonquinPercentage = std::stoi(ini.get("Algonquin", "1"));
        dgs_BohanPercentage = std::stoi(ini.get("Bohan", "3"));
        dgs_BrokerAndDukesPercentage = std::stoi(ini.get("BrokerAndDukes", "4"));
        dgs_ChargeIslandPercentage = std::stoi(ini.get("ChargeIsland", "0"));
        dgs_ColonyIslandPercentage = std::stoi(ini.get("ColonyIsland", "0"));
        dgs_HappinessIslandPercentage = std::stoi(ini.get("HappinessIsland", "0"));
    }
    if (ini.select("Secrets")) {
        se_Enabled = static_cast<bool>(std::stoi(ini.get("Enabled", "1")));
        se_SoundMultiplier = std::stoi(ini.get("SoundMultiplier", "2"));
    }
}
void scriptUnload() {
    BASS_Free();
}

void scriptTick()
{

#pragma region GetStuff
    Player player = CONVERT_INT_TO_PLAYERINDEX(GET_PLAYER_ID());
    GET_PLAYER_CHAR(player, &playerPed);

    // Get char coordinates
    float pX, pY, pZ;
    GET_CHAR_COORDINATES(playerPed, &pX, &pY, &pZ);
    playerPosition.x = pX;
    playerPosition.y = pY;
    playerPosition.z = pZ;

    // Get char velocity
    float vX, vY, vZ;
    GET_CHAR_VELOCITY(playerPed, &vX, &vY, &vZ);
    playerVelocity.x = vX;
    playerVelocity.y = vY;
    playerVelocity.z = vZ;

    // Get root camera and current position/rotation of it
    Cam mainCam;
    GET_ROOT_CAM(&mainCam);
   

    float camPosX, camPosY, camPosZ;
    float camRotX, camRotY, camRotZ;
    GET_CAM_POS(mainCam, &camPosX, &camPosY, &camPosZ);
    GET_CAM_ROT(mainCam, &camRotX, &camRotY, &camRotZ);

    SDKVector3 camPosition;
    camPosition.x = camPosX;
    camPosition.y = camPosY;
    camPosition.z = camPosZ;
    SDKVector3 camRotation;
    camRotation.x = camRotX;
    camRotation.y = camRotY;
    camRotation.z = camRotZ;

    // Get current zone the player is in
    char* rawZoneName = GET_NAME_OF_ZONE(pX, pY, pZ);
    currentZone = GTAZoneNamesLocations.find(rawZoneName)->second;

    // Get game SFX volume
    gameSFXVolume = *(uint32_t*)(baseAddress + ADDRESS_SETTINGS + (SETTING_SFX_LEVEL)) * globalSoundMultiplier;
#pragma endregion

    // Set 3D stuff and apply changes
    SetListener3DPosition(camPosition, playerVelocity, camRotation);
    BASS_Set3DFactors(1.0, 0.1, 1.0);
    BASS_Apply3D();

    // Fade active streams out and pauses them if pause menu is active
    if (IS_PAUSE_MENU_ACTIVE()) {
        dgs_abortThread = true;
        for (HSTREAM s : audioStreams) {
            if (s == 0) continue;
            if (GetStreamPlayMode(s) == AudioPlayMode::APM_Play) FadeStreamOut(s, AudioPlayMode::APM_Pause, 250);
        }
        return;
    }
    else {
        dgs_abortThread = false;
        for (HSTREAM s : audioStreams) {
            if (s == 0) continue;
            if (GetStreamPlayMode(s) == AudioPlayMode::APM_Pause) FadeStreamIn(s, gameSFXVolume, 250);
        }
    }
    if (IS_INTERIOR_SCENE()) return;

    if (!HAS_CUTSCENE_FINISHED()) {
        if (!playInCutscenes) return;
    }

    // VAmbience Stuff
    float randomNum;

#pragma region Secrets
    if (se_Enabled) {

#pragma region Aiden
        if (InRange(1, 3, GET_HOURS_OF_DAY())) {
            if (GetFloatStat(Scripting::eFloatStatistic::STAT_DERRICK_MISSION_PROGRESS) >= 100) {
                float dist = Get3DDistance(playerPosition, aidenPosition);

                // Set Aiden's Alpha
                if (DOES_CHAR_EXIST(aidenPed)) SET_PED_ALPHA(aidenPed, 125);

                // Spawn/Despawn Aiden
                if (dist <= 450) { // Spawn Aiden
                    if (aidenPed == NULL) {
                        LoadModel(215190023);
                        CREATE_CHAR(22, 215190023, aidenPosition.x, aidenPosition.y, aidenPosition.z, &aidenPed, true);
                        if (DOES_CHAR_EXIST(aidenPed)) {
                            SET_CHAR_AS_MISSION_CHAR(aidenPed);
                            BLOCK_CHAR_AMBIENT_ANIMS(aidenPed, true);
                            BLOCK_CHAR_GESTURE_ANIMS(aidenPed, true);
                            BLOCK_CHAR_VISEME_ANIMS(aidenPed, true);
                            BLOCK_COWERING_IN_COVER(aidenPed, true);
                            BLOCK_CHAR_HEAD_IK(aidenPed, true);
                            SET_CHAR_MOVEMENT_ANIMS_BLOCKED(aidenPed, true);
                            SET_CHAR_HEADING(aidenPed, 273.157);
                            SET_CHAR_PROOFS(aidenPed, true, true, true, true, true);
                            SET_CHAR_COLLISION(aidenPed, false);
                        }
                        UnloadModel(215190023);
                    }
                }
                else { // Despawn Aiden
                    if (DOES_CHAR_EXIST(aidenPed)) DELETE_CHAR(&aidenPed);
                }
                if (dist <= 80) { // Despawn Aiden
                    if (DOES_CHAR_EXIST(aidenPed)) DELETE_CHAR(&aidenPed);
                }

                // Play Sound
                if (dist <= 85) {
                    randomNum = GetRandomFloatInRange(0, 101) * 90;
                    if (randomNum < 5) {
                        if (GetStreamPlayMode(audioStreams[1]) != AudioPlayMode::APM_Play) {
                            audioStreams[1] = PlayRandomAidenSound(aidenPosition);
                        }
                    }
                }

            }
        }
        else {
            if (DOES_CHAR_EXIST(aidenPed)) DELETE_CHAR(&aidenPed);
        }
#pragma endregion

    }
#pragma endregion

#pragma region DistantVehicles
    if (dv_Enabled) {
        switch (currentZone) {
            case Zones::Alderney:
                randomNum = GetRandomFloatInRange(0, 101) * dv_GlobalMulitplier;
                if (randomNum <= dv_AlderneyPercentage) {
                    if (GetStreamPlayMode(audioStreams[0]) != AudioPlayMode::APM_Play) {
                        audioStreams[0] = PlayRandomVehicleSound(GetNextPositionOnStreet(GetPositionAroundPosition(playerPosition, dv_SoundDistance)));
                    }
                }
                break;
            case Zones::Algonquin:
                randomNum = GetRandomFloatInRange(0, 101) * dv_GlobalMulitplier;
                if (randomNum <= dv_AlgonquinPercentage) {
                    if (GetStreamPlayMode(audioStreams[0]) != AudioPlayMode::APM_Play) {
                        audioStreams[0] = PlayRandomVehicleSound(GetNextPositionOnStreet(GetPositionAroundPosition(playerPosition, dv_SoundDistance)));
                    }
                }
                break;
            case Zones::Bohan:
                randomNum = GetRandomFloatInRange(0, 101) * dv_GlobalMulitplier;
                if (randomNum <= dv_BohanPercentage) {
                    if (GetStreamPlayMode(audioStreams[0]) != AudioPlayMode::APM_Play) {
                        audioStreams[0] = PlayRandomVehicleSound(GetNextPositionOnStreet(GetPositionAroundPosition(playerPosition, dv_SoundDistance)));
                    }
                }
                break;
            case Zones::BrokerDukes:
                randomNum = GetRandomFloatInRange(0, 101) * dv_GlobalMulitplier;
                if (randomNum <= dv_BrokerAndDukesPercentage) {
                    if (GetStreamPlayMode(audioStreams[0]) != AudioPlayMode::APM_Play) {
                        audioStreams[0] = PlayRandomVehicleSound(GetNextPositionOnStreet(GetPositionAroundPosition(playerPosition, dv_SoundDistance)));
                    }
                }
                break;
            case Zones::ChargeIsland:
                randomNum = GetRandomFloatInRange(0, 101) * dv_GlobalMulitplier;
                if (randomNum <= dv_ChargeIslandPercentage) {
                    if (GetStreamPlayMode(audioStreams[0]) != AudioPlayMode::APM_Play) {
                        audioStreams[0] = PlayRandomVehicleSound(GetNextPositionOnStreet(GetPositionAroundPosition(playerPosition, dv_SoundDistance)));
                    }
                }
                break;
            case Zones::ColonyIsland:
                randomNum = GetRandomFloatInRange(0, 101) * dv_GlobalMulitplier;
                if (randomNum <= dv_ColonyIslandPercentage) {
                    if (GetStreamPlayMode(audioStreams[0]) != AudioPlayMode::APM_Play) {
                        audioStreams[0] = PlayRandomVehicleSound(GetNextPositionOnStreet(GetPositionAroundPosition(playerPosition, dv_SoundDistance)));
                    }
                }
                break;
            case Zones::HappinessIsland:
                randomNum = GetRandomFloatInRange(0, 101) * dv_GlobalMulitplier;
                if (randomNum <= dv_HappinessIslandPercentage) {
                    if (GetStreamPlayMode(audioStreams[0]) != AudioPlayMode::APM_Play) {
                        audioStreams[0] = PlayRandomVehicleSound(GetNextPositionOnStreet(GetPositionAroundPosition(playerPosition, dv_SoundDistance)));
                    }
                }
                break;
        }
    }
#pragma endregion

#pragma region DistantGun
    if (dgs_Enabled && !dgs_threadRunning) {
        switch (currentZone) {
            case Zones::Alderney:
                randomNum = GetRandomFloatInRange(0, 101) * dgs_GlobalMulitplier;
                if (randomNum <= dgs_AlderneyPercentage) {
                    if (CreateThread(NULL, NULL, GunShootingSoundsThread, NULL, NULL, NULL) != NULL) {
                        dgs_threadRunning = true;
                    }
                }
                break;
            case Zones::HappinessIsland:
                randomNum = GetRandomFloatInRange(0, 101) * dgs_GlobalMulitplier;
                if (randomNum <= dgs_HappinessIslandPercentage) {
                    if (CreateThread(NULL, NULL, GunShootingSoundsThread, NULL, NULL, NULL) != NULL) {
                        dgs_threadRunning = true;
                    }
                }
                break;
            case Zones::Algonquin:
                randomNum = GetRandomFloatInRange(0, 101) * dgs_GlobalMulitplier;
                if (randomNum <= dgs_AlgonquinPercentage) {
                    if (CreateThread(NULL, NULL, GunShootingSoundsThread, NULL, NULL, NULL) != NULL) {
                        dgs_threadRunning = true;
                    }
                }
                break;
            case Zones::ChargeIsland:
                randomNum = GetRandomFloatInRange(0, 101) * dgs_GlobalMulitplier;
                if (randomNum <= dgs_ChargeIslandPercentage) {

                }
                break;
            case Zones::ColonyIsland:
                randomNum = GetRandomFloatInRange(0, 101) * dgs_GlobalMulitplier;
                if (randomNum <= dgs_ColonyIslandPercentage) {
                    if (CreateThread(NULL, NULL, GunShootingSoundsThread, NULL, NULL, NULL) != NULL) {
                        dgs_threadRunning = true;
                    }
                }
                break;
            case Zones::Bohan:
                randomNum = GetRandomFloatInRange(0, 101) * dgs_GlobalMulitplier;
                if (randomNum <= dgs_BohanPercentage) {
                    if (CreateThread(NULL, NULL, GunShootingSoundsThread, NULL, NULL, NULL) != NULL) {
                        dgs_threadRunning = true;
                    }
                }
                break;
            case Zones::BrokerDukes:
                randomNum = GetRandomFloatInRange(0, 101) * dgs_GlobalMulitplier;
                if (randomNum <= dgs_BrokerAndDukesPercentage) {
                    if (CreateThread(NULL, NULL, GunShootingSoundsThread, NULL, NULL, NULL) != NULL) {
                        dgs_threadRunning = true;
                    }
                }
                break;
        }
    }
#pragma endregion

}