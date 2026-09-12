#import "vinhtran.hpp"
#import "loading.hxx"
#include <fstream>
#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include <math.h>
#import "../JRMemory/JRMemory.framework/Headers/MemScan.h"

bool SilentAim = false;
bool CheckWall1 = false;
Vector3 SavedMapMarker = {0, 0, 0};
static bool campc_active = false;
static float cam_fov_value = 0.4f; // Giá trị mặc định là 0.4
static std::vector<void*> camPCAddrs;
static bool isCamPCScanned = false;
bool AutoFire = false;
bool CHECKPLAYER = false;
static Vector3 g_TargetPos = {0, 0, 0};
bool spin360 = false;
static float currentY = 0.0f;
static float SpinSpeed = 5000.0f;
static float RevealDistance = 7.0f;


#include <map>

struct EnemyCacheData {
    Vector3 bestRevealPos;
    bool hasBestPos;
};

// Sử dụng static để tránh lỗi trùng lặp khi link dylib
static std::map<void *, EnemyCacheData> enemyCache;

enum FireMode { 
    MANUL,
    AUTO
}; 

enum FireStatus { 
    NONE,
    FIRING,
    CANCEL
};

struct Vars_t
{
    bool fastfire = false;
    bool xoaspeed = false;
    bool noreload = false;
    bool vohandan = false;
    bool Enable = {};
    bool AimbotEnable = {};
    bool Aimbot = {};
    bool ShowFovCircle = false;
   
    float AimFov = 360.0f;
    int AimCheck = {};
    bool ESPCount = {};
    int AimType = {};
    int AimWhen = 3;
    int AimMode = 2;
    bool isAimFov = {};
    int AimHitbox = 0; 
    
    const char* aimHitboxes[3] = {"Head", "Neck", "Body"};
    const char *dir[4] = {"None", "Fire", "Scope", "Fire Or Scope"};
    const char *aimModes[3] = {"Aim 360°", "Aim 180°", "AimFov"};

    const char* aimHitboxesVN[3] = {"Dau", "Co", "Than"};
    const char *dirVN[4] = {"Khong Co", "Ban", "Ngam", "Ban Hoac Ngam"};
    const char *aimModesVN[3] = {"Aim 360°", "Aim 180°", "AimFov"};
    
    bool VisibleCheck = true;
    bool lines = {};
    bool Box = {};
    bool Outline = {};
    bool Name = {};
    bool Health = {};
    bool Distance = {};
    bool fovaimglow = {};
    bool circlepos = {};
    bool skeleton = {};
    bool OOF = {};
    bool enemycount = {};
    float fovLineColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    ImVec4 boxColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float AimSpeed = 10.0f;
    bool IgnoreKnocked = true;
    bool AiPlayer2 = false;
} Vars;

struct HitObjectInfo {
    void *klass;
    void *monitor;
    bool m_IsInPool;
    void *HitObject;
    void *HitCollider;
    Vector3 HitLocation;
    Vector3 HitNormal;
    Vector3 RayDir;
    Vector3 StartPosition;
    int32_t Damage;
    float Distance;
    int32_t ActorLayer;
    int32_t HitGroup;
    void *HitPhysicMaterial;
    bool IgnoreHappens;
    bool ViewBlocked;
    struct Vector3 OrigStartPosition;
    uint8_t SpecialHitType;
    uint32_t SpecialHitLevelObjID;
};

class game_sdk_t
{
public:
    void init();
    int (*GetHp)(void *player);
    void *(*Curent_Match)();
    void *(*GetLocalPlayer)(void *Game);
    void *(*GetHeadPositions)(void *player);
    Vector3 (*get_position)(void *player);
    void *(*Component_GetTransform)(void *player);
    void *(*get_camera)();
    Vector3 (*WorldToViewpoint)(void*, Vector3, int);
    bool (*get_isVisible)(void *player);
    bool (*get_isLocalTeam)(void *player);
    bool (*get_IsDieing)(void *player);
    int (*get_MaxHP)(void *player);
    Vector3 (*GetForward)(void *player);
    void (*set_aim)(void *, Quaternion look);
    bool (*get_IsSighting)(void *player);
    bool (*get_IsFiring)(void *player);
    monoString *(*name)(void *player);
    void *(*_GetHeadPositions)(void *);
    void *(*_newHipMods)(void *);
    void *(*_GetLeftAnkleTF)(void *);
    void *(*_GetRightAnkleTF)(void *);
    void *(*_GetLeftToeTF)(void *);
    void *(*_GetRightToeTF)(void *);
    void *(*_getLeftHandTF)(void *);
    void *(*_getRightHandTF)(void *);
    void *(*_getLeftForeArmTF)(void *);
    void *(*_getRightForeArmTF)(void *);
};

game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init()
{
    this->GetHp = (int (*)(void *))getRealOffset(oxo("0x543592C"));
    this->Curent_Match = (void *(*)())getRealOffset(oxo("0x55C4DA4"));
    this->GetLocalPlayer = (void *(*)(void *))getRealOffset(oxo("0x2FFE494"));
    this->GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x54547E0"));
    this->get_position = (Vector3(*)(void *))getRealOffset(oxo("0x91CA56C"));
    this->Component_GetTransform = (void *(*)(void *))getRealOffset(oxo("0x91B82E4"));
    this->get_camera = (void *(*)())getRealOffset(oxo("0x915E9E4"));
    this->WorldToViewpoint = (Vector3(*)(void*, Vector3, int))getRealOffset(oxo("0x915E364"));
    this->get_isVisible = (bool (*)(void *))getRealOffset(oxo("0x53C8894"));
    this->get_isLocalTeam = (bool (*)(void *))getRealOffset(oxo("0x53E20C4"));
    this->get_IsDieing = (bool (*)(void *))getRealOffset(oxo("0x53AA18C"));
    this->get_MaxHP = (int (*)(void *))getRealOffset(oxo("0x5435A3C"));
    this->GetForward = (Vector3(*)(void *))getRealOffset(oxo("0x91CAF64"));
    this->set_aim = (void (*)(void *, Quaternion))getRealOffset(oxo("0x53C4534"));
    this->get_IsSighting = (bool (*)(void *))getRealOffset(oxo("0x53B769C"));
    this->get_IsFiring = (bool (*)(void *))getRealOffset(oxo("0x53ACC9C"));
    this->name = (monoString * (*)(void *player)) getRealOffset(oxo("0x53BE8E0"));
    this->_GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x54547E0"));
    this->_newHipMods = (void *(*)(void *))getRealOffset(oxo("0x5454990"));
    this->_GetLeftAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x5454DE0"));
    this->_GetRightAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x5454EEC"));
    this->_GetLeftToeTF = (void *(*)(void *))getRealOffset(oxo("0x5454FF8"));
    this->_GetRightToeTF = (void *(*)(void *))getRealOffset(oxo("0x5455104"));
    this->_getLeftHandTF = (void *(*)(void *))getRealOffset(oxo("0x53C3608"));
    this->_getRightHandTF = (void *(*)(void *))getRealOffset(oxo("0x53C370C"));
    this->_getLeftForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x53C3810"));
    this->_getRightForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x53C3914"));
}

