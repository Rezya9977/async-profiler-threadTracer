/*
 * Copyright 2019 Odnoklassniki Ltd, Mail.Ru Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jvmti.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "trie.cpp"

static FILE* out;
static jrawMonitorID vmtrace_lock;
static jlong start_time;
// 创建一个数据结构用于保存线程名
Trie threads_trie;
static void groupbyThread() {

}

static void statisticThreads(jvmtiEnv* jvmti) {
    jint threads_count;
    jthread* threads;    
    // 获取所有线程
    jvmtiError error = (jvmti)->GetAllThreads(&threads_count, &threads);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Error in GetAllThreads: " << error << std::endl;
        // 处理错误
        return;
    }
    for (int i = 0; i < threads_count; i++) {
        jvmtiThreadInfo info;
        jvmtiError error = jvmti->GetThreadInfo(threads[i], &info);
        if (error != JVMTI_ERROR_NONE) {
            std::cerr << "Error in GetAllThreads: " << error << std::endl;
            // 处理错误
            return;
        }
        threads_trie.insert(info.name);
        std::cout << "插入*threads->name:" << info.name << std::endl;
    }
    groupbyThread();
}

static void trace(jvmtiEnv* jvmti, const char* fmt, ...) {
    jlong current_time;
    jvmti->GetTime(&current_time);

    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    statisticThreads(jvmti);

    jvmti->RawMonitorEnter(vmtrace_lock);

    fprintf(out, "[%.5f] %s\n", (current_time - start_time) / 1000000000.0, buf);
    
    jvmti->RawMonitorExit(vmtrace_lock);
}

class ThreadName {
  private:
    jvmtiEnv* _jvmti;
    char* _name;

  public:
    ThreadName(jvmtiEnv* jvmti, jthread thread) : _jvmti(jvmti), _name(NULL) {
        jvmtiThreadInfo info;
        _name = _jvmti->GetThreadInfo(thread, &info) == 0 ? info.name : NULL;
    }

    ~ThreadName() {
        _jvmti->Deallocate((unsigned char*) _name);
    }

    char* name() {
        return _name;
    }
};


void JNICALL VMStart(jvmtiEnv* jvmti, JNIEnv* env) {
    trace(jvmti, "VM started");
}

void JNICALL VMInit(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
    trace(jvmti, "VM initialized");
}

void JNICALL VMDeath(jvmtiEnv* jvmti, JNIEnv* env) {
    trace(jvmti, "VM destroyed");
}


void JNICALL DynamicCodeGenerated(jvmtiEnv* jvmti, const char* name,
                                  const void* address, jint length) {
    trace(jvmti, "Dynamic code generated: %s (%d bytes)", name, length);
}

void JNICALL ThreadStart(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
    ThreadName tn(jvmti, thread);
    trace(jvmti, "Thread started: %s", tn.name());
}

void JNICALL ThreadEnd(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
    ThreadName tn(jvmti, thread);
    trace(jvmti, "Thread finished: %s", tn.name());
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved) {
    if (options == NULL || !options[0]) {
        out = stderr;
    } else if ((out = fopen(options, "w")) == NULL) {
        fprintf(stderr, "Cannot open output file: %s\n", options);
        return 1;
    }

    jvmtiEnv* jvmti;
    vm->GetEnv((void**) &jvmti, JVMTI_VERSION_1_0);

    jvmti->CreateRawMonitor("vmtrace_lock", &vmtrace_lock);
    jvmti->GetTime(&start_time);

    trace(jvmti, "VMTrace started");

    jvmtiCapabilities capabilities = {0};
    capabilities.can_generate_all_class_hook_events = 1;
    capabilities.can_generate_compiled_method_load_events = 1;
    capabilities.can_generate_garbage_collection_events = 1;
    jvmti->AddCapabilities(&capabilities);

    jvmtiEventCallbacks callbacks = {0};
    callbacks.VMStart = VMStart;
    callbacks.VMInit = VMInit;
    callbacks.VMDeath = VMDeath;
    callbacks.ThreadStart = ThreadStart;
    callbacks.ThreadEnd = ThreadEnd;
    jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));

    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_START, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_START, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_END, NULL);

    return 0;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm) {
    if (out != NULL && out != stderr) {
        fclose(out);
    }
}
