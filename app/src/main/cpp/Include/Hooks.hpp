#pragma once
#include "BNM/Loading.hpp"
#include "BNM/Class.hpp"
#include "BNM/Method.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Field.hpp"
#include "BNM/FieldBase.hpp"
#include "BNM/UnityStructures/Color.hpp"
#include "BNM/UnityStructures/Quaternion.hpp"
#include "BNM/UnityStructures/Raycasthit.hpp"
#include "BNM/UnityStructures/Vector2.hpp"
#include "BNM/UnityStructures/Vector3.hpp"
#include "BNM/UnityStructures.hpp"
#include "BNM/BasicMonoStructures.hpp"
#include "BNM/Loading.hpp"
#include "BNM/Class.hpp"
#include "BNMResolve.hpp"
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/Class.hpp>
#include <BNM/ClassesManagement.hpp>
#include <BNM/ComplexMonoStructures.hpp>
#include <BNM/Coroutine.hpp>
#include <BNM/DebugMessages.hpp>
#include <BNM/Defaults.hpp>
#include <BNM/Delegates.hpp>
#include <BNM/Event.hpp>
#include <BNM/EventBase.hpp>
#include <BNM/EventBase.hpp>
#include <BNM/Exceptions.hpp>
#include <BNM/Field.hpp>
#include <BNM/FieldBase.hpp>
#include <BNM/Il2CppHeaders.hpp>
#include <BNM/Image.hpp>
#include <BNM/Loading.hpp>
#include <BNM/Method.hpp>
#include <BNM/MethodBase.hpp>
#include <BNM/Operators.hpp>
#include <BNM/Property.hpp>
#include <BNM/PropertyBase.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Utils.hpp>
#include "BNM/Loading.hpp"
#include "BNM/Class.hpp"
#include "BNM/Method.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Field.hpp"
#include "BNM/FieldBase.hpp"
#include "BNM/UnityStructures/Color.hpp"
#include "BNM/UnityStructures/Quaternion.hpp"
#include "BNM/UnityStructures/Raycasthit.hpp"
#include "BNM/UnityStructures/Vector2.hpp"
#include "BNM/UnityStructures/Vector3.hpp"
#include "BNM/UnityStructures.hpp"
#include "BNM/BasicMonoStructures.hpp"
#include "BNM/Loading.hpp"
#include "BNM/Class.hpp"
#include "BNMResolve.hpp"
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/Class.hpp>
#include <BNM/ClassesManagement.hpp>
#include <BNM/ComplexMonoStructures.hpp>
#include <BNM/Coroutine.hpp>
#include <BNM/DebugMessages.hpp>
#include <BNM/Defaults.hpp>
#include <BNM/Delegates.hpp>
#include <BNM/Event.hpp>
#include <BNM/EventBase.hpp>
#include <BNM/EventBase.hpp>
#include <BNM/Exceptions.hpp>
#include <BNM/Field.hpp>
#include <BNM/FieldBase.hpp>
#include <BNM/Il2CppHeaders.hpp>
#include <BNM/Image.hpp>
#include <BNM/Loading.hpp>
#include <BNM/Method.hpp>
#include <BNM/MethodBase.hpp>
#include <BNM/Operators.hpp>
#include <BNM/Property.hpp>
#include <BNM/PropertyBase.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Utils.hpp>
#include "GorillaLocomotion.hpp"
namespace Hooks {
    bool (*RaiseEvent)(BNM::Types::byte, void*, void*, void*);
    bool new_RaiseEvent(BNM::Types::byte eventCode, void* eventContent, void* raiseEventOptions, void* sendOptions) {
        if (eventCode == (BNM::Types::byte)8) {
            return false;
        }

        BNM_LOG_INFO("%s was called", to_string(eventCode).c_str());
        return RaiseEvent(eventCode, eventContent, raiseEventOptions, sendOptions);
    }

    float (*GetSlidePercentage)(void*, RaycastHit);
    float new_GetSlidePercentage(void* instance, RaycastHit hit) {
        float slip = GetSlidePercentage(instance, hit);

        if (reinterpret_cast<Collider*>(hit.GetCollider())->GetGameObject()->GetName().starts_with("Frozone")) {
            return 1.0f;
        }

        return slip;
    }

    bool (*IsItemAllowed)(void*, void*);
    bool new_IsItemAllowed(void*, void*) {
        return true;
    }



    void (*OnDisable)(VRRig*);
    void new_OnDisable(VRRig* instance) {
        if (!instance->Same(GorillaTagger::get_Instance()->GetMyVRRig())) {
            OnDisable(instance);
        }
    }

    void (*DeregisterVRRig)(void*, VRRig*);
    void new_ReregisterVRRig(void* instance, VRRig* rig) {
        if (!rig->Same(GorillaTagger::get_Instance()->GetMyVRRig())) {
            DeregisterVRRig(instance, rig);
        }
    }

    void new_SendReport(void*, Mono::String*,Mono::String*,Mono::String*) {}
    void new_LogErrorCount(void*, Mono::String*,Mono::String*,int) {}
    void new_CloseInvalidRoom(void*) {}
    void new_GetRPCCallTracker(void*, void*, Mono::String*) {}
    bool new_ShouldDisconnectFromRoom(void*) { return false; }
    bool new_get_sendReport(void*) { return false; }
    bool new_ValidRoomName(void*) { return true; }


    void Init() {
        static Class GorillaNot = Class(O(""), O("GorillaNot"));
        BasicHook(GorillaNot.GetMethod(O("SendReport"), 3), new_SendReport, nullptr);
        BasicHook(GorillaNot.GetMethod(O("LogErrorCount"), 3), new_LogErrorCount, nullptr);
        BasicHook(GorillaNot.GetMethod(O("CloseInvalidRoom"), 1), new_CloseInvalidRoom, nullptr);
        BasicHook(GorillaNot.GetMethod(O("GetRPCCallTracker"), 3), new_GetRPCCallTracker, nullptr);
        BasicHook(GorillaNot.GetMethod(O("ShouldDisconnectFromRoom"), 1), new_ShouldDisconnectFromRoom, nullptr);
        BasicHook(GorillaNot.GetMethod(O("get_sendReport")), new_get_sendReport, nullptr);
        BasicHook(GorillaNot.GetMethod(O("ValidRoomName")), new_ValidRoomName, nullptr);

        InvokeHook(Class(O(""), O("VRRig")).GetMethod(O("OnDisable"), 1), new_OnDisable, OnDisable);
        BasicHook(Class(O(""), O("VRRig")).GetMethod(O("IsItemAllowed"), 1), new_IsItemAllowed, IsItemAllowed);
        BasicHook(Class(O(""), O("VRRigJobManager")).GetMethod(O("DeregisterVRRig"), 1), new_ReregisterVRRig, DeregisterVRRig);
        BasicHook(GorillaLocomotion::Player::GetClass().GetMethod(O("GetSlidePercentage"), 1), new_GetSlidePercentage, GetSlidePercentage);
    }
}