bool IsGod(void *player){
    return *(bool *)((uint64_t) player + 0xF94);
}

void *get_gameObject(void *Pthis)
{
    return ((void* (*)(void *))getRealOffset(0x91B8334))(Pthis);
}

static void *GetWeaponOnHand1(void *local) {
    void *(*_GetWeaponOnHand1)(void *local) = (void *(*)(void *))getRealOffset(0x53BE110);
    return _GetWeaponOnHand1(local);
}

static Vector3 Transform_INTERNAL_GetPosition(void *transform) {
    if (!transform) return Vector3::zero();
    Vector3 out = Vector3::zero();
    void (*_Transform_INTERNAL_GetPosition)(void *transform, Vector3 * out) = (void (*)(void *, Vector3 *))getRealOffset(ENCRYPTOFFSET("0x91CA5D0"));
    _Transform_INTERNAL_GetPosition(transform, &out);
    return out;
}

static void Transform_INTERNAL_SetPosition(void *player, Vvector3 inn) {
   void (*_Transform_INTERNAL_SetPosition)(void *transform, Vvector3 in) = (void (*)(void *, Vvector3))getRealOffset(oxo("0x91CA6A8"));
    _Transform_INTERNAL_SetPosition(player, inn);
}

static void Transform_INTERNAL_SetRotation(void *transform, Quaternion quat) {
    if (!transform) return;
    void (*_Transform_INTERNAL_SetRotation)(void *transform, Quaternion in) = (void (*)(void *, Quaternion))getRealOffset(0x91CAA0C);
    _Transform_INTERNAL_SetRotation(transform, quat);
}

// Hàm Xoay xoay
void RotatePlayer360(void* player, float delta) {
    if (!spin360 || !player) return;
    void* transform = game_sdk->Component_GetTransform(player);
    if (!transform) return;

    currentY = fmod(currentY + (SpinSpeed * delta), 360.0f);
    float rad = currentY * (M_PI / 180.0f);
    Quaternion rot;
    rot.x = 0.0f; rot.y = sin(rad / 0.2f); rot.z = 0.0f; rot.w = cos(rad / 0.2f);
    
    Transform_INTERNAL_SetRotation(transform, rot);
}

std::map<void*, float> enemyOriginalY;

namespace Camera$$WorldToScreen
{
    ImVec2 Regular(Vector3 pos) {
        auto cam = game_sdk->get_camera();
        if (!cam) return {0,0};

        Vector3 worldPoint = game_sdk->WorldToViewpoint(cam,pos, 2);
        Vector3 location;

        int ScreenWidth = ImGui::GetIO().DisplaySize.x;
        int ScreenHeight = ImGui::GetIO().DisplaySize.y;

        location.x = ScreenWidth * worldPoint.x;
        location.y = ScreenHeight - worldPoint.y * ScreenHeight;
        location.z  = worldPoint.z;

        return {location.x, location.y};
    }

    ImVec2 Checker(Vector3 pos, bool &checker) {
        auto cam = game_sdk->get_camera();
        if (!cam) return {0, 0};
       
        Vector3 worldPoint = game_sdk->WorldToViewpoint(cam,pos, 4);
        Vector3 location;
     
        int ScreenWidth = ImGui::GetIO().DisplaySize.x;
        int ScreenHeight = ImGui::GetIO().DisplaySize.y;
     
        location.x = ScreenWidth * worldPoint.x;
        location.y = ScreenHeight - worldPoint.y * ScreenHeight;
        location.z = worldPoint.z;
     
        checker = location.z > 1;
     
        return {location.x, location.y};
    }
}

Vector3 GetBonePosition(void *player, void *(*transformGetter)(void *)) {
    if (!player || !transformGetter) return Vector3::zero();
    void *transform = transformGetter(player);
    if (!transform) return Vector3::zero();
    void *compTrans = game_sdk->Component_GetTransform(transform);
    if (!compTrans) return Vector3::zero();
    return game_sdk->get_position(compTrans);
}

Vector3 GetHitboxPosition(void* player, int hitbox) {
    if (!player) return Vector3::zero();
    
    switch (hitbox) {
        case 0: return GetBonePosition(player, game_sdk->GetHeadPositions);
        case 1: {
            Vector3 headPos = GetBonePosition(player, game_sdk->GetHeadPositions);
            return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.2f, headPos.z);
        }
        case 2: {
            Vector3 headPos = GetBonePosition(player, game_sdk->GetHeadPositions);
            return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.4f, headPos.z);
        }
        default: return GetBonePosition(player, game_sdk->GetHeadPositions);
    }
}

Vector3 getPosition(void *player) {
    if (!player) return Vector3::zero();
    void *compTrans = game_sdk->Component_GetTransform(player);
    if (!compTrans) return Vector3::zero();
    return game_sdk->get_position(compTrans);
}

Vector3 GetHeadPosition(void *player) {
    if (!player) return Vector3::zero();
    void *headPos = game_sdk->GetHeadPositions(player);
    if (!headPos) return Vector3::zero();
    return game_sdk->get_position(headPos);
}

