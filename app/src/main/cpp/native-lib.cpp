// menu made by crimson and famousseamus.
// optimized by nawly.

// glitches port is ai so it's unoptimized.
// how did y'all not realize that.
// so here is a better template for y'all.

// discord.gg/spm
// discord.gg/anotheraxiom

// how to change the unity version:
// go to extern -> BNM-Android -> include.BNM -> UserSettings -> GlobalSettings.hpp.

#pragma once

#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <json.hpp>
#include <vector>
#include <unordered_set>
#include <map>
#include <mutex>
#include <functional>
#include <string>
#include <XRInput.hpp>
#include <GorillaLocomotion.hpp>
#include "BNMIncludes.h"
#include "Hooks.hpp"
#include "GUIManager.hpp"
#include "PhotonResolve.hpp"

#define O(str) BNM_OBFUSCATE(str)

VRRig vrrrig;
GUIManager* gui;

using namespace BNM;
using namespace BNM::IL2CPP;
using namespace BNM::Structures;
using namespace BNM::Structures::Unity;
using namespace BNM::UnityEngine;
using namespace std;
using namespace Hooks;
using namespace BNM::Defaults;

Class GorillaGameManager;
Class PhotonNetwork;

Method<bool> InRoom;
static Method<string> ServerIp;
static int currentPage = 0;
static const int buttonsPerPage = 6;
static bool mastaClient = false;
static GameObject *menu = nullptr;
static GameObject *canvasObj = nullptr;
static GameObject *referance = nullptr;
static GameObject *pointer = nullptr;
static int framePressCooldown = 0;
static float fps;

struct ModButton {
    std::string name;
    bool isToggle;
    bool isCategory;
    std::function<void()> action;
};

vector<ModButton> homePage;

string currentCategory = O("home");
map<string, bool> toggleStates;
map<string, vector<ModButton>> categories;
map<string, std::function<void()>> toggleActions;

static bool buttonsActive[] = {false, false, false, false, false, false};

static void Draw();
static void Toggle(Mono::String* relatedText);

vector<ModButton> EnabledMods() {
    vector<ModButton> mods;
    ModButton backButton;
    backButton.name = "Back";
    backButton.isToggle = false;
    backButton.isCategory = true;
    backButton.action = []() {
        currentCategory = "home";
        Draw();
    };
    mods.push_back(backButton);
    unordered_set<string> seen;

    for (const auto& [catName, buttons] : categories) {
        for (const auto& btn : buttons) {
            if (btn.isToggle && toggleStates[btn.name]) {
                if (seen.insert(btn.name).second) {
                    mods.push_back(btn);
                }
            }
        }
    }

    return mods;
}
void Home() {
    homePage.clear();
    for (auto& [name, _] : categories) {
        homePage.push_back({ name, false, true, [name]() {
            currentCategory = name;
            Draw();
        }});
    }
}

//--------------------------------------------//
// THIS IS WHERE U SETUP MODS YOU SILLY THING.//
//--------------------------------------------//

// yo that deadass looks cool lol

void InitCategories() {
    // add ur categories to here
    vector<ModButton> movementMods = {
            {"Back",       false, true,  []() {
                currentCategory = "home";
                Draw();
            }},
            {"Speedboost", false, false, []() {
                GorillaLocomotion::Player::get_Instance()->SetMaxJumpSpeed(999.f);
                GorillaLocomotion::Player::get_Instance()->SetJumpMultiplier(2.25f);
            }},
            // add more mods and as you can see there is a "isToggle" yea
    };
    vector<ModButton> blabla = {
            {"Back", false, true, []() {
                currentCategory = "home";
                Draw();
            }},                // or true
            {"Back", false, false, []() {
                // add ur mod function here
            }},
            // add more mods and as you can see there is a "isToggle" yea
    };


    // add ur categories to here
    categories["Enabled"] = EnabledMods();
    categories["Movement"] = movementMods;
    categories["BLA BLA"] = blabla;

    // add ur categories to here
    homePage.clear();
    vector<string> catergories = {
            "Enabled",
            "Movement",
            "BLA BLA",
    };
    for (const string& name : catergories) {
        homePage.push_back({ name, false, true, [name]() {
            currentCategory = name;
            currentPage = 0;
            Draw();
        }});
    }
}
 
void UpdateToggleStuff() {
    for (auto& [name, action] : toggleActions) {
        action();
    }
}
 

