#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <atomic>
#include <mutex>
#include <thread>

namespace {
constexpr char kTag[] = "NewShoesAndroid";

std::mutex g_mutex;
ANativeWindow* g_window = nullptr;
EGLDisplay g_display = EGL_NO_DISPLAY;
EGLSurface g_surface = EGL_NO_SURFACE;
EGLContext g_context = EGL_NO_CONTEXT;
std::thread g_thread;
std::atomic<bool> g_running{false};
std::atomic<bool> g_paused{false};
int g_width = 1;
int g_height = 1;

void log(const char* message) {
    __android_log_print(ANDROID_LOG_INFO, kTag, "%s", message);
}

bool make_context_locked() {
    if (!g_window || g_display != EGL_NO_DISPLAY) return g_display != EGL_NO_DISPLAY;

    g_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_display == EGL_NO_DISPLAY) return false;
    if (!eglInitialize(g_display, nullptr, nullptr)) return false;

    const EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    EGLConfig config = nullptr;
    EGLint count = 0;
    if (!eglChooseConfig(g_display, config_attribs, &config, 1, &count) || count == 0) return false;

    const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    g_context = eglCreateContext(g_display, config, EGL_NO_CONTEXT, context_attribs);
    if (g_context == EGL_NO_CONTEXT) return false;

    g_surface = eglCreateWindowSurface(g_display, config, g_window, nullptr);
    if (g_surface == EGL_NO_SURFACE) return false;

    if (!eglMakeCurrent(g_display, g_surface, g_surface, g_context)) return false;
    glViewport(0, 0, g_width, g_height);
    log("Android native graphics heartbeat initialized");
    return true;
}

void destroy_context_locked() {
    if (g_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if (g_surface != EGL_NO_SURFACE) eglDestroySurface(g_display, g_surface);
    if (g_context != EGL_NO_CONTEXT) eglDestroyContext(g_display, g_context);
    if (g_display != EGL_NO_DISPLAY) eglTerminate(g_display);
    g_display = EGL_NO_DISPLAY;
    g_surface = EGL_NO_SURFACE;
    g_context = EGL_NO_CONTEXT;
}

void render_loop() {
    while (g_running.load()) {
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (!g_paused.load() && g_window && make_context_locked()) {
                glClearColor(0.015f, 0.025f, 0.05f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                eglSwapBuffers(g_display, g_surface);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    destroy_context_locked();
}
}

extern "C" JNIEXPORT void JNICALL
Java_com_newshoes_android_NativeBridge_onSurfaceCreated(JNIEnv* env, jclass, jobject surface) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_window) ANativeWindow_release(g_window);
    g_window = ANativeWindow_fromSurface(env, surface);
    g_paused.store(false);
    if (!g_running.exchange(true)) {
        g_thread = std::thread(render_loop);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_newshoes_android_NativeBridge_onSurfaceChanged(JNIEnv*, jclass, jint width, jint height) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_width = width > 0 ? width : 1;
    g_height = height > 0 ? height : 1;
    if (g_display != EGL_NO_DISPLAY && g_surface != EGL_NO_SURFACE) {
        glViewport(0, 0, g_width, g_height);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_newshoes_android_NativeBridge_onSurfaceDestroyed(JNIEnv*, jclass) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_paused.store(true);
    destroy_context_locked();
    if (g_window) {
        ANativeWindow_release(g_window);
        g_window = nullptr;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_newshoes_android_NativeBridge_onTouch(JNIEnv*, jclass, jint action, jfloat x, jfloat y) {
    (void)action;
    (void)x;
    (void)y;
    // Reserved for the first real GameEngine input adapter.
}

extern "C" JNIEXPORT void JNICALL
Java_com_newshoes_android_NativeBridge_onHostResume(JNIEnv*, jclass) {
    g_paused.store(false);
}

extern "C" JNIEXPORT void JNICALL
Java_com_newshoes_android_NativeBridge_onHostPause(JNIEnv*, jclass) {
    g_paused.store(true);
}

extern "C" JNIEXPORT void JNICALL
Java_com_newshoes_android_NativeBridge_shutdown(JNIEnv*, jclass) {
    g_running.store(false);
    if (g_thread.joinable()) g_thread.join();
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_window) {
        ANativeWindow_release(g_window);
        g_window = nullptr;
    }
}