static Vector3 CameraMain(void *player) {
    if (!player) return Vector3::zero();
    void* camTransform = *(void **)((uint64_t)player + oxo("0x390"));
    if (!camTransform) return Vector3::zero();
    return game_sdk->get_position(camTransform);
}

Quaternion GetRotationToTheLocation(Vector3 Target, float Height, Vector3 MyEnemy) {
    Vector3 direction = (Target + Vector3(0, Height, 0)) - MyEnemy;
    return Quaternion::LookRotation(direction, Vector3(0, 1, 0));
}

Quaternion GetCurrentRotation(void* player) {
    if (!player) return Quaternion();
    void* transform = game_sdk->Component_GetTransform(player);
    if (!transform) return Quaternion();
    return Quaternion::LookRotation(game_sdk->GetForward(transform), Vector3(0, 1, 0));
}

#include "Helper/Ext.h"

class tanghinh {
public:
    static Vector3 Transform_GetPosition(void *transform) {
        if (!transform) return Vector3::zero();
        Vector3 out = Vector3::zero();
        void (*_Transform_GetPosition)(void *transform, Vector3 *out) = (void (*)(void *, Vector3 *))getRealOffset(oxo("0x91CA5D0"));
        _Transform_GetPosition(transform, &out);
        return out;
    }

    static void *Player_GetHeadCollider(void *player)
    {
        void *(*_Player_GetHeadCollider)(void *players) = (void *(*)(void *))getRealOffset(oxo("0x53C2630"));
        return _Player_GetHeadCollider(player);
    }

    static bool Physics_Raycast(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider)
    {
        bool (*_Physics_Raycast)(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider) = (bool (*)(Vector3, Vector3, unsigned int, void *))getRealOffset(oxo("0x57ED5AC"));
        return _Physics_Raycast(camLocation, headLocation, LayerID, collider);
    }

    static bool isVisible(void *enemy) {
        if (!enemy) return false;
        
        void* cam = game_sdk->get_camera();
        if (!cam) return false;
        void* camTrans = game_sdk->Component_GetTransform(cam);
        if (!camTrans) return false;

        void* headCol = Player_GetHeadCollider(enemy);
        if (!headCol) return false;
        void* targetTrans = game_sdk->Component_GetTransform(headCol);
        if (!targetTrans) return false;

        void *hitObj = NULL;
        auto Camera = Transform_GetPosition(camTrans);
        auto Target = Transform_GetPosition(targetTrans);
        return !Physics_Raycast(Camera, Target, 12, &hitObj);
    }
};

void DrawSkeleton(void *player, ImDrawList *drawList)
{
    if (!player || !drawList) return;
    
    bool isPlayerVisible = tanghinh::isVisible(player);
    Vector3 headPos = GetBonePosition(player, game_sdk->_GetHeadPositions);
    Vector3 hipPos = GetBonePosition(player, game_sdk->_newHipMods);
    Vector3 leftAnklePos = GetBonePosition(player, game_sdk->_GetLeftAnkleTF);
    Vector3 rightAnklePos = GetBonePosition(player, game_sdk->_GetRightAnkleTF);
    Vector3 leftToePos = GetBonePosition(player, game_sdk->_GetLeftToeTF);
    Vector3 rightToePos = GetBonePosition(player, game_sdk->_GetRightToeTF);
    Vector3 leftHandPos = GetBonePosition(player, game_sdk->_getLeftHandTF);
    Vector3 rightHandPos = GetBonePosition(player, game_sdk->_getRightHandTF);
    Vector3 leftForeArmPos = GetBonePosition(player, game_sdk->_getLeftForeArmTF);
    Vector3 rightForeArmPos = GetBonePosition(player, game_sdk->_getRightForeArmTF);

    bool visible = false;
    ImVec2 headScreen = Camera$$WorldToScreen::Checker(headPos, visible);
    if (!visible) return;

    ImVec2 hipScreen = Camera$$WorldToScreen::Regular(hipPos);
    ImVec2 leftAnkleScreen = Camera$$WorldToScreen::Regular(leftAnklePos);
    ImVec2 rightAnkleScreen = Camera$$WorldToScreen::Regular(rightAnklePos);
    ImVec2 leftToeScreen = Camera$$WorldToScreen::Regular(leftToePos);
    ImVec2 rightToeScreen = Camera$$WorldToScreen::Regular(rightToePos);
    ImVec2 leftHandScreen = Camera$$WorldToScreen::Regular(leftHandPos);
    ImVec2 rightHandScreen = Camera$$WorldToScreen::Regular(rightHandPos);
    ImVec2 leftForeArmScreen = Camera$$WorldToScreen::Regular(leftForeArmPos);
    ImVec2 rightForeArmScreen = Camera$$WorldToScreen::Regular(rightForeArmPos);
    
    ImColor boneColor = isPlayerVisible ? ImColor(0, 255, 0) : ImColor(255, 255, 255);
    float thickness = 1.0f;

    drawList->AddCircle(headScreen, 2.0f, boneColor, 12, thickness);
    drawList->AddLine(headScreen, hipScreen, boneColor, thickness);
    drawList->AddLine(headScreen, leftForeArmScreen, boneColor, thickness);
    drawList->AddLine(headScreen, rightForeArmScreen, boneColor, thickness);
    drawList->AddLine(leftForeArmScreen, leftHandScreen, boneColor, thickness);
    drawList->AddLine(rightForeArmScreen, rightHandScreen, boneColor, thickness);
    drawList->AddLine(hipScreen, leftAnkleScreen, boneColor, thickness);
    drawList->AddLine(hipScreen, rightAnkleScreen, boneColor, thickness);
    drawList->AddLine(leftAnkleScreen, leftToeScreen, boneColor, thickness);
    drawList->AddLine(rightAnkleScreen, rightToeScreen, boneColor, thickness);
}

