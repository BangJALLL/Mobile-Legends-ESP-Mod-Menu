#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu.h"
#include "Unity/Color.h"
#include "Unity/Vector2.h"
#include "Unity/Vector3.h"
#include "Unity/MonoList.h"
#include "Includes/ESPOverlay.h"
#include "Structs/Tools.h"
#include "Structs/fake_dlfcn.h"
#include "Structs/Il2Cpp.h"

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

bool ESP, ESPLine;
void *BattleManager;

Vector3 (*WorldToScreenPoint)(void *instance, Vector3 position);
void *(*get_main)(void *instance);
void (*SetResolution)(int width, int height, bool fullscreen);
Vector3 (*get_Position)(void *instance);
bool (*GetHPEmpty)(void *instance);

//Target lib here
#define targetLibName OBFUSCATE("liblogic.so")

void DrawESP(ESPOverlay esp, int screenWidth, int screenHeight) {
   if (ESP) {
       if (BattleManager != NULL) {
            monoList<void **> *m_ShowPlayers = *(monoList<void **> **) ((long) BattleManager + Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_ShowPlayers"));
            if (m_ShowPlayers != NULL) {
                for (int i = 0; i < m_ShowPlayers->getSize(); i++) {
                    void *obj = m_ShowPlayers->getItems()[i];
                    if (obj != NULL) {
                        void *_logicFighter = *(void **) ((long) obj + Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "_logicFighter"));
                        if (_logicFighter != NULL) {
                            bool isDead = GetHPEmpty(_logicFighter);
                            if (!isDead) {
                                Vector3 objPos = WorldToScreenPoint(get_main(NULL), get_Position(_logicFighter));
                                if (ESPLine) {
                                    esp.drawLine(Color::White(), 1, Vector2(screenWidth / 2, screenHeight / 2), Vector2(objPos.X, screenHeight - objPos.Y));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void (*old_Update)(void *instance);
void Update(void *instance) {
    if (instance != NULL) {
        BattleManager = instance;
    }
    old_Update(instance);
}

extern "C"
JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_DrawOn(JNIEnv *env, jclass type, jobject espView, jobject canvas) {
    ESPOverlay espOverlay = ESPOverlay(env, espView, canvas);
    if (espOverlay.isValid()) {
        DrawESP(espOverlay, espOverlay.width(), espOverlay.height());
    }
}

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libYOURNAME.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

    MSHookFunction((void *) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "BattleManager", "Update", 0), (void *) Update, (void **) &old_Update);

    WorldToScreenPoint = (Vector3 (*)(void *, Vector3)) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreen", 1);
    get_main = (void *(*)(void *)) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "get_main", 0);
    SetResolution = (void (*)(int, int, bool)) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Screen", "SetResolution", 3);
    get_Position = (Vector3 (*)(void *)) Il2CppGetMethodOffset("Assembly-CSharp.dll", "Battle", "LogicFighter", "get_Position", 0);
    GetHPEmpty = (bool (*)(void *)) Il2CppGetMethodOffset("Assembly-CSharp.dll", "Battle", "LogicFighter", "GetHPEmpty", 0);

    LOGI(OBFUSCATE("Done"));

    return NULL;
}

//JNI calls
extern "C" {

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

JNIEXPORT jobjectArray
JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    //Toasts added here so it's harder to remove it
    MakeToast(env, context, OBFUSCATE("Modded by LGL"), Toast::LENGTH_LONG);

    const char *features[] = {
            OBFUSCATE("ButtonOnOff_Enable ESP"),
            OBFUSCATE("Toggle_ESP Line")
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    pthread_t ptid;
    pthread_create(&ptid, NULL, antiLeech, NULL);

    return (ret);
}

JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_Preferences_Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 0:
            ESP = boolean;
            break;
        case 1:
            ESPLine = boolean;
            break;
    }
}
}

//No need to use JNI_OnLoad, since we don't use JNIEnv
//We do this to hide OnLoad from disassembler
__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

/*
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);
    return JNI_VERSION_1_6;
}
 */
