#include <jvmti.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>


// class ThreadTracer {
//     private:
    
//     public:
//             const char* title() {
//         return "Lock profile";
//     }

//     const char* units() {
//         return "ns";
//     }

//     Error start(Arguments& args);
//     void stop();

//     static void JNICALL MonitorContendedEnter(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object);
//     static void JNICALL MonitorContendedEntered(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object);
// };