bool isFov(Vector3 vec1, Vector3 vec2, int radius)
{
    int x = vec1.x;
    int y = vec1.y;
    int x0 = vec2.x;
    int y0 = vec2.y;
    if ((pow(x - x0, 2) + pow(y - y0, 2)) <= pow(radius, 2))
    {
        return true;
    }
    return false;
}



JRMemoryEngine* engine = nullptr;

void editAll(uint64_t newValue) {
    if (!engine) return;

    vector<void*> results = engine->getAllResults();

    for (int i = 0; i < results.size(); i++) {
        engine->JRWriteMemory((uint64_t)results[i], &newValue, JR_Search_Type_ULong);
    }
}


void searchSpeed() {
    if (!engine) return;

    // clear result cũ
    if (engine->result) {
        ResultDeallocate(engine->result);
        engine->result = ResultAllocate();
    }

    uint64_t value = 4397530849764387586ULL;

    AddrRange range;
    range.start = 0x100000000;
    range.end   = 0x200000000;

    engine->JRScanMemory(range, &value, JR_Search_Type_ULong);

}

void setSpeed() {
    uint64_t value = 0;
    value = 4397530849735000000ULL;

    editAll(value);
}

void disableSpeed() {
    uint64_t value = 4397530849764387586ULL;
    editAll(value);
}



// void setCamPC(JRMemoryEngine *engine, bool enabled, float fov_val) {
//     if (!engine) return;
//     if (!isCamPCScanned) {
//         AddrRange range = {0x100000000, 0x130000000};
//         float searchVal = 0.4f;
//         engine->JRScanMemory(range, &searchVal, sizeof(float));
//         camPCAddrs = engine->getAllResults();
//         isCamPCScanned = true;
//     }
//     float valToWrite = enabled ? fov_val : 0.4f;
//     for (void* addr : camPCAddrs) {
//         engine->JRWriteMemory((uint64_t)addr, &valToWrite, sizeof(float));
//     }
// }



void *GetClosestEnemy()
{
    try
    {
        float shortestDistance = 9999.0f;
        void *closestEnemy = NULL;
        
        void *get_MatchGame = game_sdk->Curent_Match();
        if (!get_MatchGame) return NULL;
            
        void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
        if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer)) return NULL;
            
        if (!Vars.Enable) return NULL;

        if (Vars.fastfire) {
        setSpeed();
        void* PlayerAttributes = *(void**)((uint64_t)LocalPlayer + 0x708);
        *(float*)((uintptr_t)PlayerAttributes + 0x260) = 0.2f;
    } 
    if (Vars.xoaspeed) {
            disableSpeed();
            void* PlayerAttributes = *(void**)((uint64_t)LocalPlayer + 0x708);
            *(float*)((uintptr_t)PlayerAttributes + 0x260) = 1.2f;
    }

        if(Vars.noreload){
            void* PlayerAttributes = *(void**)((uint64_t)LocalPlayer + 0x708); //protected PlayerAttributes JKPFFNEMJIF; 0x680
            *(bool *)((long)PlayerAttributes + 0xD9) = true; //public bool ShootNoReload; // 0xC1 -> class PlayerAttributes
        }else{
            void* PlayerAttributes = *(void**)((uint64_t)LocalPlayer + 0x708); //protected PlayerAttributes JKPFFNEMJIF; 0x680
            *(bool *)((long)PlayerAttributes + 0xD9) = false; //public bool ShootNoReload; // 0xC1 -> class PlayerAttributes
        }


        if(Vars.vohandan){
            void* WeaponHand = GetWeaponOnHand1(LocalPlayer);
            *(bool *)((long)WeaponHand + 0x7B1) = false; //protected bool m_CostAmmo; // 0x6D0-> bất tử đạn , weapon
                //protected bool OKKJNPGJJOH; // 0x741
            }else{
            void* WeaponHand = GetWeaponOnHand1(LocalPlayer);
            *(bool *)((long)WeaponHand + 0x7B1) = true; //protected bool m_CostAmmo; // 0x6D0-> bất tử đạn , weapon

        }
            
        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)get_MatchGame + oxo("0x148"));
        if (!players ) return NULL;

        void* camera = game_sdk->get_camera();
        if (!camera) return NULL;
        void* camTransform = game_sdk->Component_GetTransform(camera);
        if (!camTransform) return NULL;
            
        for (int u = 0; u < players->getSize(); u++)
        {
            void *Player = players->getValues()[u];
            if (!Player) continue;
            if (Player == LocalPlayer) continue;
            if (!game_sdk->get_MaxHP(Player)) continue;
            if (game_sdk->get_IsDieing(Player)) continue;
            // if (!game_sdk->get_isVisible(Player)) continue;
            if (game_sdk->get_isLocalTeam(Player)) continue;
            
            Vector3 PlayerPos = getPosition(Player);
            Vector3 LocalPlayerPos = getPosition(LocalPlayer);
            ImVec2 screenPos = Camera$$WorldToScreen::Regular(PlayerPos);
            bool isFov1 = isFov(Vector3(screenPos.x, screenPos.y), Vector3(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), Vars.AimFov);
            float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
            
            if (distance < 200)
            {
                Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
                float angle = Vector3::Angle(targetDir, game_sdk->GetForward(camTransform)) * 100.0f;
                if (angle <= Vars.AimFov && isFov1 && angle < shortestDistance)
                {
                    if (tanghinh::isVisible(Player))
                    {
                        shortestDistance = angle;
                        closestEnemy = Player;
                    }
                }
            }
        }
        return closestEnemy;
    }
    catch (...)
    {
        return NULL;
    }
}

