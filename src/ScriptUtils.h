#ifndef PLAYER_SCRIPTUTILS_H
#define PLAYER_SCRIPTUTILS_H

#include <cassert>

#include "CKGlobals.h"
#include "CKBeObject.h"
#include "CKBehavior.h"
#include "CKBehaviorIO.h"
#include "CKBehaviorLink.h"
#include "CKParameter.h"
#include "CKParameterIn.h"
#include "CKParameterOut.h"
#include "CKParameterLocal.h"
#include "CKParameterManager.h"

namespace scriptutils {
    inline CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehaviorIO *out, int delay = 0) {
        assert(script != nullptr);
        assert(in != nullptr);
        assert(out != nullptr);

        CKBehaviorLink *link = (CKBehaviorLink *) script->GetCKContext()->CreateObject(CKCID_BEHAVIORLINK);
        link->SetInBehaviorIO(in);
        link->SetOutBehaviorIO(out);
        link->SetInitialActivationDelay(delay);
        link->ResetActivationDelay();
        script->AddSubBehaviorLink(link);
        return link;
    }

    inline CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehaviorIO *out, int inPos, int delay = 0) {
        assert(script != nullptr);
        assert(inBeh != nullptr);
        assert(out != nullptr);

        return CreateBehaviorLink(script, inBeh->GetOutput(inPos), out, delay);
    }

    inline CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehavior *outBeh, int outPos, int delay = 0) {
        assert(script != nullptr);
        assert(in != nullptr);
        assert(outBeh != nullptr);

        return CreateBehaviorLink(script, in, outBeh->GetInput(outPos), delay);
    }

    inline CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehavior *outBeh, int inPos = 0, int outPos = 0, int delay = 0) {
        return CreateBehaviorLink(script, inBeh->GetOutput(inPos), outBeh->GetInput(outPos), delay);
    }

    inline CKBehaviorLink *GetBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehaviorIO *out, CKBehaviorLink *previous = nullptr) {
        assert(script != nullptr);
        assert(in != nullptr);
        assert(out != nullptr);

        const int linkCount = script->GetSubBehaviorLinkCount();
        int i = 0;

        if (previous) {
            for (; i < linkCount; ++i) {
                CKBehaviorLink *link = script->GetSubBehaviorLink(i);
                if (link == previous) {
                    ++i;
                    break;
                }
            }
        }

        for (; i < linkCount; ++i) {
            CKBehaviorLink *link = script->GetSubBehaviorLink(i);
            CKBehaviorIO *inIO = link->GetInBehaviorIO();
            CKBehaviorIO *outIO = link->GetOutBehaviorIO();
            if (inIO == in && outIO == out)
                return link;
        }

        return nullptr;
    }

    inline CKBehaviorLink *GetBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehavior *outBeh,
                                           int inPos = 0, int outPos = 0, CKBehaviorLink *previous = nullptr) {
        assert(script != nullptr);
        assert(inBeh != nullptr);
        assert(outBeh != nullptr);

        const int linkCount = script->GetSubBehaviorLinkCount();
        int i = 0;

        if (previous) {
            for (; i < linkCount; ++i) {
                CKBehaviorLink *link = script->GetSubBehaviorLink(i);
                if (link == previous) {
                    ++i;
                    break;
                }
            }
        }

        for (; i < linkCount; ++i) {
            CKBehaviorLink *link = script->GetSubBehaviorLink(i);
            CKBehaviorIO *inIO = link->GetInBehaviorIO();
            CKBehaviorIO *outIO = link->GetOutBehaviorIO();
            if (inIO->GetOwner() == inBeh && inBeh->GetOutput(inPos) == inIO &&
                outIO->GetOwner() == outBeh && outBeh->GetInput(outPos) == outIO)
                return link;
        }

        return nullptr;
    }

    inline CKBehaviorLink *GetBehaviorLink(CKBehavior *script, const char *inBehName, CKBehavior *outBeh,
                                           int inPos = 0, int outPos = 0, CKBehaviorLink *previous = nullptr) {
        assert(script != nullptr);
        assert(inBehName != nullptr);
        assert(outBeh != nullptr);

        const int linkCount = script->GetSubBehaviorLinkCount();
        int i = 0;

        if (previous) {
            for (; i < linkCount; ++i) {
                CKBehaviorLink *link = script->GetSubBehaviorLink(i);
                if (link == previous) {
                    ++i;
                    break;
                }
            }
        }

        for (; i < linkCount; ++i) {
            CKBehaviorLink *link = script->GetSubBehaviorLink(i);
            CKBehaviorIO *inIO = link->GetInBehaviorIO();
            CKBehaviorIO *outIO = link->GetOutBehaviorIO();
            CKBehavior *inBeh = inIO->GetOwner();
            if (strcmp(inBeh->GetName(), inBehName) == 0 && inBeh->GetOutput(inPos) == inIO &&
                outIO->GetOwner() == outBeh && outBeh->GetInput(outPos) == outIO)
                return link;
        }

        return nullptr;
    }

    inline CKBehaviorLink *GetBehaviorLink(CKBehavior *script, CKBehavior *inBeh, const char *outBehName,
                                           int inPos = 0, int outPos = 0, CKBehaviorLink *previous = nullptr) {
        assert(script != nullptr);
        assert(inBeh != nullptr);
        assert(outBehName != nullptr);

        const int linkCount = script->GetSubBehaviorLinkCount();
        int i = 0;

        if (previous) {
            for (; i < linkCount; ++i) {
                CKBehaviorLink *link = script->GetSubBehaviorLink(i);
                if (link == previous) {
                    ++i;
                    break;
                }
            }
        }

        for (; i < linkCount; ++i) {
            CKBehaviorLink *link = script->GetSubBehaviorLink(i);
            CKBehaviorIO *inIO = link->GetInBehaviorIO();
            CKBehaviorIO *outIO = link->GetOutBehaviorIO();
            CKBehavior *outBeh = outIO->GetOwner();
            if (inIO->GetOwner() == inBeh && inBeh->GetOutput(inPos) == inIO &&
                strcmp(outBeh->GetName(), outBehName) == 0 && outBeh->GetInput(outPos) == outIO)
                return link;
        }

        return nullptr;
    }

    inline CKBehaviorLink *GetBehaviorLink(CKBehavior *script, const char *inBehName, const char *outBehName,
                                           int inPos = 0, int outPos = 0, CKBehaviorLink *previous = nullptr) {
        assert(script != nullptr);
        assert(inBehName != nullptr);
        assert(outBehName != nullptr);

        const int linkCount = script->GetSubBehaviorLinkCount();
        int i = 0;

        if (previous) {
            for (; i < linkCount; ++i) {
                CKBehaviorLink *link = script->GetSubBehaviorLink(i);
                if (link == previous) {
                    ++i;
                    break;
                }
            }
        }

        for (; i < linkCount; ++i) {
            CKBehaviorLink *link = script->GetSubBehaviorLink(i);
            CKBehaviorIO *inIO = link->GetInBehaviorIO();
            CKBehaviorIO *outIO = link->GetOutBehaviorIO();
            CKBehavior *inBeh = inIO->GetOwner();
            CKBehavior *outBeh = outIO->GetOwner();
            if (strcmp(inBeh->GetName(), inBehName) == 0 && inBeh->GetOutput(inPos) == inIO &&
                strcmp(outBeh->GetName(), outBehName) == 0 && outBeh->GetInput(outPos) == outIO)
                return link;
        }

        return nullptr;
    }

    inline void SetBehaviorLink(CKBehavior *script, CKBehaviorLink *link, CKBehavior *inBeh, CKBehavior *outBeh,
                                int inPos = 0, int outPos = 0) {
        assert(link != nullptr);
        assert(inBeh != nullptr);
        assert(outBeh != nullptr);

        link->SetInBehaviorIO(inBeh->GetOutput(inPos));
        link->SetOutBehaviorIO(outBeh->GetInput(outPos));
    }

    inline CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehaviorIO *out, bool destroy = false) {
        assert(script != nullptr);
        assert(in != nullptr);
        assert(out != nullptr);

        CKBehaviorLink *link = GetBehaviorLink(script, in, out);
        if (!link)
            return nullptr;

        script->RemoveSubBehaviorLink(link);
        if (destroy) {
            CKDestroyObject(link);
            return nullptr;
        }
        return link;
    }

    inline CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehavior *outBeh,
                                              int inPos = 0, int outPos = 0, bool destroy = false) {
        assert(script != nullptr);
        assert(inBeh != nullptr);
        assert(outBeh != nullptr);

        CKBehaviorLink *link = GetBehaviorLink(script, inBeh, outBeh, inPos, outPos);
        if (!link)
            return nullptr;

        script->RemoveSubBehaviorLink(link);
        if (destroy) {
            CKDestroyObject(link);
            return nullptr;
        }
        return link;
    }

    inline CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, const char *inBehName, CKBehavior *outBeh,
                                              int inPos = 0, int outPos = 0, bool destroy = false) {
        assert(script != nullptr);
        assert(inBehName != nullptr);
        assert(outBeh != nullptr);

        CKBehaviorLink *link = GetBehaviorLink(script, inBehName, outBeh, inPos, outPos);
        if (!link)
            return nullptr;

        script->RemoveSubBehaviorLink(link);
        if (destroy) {
            CKDestroyObject(link);
            return nullptr;
        }
        return link;
    }

    inline CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, CKBehavior *inBeh, const char *outBehName,
                                              int inPos = 0, int outPos = 0, bool destroy = false) {
        assert(script != nullptr);
        assert(inBeh != nullptr);
        assert(outBehName != nullptr);

        CKBehaviorLink *link = GetBehaviorLink(script, inBeh, outBehName, inPos, outPos);
        if (!link)
            return nullptr;

        script->RemoveSubBehaviorLink(link);
        if (destroy) {
            CKDestroyObject(link);
            return nullptr;
        }
        return link;
    }

    inline CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, const char *inBehName, const char *outBehName,
                                              int inPos = 0, int outPos = 0, bool destroy = false) {
        assert(script != nullptr);
        assert(inBehName != nullptr);
        assert(outBehName != nullptr);

        CKBehaviorLink *link = GetBehaviorLink(script, inBehName, outBehName, inPos, outPos);
        if (!link)
            return nullptr;

        script->RemoveSubBehaviorLink(link);
        if (destroy) {
            CKDestroyObject(link);
            return nullptr;
        }
        return link;
    }

    inline CKBehavior *CreateBehavior(CKBehavior *script, CKGUID guid, bool useTarget = false) {
        assert(script != nullptr);
        assert(guid.d1 != 0 && guid.d2 != 0);

        CKBehavior *behavior = (CKBehavior *) script->GetCKContext()->CreateObject(CKCID_BEHAVIOR);
        behavior->InitFromGuid(guid);
        if (useTarget)
            behavior->UseTarget();
        script->AddSubBehavior(behavior);
        return behavior;
    }

    inline CKBehavior *RemoveBehavior(CKBehavior *script, CKBehavior *beh, bool destroy = false) {
        assert(script);
        assert(beh);

        const int count = script->GetSubBehaviorLinkCount();
        for (int i = 0; i < count; i++) {
            CKBehaviorLink *link = script->GetSubBehaviorLink(i);
            if (link->GetInBehaviorIO()->GetOwner() == beh || link->GetOutBehaviorIO()->GetOwner() == beh) {
                script->RemoveSubBehaviorLink(link);
                if (destroy)
                    CKDestroyObject(link);
            }
        }

        beh->Activate(false);
        script->RemoveSubBehavior(beh);
        if (destroy) {
            CKDestroyObject(beh);
            return nullptr;
        }
        return beh;
    }

    inline CKBehavior *MoveBehavior(CKBehavior *script, CKBehavior *beh, CKBehavior *dest) {
        assert(script != nullptr);
        assert(beh != nullptr);
        assert(dest != nullptr);

        RemoveBehavior(script, beh, false);
        dest->AddSubBehavior(beh);
        return beh;
    }

    inline CKBehavior *CopyBehavior(CKBehavior *script, CKBehavior *beh, CKBehavior *dest) {
        assert(script != nullptr);
        assert(beh != nullptr);
        assert(dest != nullptr);

        CKBehavior *newBeh = (CKBehavior *) script->GetCKContext()->CopyObject(beh);
        MoveBehavior(script, newBeh, dest);
        dest->AddSubBehavior(newBeh);
        return newBeh;
    }

    inline CKBehavior *GetBehavior(const XObjectPointerArray &array, const char *name, CKBehavior *previous = nullptr) {
        assert(name != nullptr);

        CKBehavior *behavior = nullptr;
        CKObject **it = array.Begin();

        if (previous) {
            for (; it != array.End(); ++it) {
                if (*it == previous) {
                    ++it;
                    break;
                }
            }
        }

        for (; it != array.End(); ++it) {
            CKObject *obj = *it;
            if (obj->GetClassID() == CKCID_BEHAVIOR && strcmp(obj->GetName(), name) == 0) {
                behavior = (CKBehavior *) obj;
                break;
            }
        }

        return behavior;
    }

    inline CKBehavior *GetBehavior(CKBehavior *script, const char *name, CKBehavior *previous = nullptr) {
        assert(script != nullptr);
        assert(name != nullptr);

        CKBehavior *behavior = nullptr;
        const int count = script->GetSubBehaviorCount();
        int i = 0;

        if (previous) {
            for (; i < count; ++i) {
                CKBehavior *b = script->GetSubBehavior(i);
                if (b == previous) {
                    ++i;
                    break;
                }
            }
        }

        for (; i < count; ++i) {
            CKBehavior *b = script->GetSubBehavior(i);
            if (strcmp(b->GetName(), name) == 0) {
                behavior = b;
                break;
            }
        }

        return behavior;
    }

    inline CKBehavior *GetBehavior(CKBehavior *script, const char *name, const char *targetName, CKBehavior *previous = nullptr) {
        assert(script != nullptr);
        assert(name != nullptr);
        assert(targetName != nullptr);

        CKBehavior *behavior = nullptr;
        const int count = script->GetSubBehaviorCount();
        int i = 0;

        if (previous) {
            for (; i < count; ++i) {
                CKBehavior *b = script->GetSubBehavior(i);
                if (b == previous) {
                    ++i;
                    break;
                }
            }
        }

        for (; i < count; ++i) {
            CKBehavior *b = script->GetSubBehavior(i);
            if (strcmp(b->GetName(), name) == 0) {
                CKObject *target = b->GetTarget();
                if (target && strcmp(target->GetName(), targetName) == 0) {
                    behavior = b;
                    break;
                }
            }
        }

        return behavior;
    }

    inline XObjectPointerArray &GetBehaviors(CKBehavior *script, XObjectPointerArray &behaviors, const char *name, bool hierarchy = false) {
        const int count = script->GetSubBehaviorCount();
        for (int i = 0; i < count; ++i) {
            CKBehavior *b = script->GetSubBehavior(i);
            if (strcmp(b->GetName(), name) == 0)
                behaviors.PushBack(b);
            if (hierarchy && b->GetSubBehaviorCount() != 0)
                GetBehaviors(b, behaviors, name, hierarchy);
        }

        return behaviors;
    }

    inline CKParameterLocal *CreateLocalParameter(CKBehavior *script, const char *name = "",
                                                  CKGUID type = CKPGUID_STRING) {
        assert(script != nullptr);
        return script->CreateLocalParameter((CKSTRING) name, type);
    }

    template<typename T>
    CKParameterLocal *CreateLocalParameter(CKBehavior *script, const char *name, T value, CKGUID type) {
        CKParameterLocal *param = CreateLocalParameter(script, name, type);
        param->SetValue(&value, sizeof(T));
        return param;
    }

    template<>
    CKParameterLocal *CreateLocalParameter<CKObject *>(CKBehavior *script, const char *name, CKObject *value, CKGUID type) {
        return CreateLocalParameter(script, name, CKOBJID(value), type);
    }

    template<>
    CKParameterLocal *CreateLocalParameter<const char *>(CKBehavior *script, const char *name, const char *value, CKGUID type) {
        CKParameterLocal *param = CreateLocalParameter(script, name, type);
        param->SetStringValue((CKSTRING) value);
        return param;
    }

    inline CKParameterLocal *CreateLocalParameter(CKBehavior *script, const char *name = "", const char *value = "") {
        return CreateLocalParameter(script, name, value, CKPGUID_STRING);
    }

    inline CKParameterLocal *GenerateInputParameter(CKBehavior *script, CKBehavior *beh, int inPos) {
        assert(script != nullptr);
        assert(beh != nullptr);

        if (inPos < 0 || inPos > beh->GetInputParameterCount())
            return nullptr;

        CKParameterIn *inParam = beh->GetInputParameter(inPos);
        CKParameterManager *pm = beh->GetCKContext()->GetParameterManager();
        CKParameterLocal *param = CreateLocalParameter(script, inParam->GetName(),
                                                       pm->ParameterTypeToGuid(inParam->GetType()));
        inParam->SetDirectSource(param);
        return param;
    }

    template<typename T>
    CKParameterLocal *GenerateInputParameter(CKBehavior *script, CKBehavior *beh, int inPos, T value) {
        CKParameterLocal *param = GenerateInputParameter(script, beh, inPos);
        param->SetValue(&value, sizeof(T));
        return param;
    }

    template<>
    CKParameterLocal *GenerateInputParameter<CKObject *>(CKBehavior *script, CKBehavior *beh, int inPos,
                                                         CKObject *value) {
        return GenerateInputParameter(script, beh, inPos, CKOBJID(value));
    }

    template<>
    CKParameterLocal *GenerateInputParameter<const char *>(CKBehavior *script, CKBehavior *beh, int inPos,
                                                           const char *value) {
        CKParameterLocal *param = GenerateInputParameter(script, beh, inPos);
        param->SetValue((CKSTRING) value);
        return param;
    }

    template<>
    CKParameterLocal *GenerateInputParameter<CKSTRING>(CKBehavior *script, CKBehavior *beh, int inPos, CKSTRING value) {
        CKParameterLocal *param = GenerateInputParameter(script, beh, inPos);
        param->SetValue(value);
        return param;
    }

    inline CKParameterLocal *GenerateTargetParameter(CKBehavior *script, CKBehavior *beh) {
        assert(script != nullptr);
        assert(beh != nullptr);

        CKParameterIn *targetParam = beh->GetTargetParameter();
        CKParameterManager *pm = beh->GetCKContext()->GetParameterManager();
        CKParameterLocal *param = CreateLocalParameter(script, targetParam->GetName(), pm->ParameterTypeToGuid(targetParam->GetType()));
        targetParam->SetDirectSource(param);
        return param;
    }

    template<typename T>
    CKParameterLocal *GenerateTargetParameter(CKBehavior *script, CKBehavior *beh, T value) {
        CKParameterLocal *param = GenerateTargetParameter(script, beh);
        param->SetValue(&value, sizeof(T));
        return param;
    }

    template<>
    CKParameterLocal *GenerateTargetParameter<CKObject *>(CKBehavior *script, CKBehavior *beh, CKObject *value) {
        return GenerateTargetParameter(script, beh, CKOBJID(value));
    }

    template<>
    CKParameterLocal *GenerateTargetParameter<const char *>(CKBehavior *script, CKBehavior *beh, const char *value) {
        CKParameterLocal *param = GenerateTargetParameter(script, beh);
        param->SetValue(value);
        return param;
    }

    template<>
    CKParameterLocal *GenerateTargetParameter<CKSTRING>(CKBehavior *script, CKBehavior *beh, CKSTRING value) {
        CKParameterLocal *param = GenerateTargetParameter(script, beh);
        param->SetValue(value);
        return param;
    }

    template<typename T>
    void SetParameterValue(CKParameter *param, T value) {
        param->SetValue(&value, sizeof(T));
    }

    inline void SetParameterObject(CKParameter *param, CKObject *value) {
        CK_ID obj = CKOBJID(value);
        SetParameterValue(param, obj);
    }

    inline void SetParameterString(CKParameter *param, const char *value) {
        param->SetStringValue((CKSTRING) value);
    }

    template<typename T>
    void SetTargetParameterValue(CKBehavior *beh, T value) {
        assert(beh != nullptr);
        CKParameter *param = beh->GetTargetParameter()->GetDirectSource();
        param->SetValue(&value, sizeof(T));
    }

    inline void SetTargetParameterObject(CKBehavior *beh, CKObject *value) {
        SetTargetParameterValue(beh, CKOBJID(value));
    }

    inline void SetTargetParameterString(CKBehavior *beh, const char *value) {
        assert(beh != nullptr);
        CKParameter *param = beh->GetTargetParameter()->GetDirectSource();
        param->SetValue((CKSTRING) value);
    }

    template<typename T>
    void SetInputParameterValue(CKBehavior *beh, int inPos, T value) {
        assert(beh != nullptr);
        if (0 <= inPos && inPos < beh->GetInputParameterCount()) {
            CKParameter *param = beh->GetInputParameter(inPos)->GetDirectSource();
            param->SetValue(&value, sizeof(T));
        }
    }

    inline void SetInputParameterObject(CKBehavior *beh, int inPos, CKObject *value) {
        SetInputParameterValue(beh, inPos, CKOBJID(value));
    }

    inline void SetInputParameterString(CKBehavior *beh, int inPos, const char *value) {
        assert(beh != nullptr);
        if (0 <= inPos && inPos < beh->GetInputParameterCount()) {
            CKParameter *param = beh->GetInputParameter(inPos)->GetDirectSource();
            param->SetValue((CKSTRING) value);
        }
    }

    template<typename T>
    void SetLocalParameterValue(CKBehavior *beh, int pos, T value) {
        assert(beh != nullptr);
        if (0 <= pos && pos < beh->GetLocalParameterCount()) {
            CKParameterLocal *param = beh->GetLocalParameter(pos);
            param->SetValue(&value, sizeof(T));
        }
    }

    inline void SetLocalParameterObject(CKBehavior *beh, int pos, CKObject *value) {
        SetLocalParameterValue(beh, pos, CKOBJID(value));
    }

    inline void SetLocalParameterString(CKBehavior *beh, int pos, const char *value) {
        assert(beh != nullptr);
        if (0 <= pos && pos < beh->GetLocalParameterCount()) {
            CKParameterLocal *param = beh->GetLocalParameter(pos);
            param->SetValue((CKSTRING) value);
        }
    }

    template<typename T>
    T GetParameterValue(CKParameter *param) {
        T res;
        param->GetValue(&res);
        return res;
    }

    inline CKObject *GetParameterObject(CKParameter *param) {
        return param->GetValueObject();
    }

    inline const char *GetParameterString(CKParameter *param) {
        return (const char *) param->GetReadDataPtr();
    }

    template<typename T>
    T GetInputParameterValue(CKBehavior *beh, int inPos) {
        assert(beh != nullptr);
        if (0 <= inPos && inPos < beh->GetInputParameterCount()) {
            T res;
            beh->GetInputParameter(inPos)->GetValue(&res);
            return res;
        }
        return T();
    }

    inline CKObject *GetInputParameterObject(CKBehavior *beh, int inPos) {
        assert(beh != nullptr);
        if (0 <= inPos && inPos < beh->GetInputParameterCount()) {
            CKParameter *param = beh->GetInputParameter(inPos)->GetDirectSource();
            return param->GetValueObject();
        }
        return nullptr;
    }

    inline const char *GetInputParameterString(CKBehavior *beh, int inPos) {
        assert(beh != nullptr);
        if (0 <= inPos && inPos < beh->GetInputParameterCount()) {
            return (const char *) beh->GetInputParameter(inPos)->GetReadDataPtr();
        }
        return nullptr;
    }

    template<typename T>
    T GetOutputParameterValue(CKBehavior *beh, int outPos) {
        assert(beh != nullptr);
        if (0 <= outPos && outPos < beh->GetOutputParameterCount()) {
            T res;
            beh->GetOutputParameter(outPos)->GetValue(&res);
            return res;
        }
        return T();
    }

    inline CKObject *GetOutputParameterObject(CKBehavior *beh, int outPos) {
        assert(beh != nullptr);
        if (0 <= outPos && outPos < beh->GetOutputParameterCount()) {
            return beh->GetOutputParameter(outPos)->GetValueObject();
        }
        return nullptr;
    }

    inline const char *GetOutputParameterString(CKBehavior *beh, int outPos) {
        assert(beh != nullptr);
        if (0 <= outPos && outPos < beh->GetOutputParameterCount()) {
            return (const char *) beh->GetOutputParameter(outPos)->GetReadDataPtr();
        }
        return nullptr;
    }

    template<typename T>
    T GetLocalParameterValue(CKBehavior *beh, int pos) {
        assert(beh != nullptr);
        if (0 <= pos && pos < beh->GetLocalParameterCount()) {
            T res;
            beh->GetLocalParameter(pos)->GetValue(&res);
            return res;
        }
        return T();
    }

    inline CKObject *GetLocalParameterObject(CKBehavior *beh, int pos) {
        assert(beh != nullptr);
        if (0 <= pos && pos < beh->GetLocalParameterCount()) {
            CKParameterLocal *param = beh->GetLocalParameter(pos);
            return param->GetValueObject();
        }
        return nullptr;
    }

    inline const char *GetLocalParameterString(CKBehavior *beh, int pos) {
        assert(beh != nullptr);
        if (0 <= pos && pos < beh->GetLocalParameterCount()) {
            return (const char *) beh->GetLocalParameter(pos)->GetReadDataPtr();
        }
        return nullptr;
    }
}

#endif // PLAYER_SCRIPTUTILS_H