void UpdateFPS() {
    fps = 1.0f / Time::GetUnscaledDeltaTime();
}

void (*Update)(void*);
void new_Update(void* instance) {
    Update(instance);
    UpdateFPS();
    UpdateToggleStuff();
    static bool gripDown;
    static bool gravityToggled = false;
    static bool flying = false;
    static int btnCooldown = 0;
    static float maxJumpSpeed = 0;
    if (maxJumpSpeed == 0) {
        maxJumpSpeed = GorillaLocomotion::Player::get_Instance()->GetMaxJumpSpeed();
    }
 
    float grip = XRInput::GetBoolFeature(BoolFeature::SecondaryButton, Controller::Left);
    if (grip) {
        gripDown = true;
    }
    else {
        gripDown = false;
    }

    if (gripDown && menu == nullptr) {
        InitCategories();
        Draw();
        if (referance == nullptr) {
            referance = GameObject::CreatePrimitive(PrimitiveType::Sphere);
            //GameObject::Destroy(referance->GetComponent(MeshRenderer::GetType())); // comment this out to make the referance visible.
            // ^^^ Referance is on the left side of ur right hand for some reason (on the github gifs, its on the finger i have no clue why)
            referance->GetTransform()->SetParent(GameObject::Find("RightHand Controller")->GetTransform());
            reinterpret_cast<Renderer*>(referance->GetComponent(Renderer::GetType()))->GetMaterial()->SetColor(Color(0.4f, 0.0f, 0.6f));
            referance->GetTransform()->SetLocalPosition(Vector3(0.f, -0.1f, 0.f));
            referance->GetTransform()->SetLocalScale(Vector3(0.01f, 0.01f, 0.01f));
        }
    } else if (!gripDown && menu != nullptr) {
        GameObject::Destroy(menu);
        menu = nullptr;
        GameObject::Destroy(referance);
        referance = nullptr;
    }

    if (gripDown && menu != nullptr) {
        menu->GetTransform()->SetPosition(GameObject::Find("LeftHand Controller")->GetTransform()->GetPosition());
        menu->GetTransform()->SetRotation(GameObject::Find("LeftHand Controller")->GetTransform()->GetRotation());
    }

    if (btnCooldown > 0) {
        if (Time::GetFrameCount() > btnCooldown) {
            btnCooldown = 0;
            buttonsActive[3] = false;
            GameObject::Destroy(menu);
            menu = nullptr;
            Draw();
        }
    }
}
//}
struct BtnCollider : BNM::UnityEngine::MonoBehaviour
{
    BNM_CustomClass(BtnCollider,
                    BNM::CompileTimeClassBuilder(BNM_OBFUSCATE_TMP(O("")), BNM_OBFUSCATE_TMP(O("BtnCollider"))).Build(),
                    BNM::CompileTimeClassBuilder(BNM_OBFUSCATE_TMP(O("UnityEngine")), BNM_OBFUSCATE_TMP(O("MonoBehaviour")), BNM_OBFUSCATE_TMP(O("UnityEngine.CoreModule"))).Build(),
                    {},
    ); // custom monobehavior yappington, cool part is this acts as a real class and i can do like Class BtnCollider = Class(O(""), O("BtnCollider")); and actually load it, i do that later in the code.

    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = BtnCollider();
        *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;
    }

    void Start() {
    }
    Mono::String* relatedText;
    void OnTriggerEnter(Collider* collider)
    {
        if (Time::GetFrameCount() >= framePressCooldown + 30)
        {
            vrrrig.PlayHandTap(8, true, 999999.f);
            Toggle(relatedText);
            framePressCooldown = Time::GetFrameCount();
        }
    }
    BNM_CustomField(relatedText, BNM::Defaults::Get<Mono::String*>(), "relatedText");
    BNM_CustomMethod(OnTriggerEnter, false, BNM::Defaults::Get<void>(), "OnTriggerEnter");
    BNM_CustomMethod(Start, false, BNM::Defaults::Get<void>(), "Start");
    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");
};
void AddButton(float offset, const std::string& text, bool toggled = false)
{
    GameObject* newBtn = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(newBtn->GetComponent(Rigidbody::GetType()));
    reinterpret_cast<Collider*>(newBtn->GetComponent(BoxCollider::GetType()))->SetIsTrigger(true);
    Rigidbody* rb = (Rigidbody*)newBtn->AddComponent(Rigidbody::GetType());
    rb->SetIsKinematic(true);
    newBtn->GetTransform()->SetParent(menu->GetTransform());
    newBtn->GetTransform()->SetRotation(Quaternion::identity);
    newBtn->GetTransform()->SetLocalScale(Vector3(0.09f, 0.8f, 0.08f));
    newBtn->GetTransform()->SetLocalPosition(Vector3(0.56f, 0.f, 0.28f - offset));
    Class BtnCollider = Class(O(""), O("BtnCollider"));
    newBtn->AddComponent(BtnCollider.GetMonoType());
    Field<Mono::String*> relatedText = BtnCollider.GetField(O("relatedText"));
    Il2CppObject* btnInst = reinterpret_cast<Behaviour*>(newBtn->GetComponent(BtnCollider.GetMonoType()));
    relatedText.SetInstance(btnInst);
    relatedText.Set(CreateMonoString(text));

    Renderer* renderer = reinterpret_cast<Renderer*>(newBtn->GetComponent(Renderer::GetType()));
    renderer->GetMaterial()->SetColor(toggled ? Color(0.4f, 0.05f, 0.05f, 1.f) : Color(0.1f, 0.1f, 0.1f, 1.0f));

    GameObject* outlineObj = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(outlineObj->GetComponent(Rigidbody::GetType()));
    GameObject::Destroy(outlineObj->GetComponent(BoxCollider::GetType()));
    GameObject::Destroy(outlineObj->GetComponent(MeshCollider::GetType()));
    Renderer* outlineRenderer = reinterpret_cast<Renderer*>(outlineObj->GetComponent(Renderer::GetType()));
    outlineRenderer->GetMaterial()->SetColor(Color(0.4f, 0.0f, 0.6f));
    outlineObj->GetTransform()->SetParent(newBtn->GetTransform());
    outlineObj->GetTransform()->SetLocalPosition(Vector3(0.f, 0.f, 0.01f));
    outlineObj->GetTransform()->SetLocalRotation(Quaternion::identity);
    outlineObj->GetTransform()->SetLocalScale(Vector3(0.9f, 1.01f, 1.05f));

    GameObject* titleObj = (GameObject*)GameObject::GetClass().CreateNewObjectParameters();
    titleObj->GetTransform()->SetParent(canvasObj->GetTransform());
    Text* title = (Text*)(titleObj->AddComponent(Text::GetType()));
    title->SetFont((Font*)(Resources::GetBuiltinResource(Font::GetType(), O("Arial.ttf")))); // same font as original but feel free to change
    title->SetText(text);
    title->SetFontSize(1);
    title->SetAlignment(TextAnchor::MiddleCenter);
    title->SetTextResizeForBestFit(true);
    title->SetResizeTextMinSize(0);
    RectTransform* titleTransform = title->GetRectTransform();
    titleTransform->SetLocalPosition(Vector3::zero);
    titleTransform->SetSizeDelta(Vector2(0.2f, 0.03f));
    titleTransform->SetLocalPosition(Vector3(0.064f, 0.f, 0.111f - (offset / 2.55f)));
    titleTransform->SetRotation(Quaternion::FromEuler(Vector3(180.f, 90.f, 90.f)));
}
void AddDisconnectButton(float offset)
{
    //another dupe of AddButton for disconnect
    GameObject* newBtn = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(newBtn->GetComponent(Rigidbody::GetType()));
    reinterpret_cast<Collider*>(newBtn->GetComponent(BoxCollider::GetType()))->SetIsTrigger(true);
    Rigidbody* rb = (Rigidbody*)newBtn->AddComponent(Rigidbody::GetType());
    rb->SetIsKinematic(true);
    newBtn->GetTransform()->SetParent(menu->GetTransform());
    newBtn->GetTransform()->SetRotation(Quaternion::identity);
    newBtn->GetTransform()->SetLocalScale(Vector3(0.09f, 0.8f, 0.08f));
    newBtn->GetTransform()->SetLocalPosition(Vector3(0.56f, 0.f, 0.28f - offset));
    Class BtnCollider = Class(O(""), O("BtnCollider"));
    newBtn->AddComponent(BtnCollider.GetMonoType());
    Field<Mono::String*> relatedText = BtnCollider.GetField(O("relatedText"));
    Il2CppObject* btnInst = reinterpret_cast<Behaviour*>(newBtn->GetComponent(BtnCollider.GetMonoType()));
    relatedText.SetInstance(btnInst);
    relatedText.Set(CreateMonoString("Disconnect"));

    Renderer* renderer = reinterpret_cast<Renderer*>(newBtn->GetComponent(Renderer::GetType()));
    renderer->GetMaterial()->SetColor(Color(0.4f, 0.0f, 0.6f));

    GameObject* outlineObj = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(outlineObj->GetComponent(Rigidbody::GetType()));
    GameObject::Destroy(outlineObj->GetComponent(BoxCollider::GetType()));
    GameObject::Destroy(outlineObj->GetComponent(MeshCollider::GetType()));
    Renderer* outlineRenderer = reinterpret_cast<Renderer*>(outlineObj->GetComponent(Renderer::GetType()));
    outlineRenderer->GetMaterial()->SetColor(Color::black);
    outlineObj->GetTransform()->SetParent(newBtn->GetTransform());
    outlineObj->GetTransform()->SetLocalPosition(Vector3(0.f, 0.f, 0.01f));
    outlineObj->GetTransform()->SetLocalRotation(Quaternion::identity);
    outlineObj->GetTransform()->SetLocalScale(Vector3(0.9f, 1.01f, 1.05f));
    GameObject* titleObj = (GameObject*)GameObject::GetClass().CreateNewObjectParameters();
    titleObj->GetTransform()->SetParent(canvasObj->GetTransform());
    Text* title = (Text*)(titleObj->AddComponent(Text::GetType()));
    title->SetFont((Font*)(Resources::GetBuiltinResource(Font::GetType(), O("Arial.ttf"))));
    title->SetText("Disconnect");
    title->SetFontSize(1);
    title->SetAlignment(TextAnchor::MiddleCenter);
    title->SetTextResizeForBestFit(true);
    title->SetResizeTextMinSize(0);

    RectTransform* titleTransform = title->GetRectTransform();
    titleTransform->SetLocalPosition(Vector3::zero);
    titleTransform->SetSizeDelta(Vector2(0.2f, 0.03f));
    titleTransform->SetLocalPosition(Vector3(0.064f, 0.f, 0.111f - (offset / 2.55f)));
    titleTransform->SetRotation(Quaternion::FromEuler(Vector3(180.f, 90.f, 90.f)));
}
void AddSidePageButton(bool left, const std::string& text)
{
    //drawing
    GameObject* newBtn = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(newBtn->GetComponent(Rigidbody::GetType()));
    reinterpret_cast<Collider*>(newBtn->GetComponent(BoxCollider::GetType()))->SetIsTrigger(true);
    Rigidbody* rb = (Rigidbody*)newBtn->AddComponent(Rigidbody::GetType());
    rb->SetIsKinematic(true);

    newBtn->GetTransform()->SetParent(menu->GetTransform());
    newBtn->GetTransform()->SetRotation(Quaternion::FromEuler(Vector3(90.0f, 0.f, 0.f)));
    newBtn->GetTransform()->SetLocalScale(Vector3(0.04f, 0.6f, 0.12f));
    float xPos = left ? 0.5f : 0.5f;
    float yPos = left ? 0.6f : -0.6f;
    newBtn->GetTransform()->SetLocalPosition(Vector3(xPos, yPos, 0.f));

    Class BtnCollider = Class(O(""), O("BtnCollider"));
    newBtn->AddComponent(BtnCollider.GetMonoType());
    Field<Mono::String*> relatedText = BtnCollider.GetField(O("relatedText"));
    Il2CppObject* btnInst = reinterpret_cast<Behaviour*>(newBtn->GetComponent(BtnCollider.GetMonoType()));
    relatedText.SetInstance(btnInst);
    relatedText.Set(CreateMonoString(text));
    Renderer* renderer = reinterpret_cast<Renderer*>(newBtn->GetComponent(Renderer::GetType()));
    renderer->GetMaterial()->SetColor(Color(0.0f, 0.0f, 0.0f, 1.0f));

    //actual outline object coming into play
    GameObject* outlineObj = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(outlineObj->GetComponent(Rigidbody::GetType()));
    GameObject::Destroy(outlineObj->GetComponent(BoxCollider::GetType()));
    GameObject::Destroy(outlineObj->GetComponent(MeshCollider::GetType()));

    Renderer* outlineRenderer = reinterpret_cast<Renderer*>(outlineObj->GetComponent(Renderer::GetType()));
    outlineRenderer->GetMaterial()->SetColor(Color(0.4f, 0.0f, 0.6f));
    outlineObj->GetTransform()->SetParent(newBtn->GetTransform());
    outlineObj->GetTransform()->SetLocalPosition(Vector3(0.f, 0.f, -0.05f)); // Very slightly behind
    outlineObj->GetTransform()->SetLocalRotation(Quaternion::identity);
    outlineObj->GetTransform()->SetLocalScale(Vector3(0.95f, 1.01f, 1.05f));
 
    //text stuff
    GameObject* titleObj = (GameObject*)GameObject::GetClass().CreateNewObjectParameters();
    titleObj->GetTransform()->SetParent(canvasObj->GetTransform());
    Text* title = (Text*)(titleObj->AddComponent(Text::GetType()));
    title->SetFont((Font*)(Resources::GetBuiltinResource(Font::GetType(), O("Arial.ttf")))); // same font as original but feel free to change
    title->SetText(text);
    title->SetFontSize(1);
    title->SetAlignment(TextAnchor::MiddleCenter);
    title->SetTextResizeForBestFit(true);
    title->SetResizeTextMinSize(0);
    RectTransform* titleTransform = title->GetRectTransform();
    titleTransform->SetLocalPosition(Vector3::zero);
    titleTransform->SetSizeDelta(Vector2(0.2f, 0.03f));
    titleTransform->SetLocalPosition(Vector3(0.064f, 0.f, 0.111f - (2.55f)));
    titleTransform->SetRotation(Quaternion::FromEuler(Vector3(180.f, 90.f, 90.f)));
}
static void Draw()
{
    Home(); //this is just the home page, its what draws it.
    vector<ModButton>* catButtons = (currentCategory == O("home")) ? &homePage : &categories[currentCategory]; //makes it so the hom page has the category buttons and some other stuff. pretty useful.
    GameObject::Destroy(menu); //incase the menu wasnt nullptr or just non-existant, it destroys the old one to prevent overlapping and stuff.
    //menu stuff and stuff :)
    menu = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(menu->GetComponent(Rigidbody::GetType()));
    GameObject::Destroy(menu->GetComponent(BoxCollider::GetType()));
    GameObject::Destroy(menu->GetComponent(Renderer::GetType()));
    menu->GetTransform()->SetLocalScale(Vector3(0.1f, 0.3f, 0.4f));
    //actual background/object for the menu
    GameObject* background = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(background->GetComponent(Rigidbody::GetType()));
    GameObject::Destroy(background->GetComponent(BoxCollider::GetType()));
    background->GetTransform()->SetParent(menu->GetTransform());
    background->GetTransform()->SetRotation(Quaternion::identity);
    background->GetTransform()->SetLocalScale(Vector3(0.1f, 1.f, 1.f));
    reinterpret_cast<Renderer*>(background->GetComponent(Renderer::GetType()))->GetMaterial()->SetColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
    background->GetTransform()->SetPosition(Vector3(0.05f, 0.f, 0.f));
    //canvas stuff for the menu
    canvasObj = (GameObject*)GameObject::GetClass().CreateNewObjectParameters();
    canvasObj->GetTransform()->SetParent(menu->GetTransform());
    Canvas* canvas = (Canvas*)canvasObj->AddComponent(Canvas::GetType());
    CanvasScaler* canvasScale = (CanvasScaler*)canvasObj->AddComponent(CanvasScaler::GetType());
    canvasObj->AddComponent(GraphicRaycaster::GetType());
    canvas->SetRenderMode(RenderMode::WorldSpace);
    canvasScale->SetDynamicPixelsPerUnit(1000);

    //outline stuff
    GameObject* outlineObj = GameObject::CreatePrimitive(PrimitiveType::Cube);
    GameObject::Destroy(outlineObj->GetComponent(Rigidbody::GetType()));
    GameObject::Destroy(outlineObj->GetComponent(BoxCollider::GetType()));
    outlineObj->GetTransform()->SetParent(menu->GetTransform());
    outlineObj->GetTransform()->SetRotation(Quaternion::identity);
    outlineObj->GetTransform()->SetLocalScale(Vector3(0.090f, 1.01f, 1.01f));
    outlineObj->GetTransform()->SetPosition(Vector3(0.05f, 0.f, 0.f));
    Renderer* outlineRenderer = reinterpret_cast<Renderer*>(outlineObj->GetComponent(Renderer::GetType()));
    outlineRenderer->GetMaterial()->SetColor(Color(0.4f, 0.0f, 0.6f));

    //title shit
    GameObject* titleObj = (GameObject*)GameObject::GetClass().CreateNewObjectParameters();
    titleObj->GetTransform()->SetParent(canvasObj->GetTransform());
    Text* title = (Text*)titleObj->AddComponent(Text::GetType());
    title->SetFont((Font*)(Resources::GetBuiltinResource(Font::GetType(), O("Arial.ttf"))));
    title->SetText((O("<color=#660099>Bus</color><color=grey> Client</color>\n<i><color=grey>[") + currentCategory + "]</color></i>").c_str()); // here is the menu title, change this iyw
    // change that if u want to change stuff yes.
    title->SetFontSize(1);
    title->SetAlignment(TextAnchor::MiddleCenter);
    title->SetTextResizeForBestFit(true);
    title->SetResizeTextMinSize(0);
    RectTransform* titleTransform = title->GetRectTransform();
    titleTransform->SetLocalPosition(Vector3::zero);
    titleTransform->SetSizeDelta(Vector2(0.28f, 0.05f));
    titleTransform->SetPosition(Vector3(0.06f, 0.f, 0.175f));
    titleTransform->SetRotation(Quaternion::FromEuler(Vector3(180.f, 90.f, 90.f)));
    //this is for pages + limiting the buttons to 4 per page.
    int startIndex = currentPage * buttonsPerPage;
    int endIndex = min((int)catButtons->size(), startIndex + buttonsPerPage);
    float offset = 0;
    //this is where limiting buttons for each page comes into play.
    for (int i = startIndex; i < endIndex; i++) {
        const auto& btn = catButtons->at(i);
        bool toggled = btn.isToggle && toggleStates[btn.name];
        AddButton(offset, btn.name, toggled);
        offset += 0.13f;
    }
    //page buttons that get added
    AddDisconnectButton(-0.30f);
    AddSidePageButton(true, O("<"));
    AddSidePageButton(false, O(">"));
}
static void Toggle(Mono::String* relatedText) //handles pages + other stuff related to toggling and stuff. works pretty nice
{
    string text = relatedText->str();
    vector<ModButton>* currentList = (currentCategory == O("home")) ? &homePage : &categories[currentCategory];
    int total = (currentCategory == O("home")) ? homePage.size() : categories[currentCategory].size();
    int totalPages = (total + buttonsPerPage - 1) / buttonsPerPage;
    vrrrig.PlayHandTap(8, true, 999999.f);
    if (text == "Disconnect") {
        Method<void*> dc = PhotonNetwork.GetMethod("Disconnect");
        dc.Call();
    }
    if (totalPages > 0) {
        if (text == O(">")) {
            currentPage = (currentPage + 1) % totalPages;
        } else if (text == O("<")) {
            currentPage = (currentPage == 0) ? (totalPages - 1) : (currentPage - 1);
        }
    }
    for (auto& btn : *currentList) {
        if (btn.name == text) {
            if (btn.isToggle) {
                toggleStates[btn.name] = !toggleStates[btn.name];
                if (toggleStates[btn.name]) {
                    toggleActions[btn.name] = btn.action;  //if isToggle then it will nicely be
                } else {
                    toggleActions.erase(btn.name); //just turns it off/removes it from executing after toggled off
                }
            } else {
                btn.action(); //just calls once when the isToggle is false (a button)
            }
            break;
        }
    }

    GameObject::Destroy(menu);
    menu = nullptr;
    Draw();
}

void OnLoaded() {
    PhotonNetwork = Class(O("Photon.Pun"), O("PhotonNetwork"), Image(O("PhotonUnityNetworking")));
    GorillaGameManager = Class("", "GorillaTagManager");
    InRoom = PhotonNetwork.GetMethod(O("get_InRoom"));
    ServerIp = PhotonNetwork.GetMethod("get_ServerAddress");  //not useless this guy is lying!!!  // again, useless check but if you want it to work just like the original, you can uncomment the check in new_Update and the menu will only work if ur in a room.
    InvokeHook(Class("GorillaNetworking", "CosmeticsController").GetMethod("Update"), new_Update, Update);
    //InvokeHook(Class("Photon.VR", "PhotonVRManager").GetProperty("IsCorrect"), new_IsCorrect, IsCorrect);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);
    BNM::Loading::TryLoadByJNI(env);
    return JNI_VERSION_1_6;
}