void *GetClosestEnemysilent()
{
    try
    {
        float shortestDistance = 99999.0f;
        void *closestEnemy = NULL;

        void *get_MatchGame = game_sdk->Curent_Match();
        if (!get_MatchGame) return NULL;

        void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
        if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer)) return NULL;

        if (!Vars.Enable) return NULL;

        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long) get_MatchGame + 0x148);
        if (!players ) return NULL;

        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        ImVec2 center(screenSize.x / 2, screenSize.y / 2);

        for (int i = 0; i < players->getSize(); i++) {
            void *Player = players->getValues()[i];

            if (!Player || Player == LocalPlayer) continue;
            if (!game_sdk->get_MaxHP(Player)) continue;
            if (game_sdk->get_IsDieing(Player)) continue;
            if (game_sdk->get_isLocalTeam(Player)) continue;
            if (IsGod(Player)) continue;

            int hp = game_sdk->GetHp(Player);
            if (Vars.IgnoreKnocked && hp <= 0) continue;

            bool isInsideCamera = false;
            Vector3 pos = getPosition(Player);
            ImVec2 screenPos = Camera$$WorldToScreen::Checker(pos, isInsideCamera);

            if (!isInsideCamera) continue;
            if (screenPos.x < 0 || screenPos.x > screenSize.x || screenPos.y < 0 || screenPos.y > screenSize.y) continue;

            if (CheckWall1)
            {
             // if (!game_sdk->get_isVisible(Player)) continue;
                if (!tanghinh::isVisible(Player)) continue;
            }

            float dx = screenPos.x - center.x;
            float dy = screenPos.y - center.y;
            float screenDist = sqrtf(dx * dx + dy * dy);

            if (screenDist < shortestDistance)
            {
                shortestDistance = screenDist;
                closestEnemy = Player;
            }
        }

        return closestEnemy;
    }
    catch (...)
    {
        return NULL;
    }
}

int SetDamage = 1;

void *getItransform(void *itransform) {
    void * (*_itransformNode)(void *_this) = (void*(*)(void*))getRealOffset(0x1850760);
    return _itransformNode(itransform);
}

static float get_Range(void *pthis)
{
    return ((float (*)(void *))getRealOffset(ENCRYPTOFFSET("0x5884080")))(pthis);
}

bool isEnemyInRangeWeapon(void *player, void *enemy, void* weapon)
{
    if (player != nullptr && enemy != nullptr && weapon != nullptr)
    {
        Vector3 EnemyHeadPosition = GetHeadPosition(enemy);
        Vector3 PlayerHeadPosition = GetHeadPosition(player);
        float distance = Vector3::Distance(PlayerHeadPosition, EnemyHeadPosition);
        float range = get_Range(weapon);

        if (distance <= range) {
            return true;
        }
    }
    return false;
}

Vector3 GetHipPosition(void* player) {
    if(!player) return Vector3::zero();
    void *HipITF= *(void **)((uint64_t) player + 0x648);
    if(!HipITF) return Vector3::zero();
    void *HipTF = getItransform(HipITF);
    Vector3 Hip = Transform_INTERNAL_GetPosition(HipTF);
    return Hip;
}

int (*old_BLAGCMCGEJG1)(void *, HitObjectInfo *);
int BLAGCMCGEJG1(void *ist, HitObjectInfo *HitObject) {
    if (SilentAim && HitObject) {
        void *match = game_sdk->Curent_Match();
        if (match) {
            void *localPlayer = game_sdk->GetLocalPlayer(match);
            if (localPlayer) {
                void *weapon = GetWeaponOnHand1(localPlayer);
                void *enemy = GetClosestEnemysilent();
                if (enemy && weapon) {
                    if (isEnemyInRangeWeapon(localPlayer, enemy, weapon)) {
                        Vector3 enemyPos;
                        if (SetDamage == 1)
                            enemyPos = GetHeadPosition(enemy);
                        else
                            enemyPos = GetHipPosition(enemy);
                        
                        Vector3 startPos = GetHeadPosition(localPlayer);
                        void* headCollider = tanghinh::Player_GetHeadCollider(enemy);
                        
                        if (headCollider) {
                            HitObject->HitObject = get_gameObject(headCollider);
                            HitObject->HitCollider = headCollider;
                            HitObject->HitLocation = enemyPos;
                            HitObject->HitNormal = enemyPos;
                            HitObject->RayDir = Vector3::Normalized(enemyPos - startPos);
                            HitObject->StartPosition = startPos;
                            HitObject->OrigStartPosition = startPos;
                            HitObject->HitGroup = 1; 
                            HitObject->SpecialHitType = 0;
                            HitObject->IgnoreHappens = false;
                            HitObject->ViewBlocked = false;
                        }
                    }
                }
            }
        }
    }
    return old_BLAGCMCGEJG1(ist, HitObject);
}

void ProcessAimbot() {
    if (!Vars.Aimbot) return;
    
    void *CurrentMatch = game_sdk->Curent_Match();

    if (!CurrentMatch) return;
    
    void *LocalPlayer = game_sdk->GetLocalPlayer(CurrentMatch);

    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer)) return;
    
    void *closestEnemy = GetClosestEnemy();

    if (!closestEnemy || !game_sdk->Component_GetTransform(closestEnemy)) return;

    Vector3 EnemyLocation = GetHitboxPosition(closestEnemy, Vars.AimHitbox);

    if (EnemyLocation == Vector3::zero()) return;
    
    Vector3 PlayerLocation = CameraMain(LocalPlayer);

    if (PlayerLocation == Vector3::zero()) return;

    bool IsScopeOn = game_sdk->get_IsSighting(LocalPlayer);
    bool IsFiring = game_sdk->get_IsFiring(LocalPlayer);
    bool shouldAim =
        (Vars.AimWhen == 0) ||                        
        (Vars.AimWhen == 1 && IsFiring) ||             
        (Vars.AimWhen == 2 && IsScopeOn) ||           
        (Vars.AimWhen == 3 && (IsFiring || IsScopeOn)); 

    if (shouldAim && (!Vars.VisibleCheck || tanghinh::isVisible(closestEnemy))) {
        if (game_sdk->get_IsDieing(closestEnemy) && Vars.IgnoreKnocked) {
            float shortestDistance = 9999.0f;
            void *newTarget = NULL;
            Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)CurrentMatch + oxo("0x148"));
            
            if (players) {
                for (int u = 0; u < players->getSize(); u++) {
                    void *Player = players->getValues()[u];
                    if (!Player || Player == LocalPlayer || !game_sdk->get_MaxHP(Player) || game_sdk->get_isLocalTeam(Player) || Player == closestEnemy)
                        continue;

                    if (Vars.IgnoreKnocked && game_sdk->get_IsDieing(Player)) continue;
                    if (Vars.VisibleCheck && !tanghinh::isVisible(Player)) continue;

                    Vector3 PlayerPos = GetHitboxPosition(Player, Vars.AimHitbox);
                    float distance = Vector3::Distance(PlayerLocation, PlayerPos);
                    if (distance < 300 && distance < shortestDistance) {
                        shortestDistance = distance;
                        newTarget = Player;
                    }
                }
            }

            if (newTarget) {
                EnemyLocation = GetHitboxPosition(newTarget, Vars.AimHitbox);
                closestEnemy = newTarget;
            } else {
                return;
            }
        }

        Quaternion TargetLook = GetRotationToTheLocation(EnemyLocation, 0.05f, PlayerLocation);
        game_sdk->set_aim(LocalPlayer, TargetLook);
    }
}

