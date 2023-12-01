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


#include "threadTracer.h"

static FILE* out;
static jrawMonitorID vmtrace_lock;
static jlong start_time;

// 创建一个数据结构用于保存线程信息
std::vector<jvmtiThreadInfo> threads_info_vector;

static void trace(jvmtiEnv* jvmti) {
    // TODO:
    jint* threads_count_ptr;
    jthread** threads_ptr;    
    // 获取所有线程
    jvmtiError error = jvmti->GetAllThreads(threads_count_ptr, threads_ptr);
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Error in GetAllThreads: " << error << std::endl;
        
        if (threads_count_ptr == NULL) {
            std::cerr << "threads_count_ptr" << std::endl;
        }
        if (threads_ptr == NULL) {
            std::cerr << "threads_ptr" << std::endl;
        }

        // 处理错误
        return;
    }

    jint threads_count = *threads_count_ptr;
    printf("Number of threads: %d\n", threads_count);
    jthread* threads = *threads_ptr;
    
    // 遍历每个线程并获取详细信息
    for (int i = 0; i < threads_count; i++) {
        jvmtiThreadInfo info;
        // 清空结构体
        memset(&info, 0, sizeof(jvmtiThreadInfo));
        
        // 获取线程详细信息
        jvmti->GetThreadInfo(threads[i], &info);
        
        // 打印线程信息
        // std::cout << "-----------------------" << std::endl;
        // std::cout << "Name:                 " << info.name << std::endl;
        // std::cout << "priority:             " << info.priority << std::endl;
        // std::cout << "is_daemon:            " << info.is_daemon << std::endl;
        // std::cout << "thread_group:         " << info.thread_group << std::endl;
        // std::cout << "context_class_loader: " << info.context_class_loader << std::endl;
        
        // // 释放 GetThreadInfo 分配的资源
        // (*jvmti)->Deallocate(jvmti, (unsigned char*)info.name);
        // (*jvmti)->Deallocate(jvmti, (unsigned char*)info.thread_group);
        // (*jvmti)->Deallocate(jvmti, (unsigned char*)info.context_class_loader);
    }
    
    // // 释放 GetAllThreads 分配的资源
    // (*jvmti)->Deallocate(jvmti, (unsigned char*)threads);
    



    // 处理保存的线程信息，这里可以根据需求进行进一步处理或打印
    // for (const auto& info : threads_info_vector) {
    //     std::cout << "-----------------------" << std::endl;
    //     std::cout << "Name:                 " << info.name << std::endl;
    //     std::cout << "priority:             " << info.priority << std::endl;
    //     std::cout << "is_daemon:            " << info.is_daemon << std::endl;
    //     std::cout << "thread_group:         " << info.thread_group << std::endl;
    //     std::cout << "context_class_loader: " << info.context_class_loader << std::endl;

    // }

    
    // char buf[1024];
    // va_list args;
    // va_start(args, fmt);
    // vsnprintf(buf, sizeof(buf), fmt, args);
    // va_end(args);


    // fprintf(out, "[%.5f] %s\n", (current_time - start_time) / 1000000000.0, buf);
}



// void JNICALL VMStart(jvmtiEnv* jvmti, JNIEnv* env) {
//     trace(jvmti, "VM started");
// }

// void JNICALL VMInit(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
//     trace(jvmti, "VM initialized");
// }

// void JNICALL VMDeath(jvmtiEnv* jvmti, JNIEnv* env) {
//     trace(jvmti, "VM destroyed");
// }

void JNICALL ThreadStart(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
    // TODO:
    trace(jvmti);
}

// void JNICALL ThreadEnd(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
//     ThreadName tn(jvmti, thread);
//     trace(jvmti, "Thread finished: %s", tn.name());
// }

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

    // trace(jvmti);

    jvmtiCapabilities capabilities = {0};
    capabilities.can_generate_all_class_hook_events = 1;
    // capabilities.can_generate_compiled_method_load_events = 1;
    // capabilities.can_generate_garbage_collection_events = 1;
    jvmti->AddCapabilities(&capabilities);

    jvmtiEventCallbacks callbacks = {0};
    // callbacks.VMStart = VMStart;
    // callbacks.VMInit = VMInit;
    // callbacks.VMDeath = VMDeath;
    callbacks.ThreadStart = ThreadStart;
    // callbacks.ThreadEnd = ThreadEnd;

    jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));

    // jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_START, NULL);
    // jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);
    // jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_START, NULL);
    // jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_END, NULL);

    return 0;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm) {
    if (out != NULL && out != stderr) {
        fclose(out);
    }
}
