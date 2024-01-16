#include <jni.h>
#include <android/log.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <thread>
#include <chrono>

#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <fcntl.h>

#define tag "demo"

static int child_pid;

static const char* blacklistedMountPaths[] = {
        "magisk",
        "core/mirror",
        "core/img"
};


class AntiDebugManager {
public:

    static void portScan() {

        __android_log_print(ANDROID_LOG_DEBUG, tag, "Portscan running!");

        struct sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        inet_aton("127.0.0.1", &(sa.sin_addr));

        int sock;
        char res[7];
        int ret;
        int i;

        while (1) {

            for(i = 0 ; i <= 65535 ; i++) {

                sock = socket(AF_INET , SOCK_STREAM , 0);
                sa.sin_port = htons(i);

                if (connect(sock , (struct sockaddr*)&sa , sizeof sa) != -1) {
                    memset(res, 0 , 7);

                    send(sock, "\x00", 1, NULL);
                    send(sock, "AUTH\r\n", 6, NULL);

                    usleep(100);

                    if ((ret = recv(sock, res, 6, MSG_DONTWAIT)) != -1) {
                        if (strcmp(res, "REJECT") == 0) {
                            __android_log_print(ANDROID_LOG_DEBUG, tag, "Portscan detected frida, killing!");
                        }
                    }
                }

                close(sock);
            }
        }};
    

        static void  portCheck() {
            __android_log_print(ANDROID_LOG_DEBUG, tag, "Running port check");
            struct sockaddr_in sa;

            memset(&sa, 0, sizeof(sa));
            sa.sin_family = AF_INET;
            sa.sin_port = htons(27042);
            inet_aton("127.0.0.1", &(sa.sin_addr));

            int sock = socket(AF_INET , SOCK_STREAM , 0);

            if (connect(sock , (struct sockaddr*)&sa , sizeof sa) != -1) {
                __android_log_print(ANDROID_LOG_DEBUG, tag, "Port detected! Killing the app");
                kill(getpid(), SIGKILL);
            }
        };

        static void libCheck(){

        __android_log_print(ANDROID_LOG_DEBUG, tag, "Running lib check");

        char line[512];
        FILE* fp;

        fp = fopen("/proc/self/maps", "r");

        if (fp) {
            while (fgets(line, 512, fp)) {
                if (strstr(line, "frida")) {
                    __android_log_print(ANDROID_LOG_DEBUG, tag, "Lib detected, killing the app");
                    kill(getpid(), SIGKILL);
                }
            }

            fclose(fp);

            } else {
            }
        }

    static void *monitor_pid(void *) {
        int status;

        waitpid(child_pid, &status, 0);
        _exit(0);
    }

    static void antiDebug() {
        __android_log_print(ANDROID_LOG_DEBUG, tag, "Hooking our debugger!");

        child_pid = fork();

        if (child_pid == 0) {
            int ppid = getppid();
            int status;

            if (ptrace(PTRACE_ATTACH, ppid, NULL, NULL) == 0) {
                waitpid(ppid, &status, 0);

                ptrace(PTRACE_CONT, ppid, NULL, NULL);

                while (waitpid(ppid, &status, 0)) {
                    if (WIFSTOPPED(status)) {
                        ptrace(PTRACE_CONT, ppid, NULL, NULL);
                    } else {
                        _exit(0);
                    }
                }
            }
        } else {
            pthread_t t;
            pthread_create(&t, NULL, monitor_pid, (void *)NULL);
        }
    }



    static bool is_mountpaths_detected() {
        int len = sizeof(blacklistedMountPaths) / sizeof(blacklistedMountPaths[0]);
        bool bRet = false;

        FILE* fp = fopen("/proc/self/mounts", "r");
        if (fp == NULL) {
            return bRet;
        }

        fseek(fp, 0L, SEEK_END);
        long size = ftell(fp);
        __android_log_print(ANDROID_LOG_INFO, tag, "Opening Mount file size: %ld", size);
        if (size == 0)
            size = 20000;
        fseek(fp, 0L, SEEK_SET);

        char* buffer = (char*)calloc(size, sizeof(char));
        if (buffer == NULL) {
            fclose(fp);
            return bRet;
        }

        size_t read = fread(buffer, 1, size, fp);
        if (read == 0) {
            fclose(fp);
            free(buffer);
            return bRet;
        }

        int count = 0;
        for (int i = 0; i < len; i++) {
            char* rem = strstr(buffer, blacklistedMountPaths[i]);
            if (rem != NULL) {
                count++;
                __android_log_print(ANDROID_LOG_INFO, tag, "Found Mount Path: %s", blacklistedMountPaths[i]);
                break;
            }
        }

        if (count > 0)
            bRet = true;

        fclose(fp);
        free(buffer);

        return bRet;
    }



    static void X() {
         if (is_mountpaths_detected()){
             kill(getpid(), SIGKILL);
         }

        AntiDebugManager::libCheck();
        AntiDebugManager::portCheck();
        AntiDebugManager::portScan();
        
    }

};

extern "C" JNIEXPORT void JNICALL
Java_com_tcn_demo_AntiDebugManager_X(JNIEnv *env, jobject /* this */) {
AntiDebugManager::X();
};

extern "C" JNIEXPORT void JNICALL
Java_com_tcn_demo_AntiDebugManager_antiDebug(JNIEnv *env, jobject /* this */) {
AntiDebugManager::antiDebug();
};