void UpdateCHECKPLAYER()
{
    CHECKPLAYER = false;

    void* match = game_sdk->Curent_Match();
    if (!match) return;

    void* local = game_sdk->GetLocalPlayer(match);
    if (!local) return;

    void* cam = game_sdk->get_camera();
    if (!cam) return;

    Vector3 localPos = getPosition(local);
    Vector3 localForward =
        game_sdk->GetForward(game_sdk->Component_GetTransform(cam));

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 screenCenter(screenSize.x / 2.f, screenSize.y / 2.f);

    Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)match + 0x148);
if (!players )
return;

    for (int i = 0; i < players->getSize(); i++) {
void *enemy = players->getValues()[i];
        if (!enemy || enemy == local) continue;

        if (!game_sdk->Component_GetTransform(enemy)) continue;
        if (!game_sdk->get_MaxHP(enemy)) continue;
        if (game_sdk->get_IsDieing(enemy)) continue;
        if (game_sdk->get_isLocalTeam(enemy)) continue;
        if (IsGod(enemy)) continue;

        int hp = game_sdk->GetHp(enemy);
        if (Vars.IgnoreKnocked && hp <= 0) continue;

        if (!game_sdk->get_isVisible(enemy)) continue;
        if (!tanghinh::isVisible(enemy)) continue;

        Vector3 enemyPos = getPosition(enemy);
        if (Vector3::Distance(localPos, enemyPos) > 200.f) continue;

        bool isInsideCamera = false;
        ImVec2 screenPos =
            Camera$$WorldToScreen::Checker(enemyPos, isInsideCamera);

        if (!isInsideCamera) continue;

        if (screenPos.x < 0 || screenPos.x > screenSize.x ||
            screenPos.y < 0 || screenPos.y > screenSize.y)
            continue;

        if (!isFov(
            Vector3(screenPos.x, screenPos.y, 0),
            Vector3(screenCenter.x, screenCenter.y, 0),
            Vars.AimFov))
            continue;

        Vector3 targetDir = Vector3::Normalized(enemyPos - localPos);
        float angle = Vector3::Angle(targetDir, localForward);
        if (angle > Vars.AimFov * 0.5f)
            continue;

        CHECKPLAYER = true;
        return;
    }
}

