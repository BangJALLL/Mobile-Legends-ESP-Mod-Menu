#ifndef ESPOverlay_H
#define ESPOverlay_H

#include <jni.h>

class ESPOverlay {
    private:
    JNIEnv *_env;
    jobject _cvsView;
    jobject _cvs;
    public:
    ESPOverlay() {
        _env = nullptr;
        _cvsView = nullptr;
        _csv = nullptr;
    }

    ESPOverlay(JNIEnv *env, jobject cvsView, jobject cvs) {
        this->_env = env;
        this->_cvsView = cvsView;
        this->_cvs = cvs;
    }

    bool isValid() const {
        return (_env != nullptr && _cvsView != nullptr && _cvs != nullptr);
    }

    int width() const {
        if (isValid()) {
            jclass canvas = _env->GetObjectClass(_cvs);
            jmethodID widthID = _env->GetMethodID(canvas, "getWidth", "()I");
            return _env->CallIntMethod(_cvs, widthID);
        }
        return 0;
    }

    int height() const {
        if (isValid()) {
            jclass canvas = _env->GetObjectClass(_cvs);
            jmethodID heightID = _env->GetMethodID(canvas, "getHeight", "()I");
            return _env->CallIntMethod(_cvs, heightID);
        }
        return 0;
    }

    void drawLine(Color color, float thickness, Vector2 start, Vector2 end) {
        if (isValid()) {
            jclass canvasView = _env->GetObjectClass(_cvsView);
            jmethodID drawline = _env->GetMethodID(canvasView, "DrawLine", "(Landroid/graphics/Canvas;IIIIFFFFF)V");
            _env->CallVoidMethod(_cvsView, drawline, _cvs, (int) color.r, (int) color.g, (int) color.b, (int) color.a, thickness, start.X, start.Y, end.X, end.Y);
        }
    }
};
#endif