void get_players()
{
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list) return;
    if (!Vars.Enable) return;

    // UpdateCHECKPLAYER();
    // if (Vars.flyaltura)
    //     FlyHack22();

    try
    {
        ProcessAimbot();

        void *current_Match = game_sdk->Curent_Match();
        if (!current_Match) return;

        void *local_player = game_sdk->GetLocalPlayer(current_Match);
        if (!local_player) return;

// if (local_player) {
//     // Chạy Reveal Enemy
//     if (RevealEnemy) {
//         RunRevealEnemy2();
//     }

//     // Chạy Xoay xoay
//     if (spin360) {
//         float delta_fix = 0.01f; // Fix lỗi GetDeltaTime
//         RotatePlayer360(local_player, delta_fix);
//     }
// }

        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)current_Match + 0x148);
        if (!players) return;

        void *camera = game_sdk->get_camera();
        if (!camera) return;
        
        void *camTransform = game_sdk->Component_GetTransform(camera);
        if (!camTransform) return;

        for (int u = 0; u < players->getSize(); u++)
        {
            void *closestEnemy = players->getValues()[u];
            if (!closestEnemy) continue;
            if (!game_sdk->Component_GetTransform(closestEnemy)) continue;
            if (closestEnemy == local_player) continue;
            if (!game_sdk->get_MaxHP(closestEnemy)) continue;
            if (game_sdk->get_IsDieing(closestEnemy)) continue;
            if (!game_sdk->get_isVisible(closestEnemy)) continue;
            if (game_sdk->get_isLocalTeam(closestEnemy)) continue;

            Vector3 pos = getPosition(closestEnemy);
            Vector3 pos2 = getPosition(local_player);
            float distance = Vector3::Distance(pos, pos2);
            if (distance > 200.0f) continue;
            
            ImColor line_color = ImColor(255, 255, 255);
            bool w2sc;
            ImVec2 top_pos = Camera$$WorldToScreen::Regular(pos + Vector3(0, 1.6, 0));
            ImVec2 bot_pos = Camera$$WorldToScreen::Regular(pos);
            ImVec2 pos_3 = Camera$$WorldToScreen::Checker(pos, w2sc);
            
            auto pmtXtop = top_pos.x;
            auto pmtXbottom = bot_pos.x;
            
            if (top_pos.x > bot_pos.x)
            {
                pmtXtop = bot_pos.x;
                pmtXbottom = top_pos.x;
            }
            
            Camera$$WorldToScreen::Checker(pos + Vector3(0, 0.75f, 0), w2sc);
            float calculatedPosition = fabs((top_pos.y - bot_pos.y) * (0.0092f / 0.019f) / 2);

            ImRect rect(
                ImVec2(pmtXtop - calculatedPosition, top_pos.y),
                ImVec2(pmtXbottom + calculatedPosition, bot_pos.y));
                
            const auto &viewpos = game_sdk->get_position(camTransform);
            
            if (w2sc)
            {
                if (Vars.lines)
                {
                    if (game_sdk->get_IsDieing(closestEnemy))
                        draw_list->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0), ImVec2(rect.GetCenter().x, rect.Min.y), ImColor(255, 0, 0));
                    else
                        draw_list->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0), ImVec2(rect.GetCenter().x, rect.Min.y), line_color);
                }
                if (Vars.Box)
                {
                    if (game_sdk->get_IsDieing(closestEnemy))
                        draw_list->AddRect(rect.Min, rect.Max, ImColor(255, 0, 0));
                    else
                        draw_list->AddRect(rect.Min, rect.Max, ImColor(255, 255, 255));
                    
                    if (Vars.Outline)
                    {
                        draw_list->AddRect(rect.Min - ImVec2(1, 1), rect.Max + ImVec2(1, 1), ImColor(0, 0, 0), 0.65, 0, 1);
                        draw_list->AddRect(rect.Min + ImVec2(1, 1), rect.Max - ImVec2(1, 1), ImColor(0, 0, 0), 0.65, 0, 1);
                    }
                }
                if (Vars.Name)
                {
                    auto pname = game_sdk->name(closestEnemy);
                    std::string names = "null";
                    if (pname)
                        names = pname->toCPPString();
                    std::transform(names.begin(), names.end(), names.begin(), ::tolower);
                    auto playername = names;
                    std::string name = names;
                    ImVec2 text_size = verdana_smol->CalcTextSizeA(8, FLT_MAX, 0, names.c_str());
                    ImVec2 name_pos = {
                        rect.Min.x + (rect.GetWidth() / 2) - text_size.x / 2,
                        rect.Min.y - 2 - text_size.y};
                    AddText(verdana_smol, 8, false, Vars.Outline, name_pos, ImColor(255, 255, 255), name);
                }
                if (Vars.Health)
                {
                    auto health = game_sdk->GetHp(closestEnemy);
                    auto maxhealth = game_sdk->get_MaxHP(closestEnemy);
                    float health_multiplier = (float)health / (float)maxhealth;
                    float health_bar_pos = rect.Min.x - 4;
                    draw_list->AddLine({health_bar_pos, rect.Min.y - 1}, {health_bar_pos, rect.Max.y}, ImColor(0, 0, 0, 100), 3);
                    draw_list->AddLine({health_bar_pos - 0.5f, rect.Max.y}, {health_bar_pos - 0.5f, rect.Max.y - (rect.GetHeight() + 1) * health_multiplier}, ImColor(0, 255, 0), 3);
                    if (Vars.Outline)
                        draw_list->AddRect({health_bar_pos - 2, rect.Min.y - 1}, {health_bar_pos + 2, rect.Max.y + 1}, ImColor(0, 0, 0));
                    std::string hpstr = fmt::format(oxorany("{}HP"), static_cast<int>(health));
                    ImVec2 text_size_hp = pixel_smol->CalcTextSizeA(8, FLT_MAX, 0, hpstr.c_str());
                    ImVec2 text_pos = {
                        rect.Min.x + (rect.GetWidth() / 2) - text_size_hp.x / 2,
                        rect.Max.y};
                    AddText(pixel_smol, 8, false, true, text_pos, ImColor(0, 255, 0), hpstr.c_str());
                }
                if (Vars.Distance)
                {
                    std::string distancestr = fmt::format(oxorany("{}M"), static_cast<int>(distance));
                    ImVec2 distance_pos = {
                        rect.Max.x + 4,
                        rect.Min.y};
                    AddText(pixel_smol, 8, false, true, distance_pos, ImColor(255, 255, 255), distancestr.c_str());
                }
                if (Vars.circlepos)
                {
                    Draw3DCircle(pos, 1.0f, 0.5f, ImColor(255, 0, 0), 36, false, 0.5f);
                }
                if (Vars.skeleton)
                {
                    DrawSkeleton(closestEnemy, draw_list);
                }
            }
            if (Vars.OOF)
            {
                if ((pos_3.x < 0 || pos_3.x > disp.width) || (pos_3.y < 0 || pos_3.y > disp.height) || !w2sc)
                {
                    constexpr int maxpixels = 150;
                    int pixels = maxpixels;
                    if (w2sc)
                    {
                        if (pos_3.x < 0)
                            pixels = clamp((int)-pos_3.x, 0, (int)maxpixels);
                        if (pos_3.y < 0)
                            pixels = clamp((int)-pos_3.y, 0, (int)maxpixels);

                        if (pos_3.x > disp.width)
                            pixels = clamp((int)pos_3.x - (int)disp.width, 0, (int)maxpixels);
                        if (pos_3.y > disp.height)
                            pixels = clamp((int)pos_3.y - (int)disp.height, 0, (int)maxpixels);
                    }

                    float opacity = (float)pixels / (float)maxpixels;
                    float size = 3.5f;
                    Vector3 viewdir = game_sdk->GetForward(camTransform);
                    Vector3 targetdir = Vector3::Normalized(pos - viewpos);

                    float viewangle = atan2(viewdir.z, viewdir.x) * Rad2Deg;
                    float targetangle = atan2(targetdir.z, targetdir.x) * Rad2Deg;

                    if (viewangle < 0) viewangle += 360;
                    if (targetangle < 0) targetangle += 360;

                    float angle = targetangle - viewangle;

                    while (angle < 0) angle += 360;
                    while (angle > 360) angle -= 360;

                    angle = 360 - angle;
                    angle -= 90;
                    OtFovV1(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2, 90 + distance * 2,
                            angle - size,
                            angle + size,
                            ImColor(1.f, 1.f, 1.f, 1.f * opacity), 1);
                }
            }
        }
    }
    catch (...)
    {
        return;
    }
}


// void AiPlayer2() {
//     // Sửa lỗi logic ||
//     if (!Vars.Enable || !Vars.AiPlayer2)
//         return;

//     void *match = game_sdk->Curent_Match();
//     if (!match) return;

//     void *local = game_sdk->GetLocalPlayer(match);
//     if (!local || !game_sdk->Component_GetTransform(local)) return;

//     void *localTF = game_sdk->Component_GetTransform(local);

//     Dictionary<uint8_t *, void *> *players = *(Dictionary<uint8_t *, void *> **)((long)match + oxo("0x148"));
//     if (!players || players->getValues().empty()) return;

//     // Lấy hướng nhìn của mình
//     Vector3 forward = game_sdk->GetForward(localTF);
//     // Tính toán hướng bên trái (Vuông góc với hướng nhìn)
//     Vector3 leftDir = Vector3(-forward.z, 0.0f, forward.x);

//     float pullStep = 1.5f;
//     float returnStep = 0.4f;

//     static int tick = 0;
//     tick++;

//     auto enemyList = players->getValues();
//     for (int i = 0; i < players->getSize(); i++) {
//         void *enemy = enemyList[i];
        
//         if (!enemy || enemy == local) continue;
//         void *enemyTF = game_sdk->Component_GetTransform(enemy);
//         if (!enemyTF) continue;
        
//         if (game_sdk->get_IsDieing(enemy)) continue;
//         if (game_sdk->get_isLocalTeam(enemy)) continue;

//         Vector3 enemyPos = game_sdk->get_position(enemyTF);

//         // Logic làm rung lắc vị trí địch
//         if (tick % 4 != 0) {
//             enemyPos.x += leftDir.x * pullStep;
//             enemyPos.z += leftDir.z * pullStep;
//         } else {
//             enemyPos.x -= leftDir.x * returnStep;
//             enemyPos.z -= leftDir.z * returnStep;
//         }

//         // Fix lỗi Vvector3 như lúc nãy nhé
//         Transform_INTERNAL_SetPosition(enemyTF, Vvector3(enemyPos.x, enemyPos.y, enemyPos.z));
//     }
// }

// // --- Hàm thực thi Teleport ---
// void TeleportMarkVoid() {
//     // Chỉ thực thi khi bật Checkbox (Vars.TeleportMark1) và đã có điểm đánh dấu
//     if (Vars.TeleportMark1 && HasMapMarker) {        
//         void* match = game_sdk->Curent_Match();
//         if (!match) return;
        
//         void* localPlayer = game_sdk->GetLocalPlayer(match);
//         if (!localPlayer) return;

//         void* tf = game_sdk->Component_GetTransform(localPlayer);
//         if (!tf) return;

//         Vector3 currentPos = game_sdk->get_position(tf);
        
//         // Dịch chuyển X, Z tới điểm mark, giữ nguyên Y (để không bị lún đất)
//         Vector3 targetPos;
//         targetPos.x = SavedMapMarker.x;
//         targetPos.y = currentPos.y + 1.0f; // Nhích lên 1m cho an toàn
//         targetPos.z = SavedMapMarker.z;

//         // Dùng hàm INTERNAL để thực thi
//         Transform_INTERNAL_SetPosition(tf, Vvector3(targetPos.x, targetPos.y, targetPos.z));
        
//         // Sau khi Tele xong thì tự tắt checkbox để tránh bị giật (Tùy bạn chọn)
//         Vars.TeleportMark1 = false; 
//     }
// }

// Khai báo con trỏ hàm gốc
void (*_AutoFire)(void *_this, int32_t pFireStatus, int32_t pFireMode);

void old_AutoFire(void *_this, int32_t pFireStatus, int32_t pFireMode) 
{
    // Loại bỏ hoàn toàn static clock và các biến delay
    
    if (_this != NULL && AutoFire) 
    {
        // Kiểm tra xem có mục tiêu trong tầm ngắm/quét không
        // Nếu CHECKPLAYER (đã tìm thấy địch) thì ép bắn 0s
        if (CHECKPLAYER) 
        {
            pFireStatus = FireStatus::FIRING; // Ép trạng thái bắn
            pFireMode = FireMode::AUTO;      // Ép chế độ sấy liên thanh
        }
    }

    return _AutoFire(_this, pFireStatus, pFireMode);
}


void aimbot()
{
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
    if (!Vars.Aimbot) return;
        
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list) return;
        
    void *Match = game_sdk->Curent_Match();
    if (!Match) return;
        
    if (Vars.isAimFov)
    {
        if (Vars.fovaimglow)
            drawcircleglow(draw_list, center, Vars.AimFov, ImColor(Vars.fovLineColor[0], Vars.fovLineColor[1], Vars.fovLineColor[2], Vars.fovLineColor[3]), 999, 1, 12);
        else
            draw_list->AddCircle(center, Vars.AimFov, ImColor(Vars.fovLineColor[0], Vars.fovLineColor[1], Vars.fovLineColor[2], Vars.fovLineColor[3]), 100);
    }
    void *LocalPlayer = game_sdk->GetLocalPlayer(Match);
    if (!LocalPlayer) return;
        
    void *playertarget = GetClosestEnemy();
    if (!playertarget) return;
        
    ImVec2 EnemyLocation = Camera$$WorldToScreen::Regular(GetHeadPosition(playertarget));
    drawlineglow(draw_list, ImVec2(center.x, center.y), EnemyLocation, ImColor(255, 255, 255), 1, 3);
}

void draw_watermark()
{
    std::string claw = oxorany("");
    ImVec2 text_size = verdana_smol->calc_size(1, claw);
    ImVec2 text_pos(
        10, 
        ImGui::GetIO().DisplaySize.y - text_size.y - 10); 
        
    AddText(verdana_smol, 16, false, false, text_pos + ImVec2(1, 1), ImColor(0, 0, 0, 150), claw);
    
    static float hue = 0.0f;
    hue += ImGui::GetIO().DeltaTime * 0.1f;
    if (hue > 1.0f)
        hue = 0.0f;
        
    ImColor rainbow = ImColor::HSV(hue, 0.8f, 0.8f);
    AddText(verdana_smol, 16, false, false, text_pos, rainbow, claw);
    
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddLine(
        ImVec2(text_pos.x, text_pos.y + text_size.y + 2),
        ImVec2(text_pos.x + text_size.x, text_pos.y + text_size.y + 2),
        rainbow,
        2.0f);
}
