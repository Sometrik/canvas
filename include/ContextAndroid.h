#ifndef _CANVAS_CONTEXTANDROID_H_
#define _CANVAS_CONTEXTANDROID_H_

#include "Context.h"

#include <sstream>
#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>

namespace canvas {
class AndroidCache {
public:
  AndroidCache(JNIEnv * _env, jobject _assetManager) {
    _env->GetJavaVM(&javaVM);
    assetManager = _env->NewGlobalRef(_assetManager);
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache created");

    initJava();
  }

  ~AndroidCache() {
    JNIEnv * env = getJNIEnv();
//    env->DeleteGlobalRef(assetManager);
    if (javaInitialized) {
//      env->DeleteGlobalRef(typefaceClass);
//      env->DeleteGlobalRef(rectFClass);
//      env->DeleteGlobalRef(rectClass);
//      env->DeleteGlobalRef(canvasClass);
//      env->DeleteGlobalRef(paintClass);
//      env->DeleteGlobalRef(pathClass);
//      env->DeleteGlobalRef(bitmapClass);
//      env->DeleteGlobalRef(assetManagerClass);
//      env->DeleteGlobalRef(factoryClass);
//      env->DeleteGlobalRef(paintStyleClass);
//      env->DeleteGlobalRef(alignClass);
//      env->DeleteGlobalRef(bitmapConfigClass);
//      env->DeleteGlobalRef(bitmapOptionsClass);
//      env->DeleteGlobalRef(fileClass);
//      env->DeleteGlobalRef(fileInputStreamClass);
    }
  }

  void initJava() {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache initJava called");

    if (!javaInitialized) {
      javaInitialized = true;

      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache java is being initialized");
      JNIEnv * env = getJNIEnv();

      typefaceClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Typeface"));
      canvasClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Canvas"));
      assetManagerClass = (jclass) env->NewGlobalRef(env->FindClass("android/content/res/AssetManager"));
      factoryClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/BitmapFactory"));
      bitmapClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Bitmap"));
      paintClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Paint"));
      pathClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Path"));
      paintStyleClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Paint$Style"));
      alignClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Paint$Align"));
      bitmapConfigClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Bitmap$Config"));
      field_argb_8888 = env->GetStaticFieldID(bitmapConfigClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
      field_rgb_565 = env->GetStaticFieldID(bitmapConfigClass, "RGB_565", "Landroid/graphics/Bitmap$Config;");
      field_alpha_8 = env->GetStaticFieldID(bitmapConfigClass, "ALPHA_8", "Landroid/graphics/Bitmap$Config;");
      rectFClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/RectF"));
      rectClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Rect"));
      bitmapOptionsClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/BitmapFactory$Options"));
      fileClass = (jclass) env->NewGlobalRef(env->FindClass("java/io/File"));
      fileInputStreamClass = (jclass) env->NewGlobalRef(env->FindClass("java/io/FileInputStream"));

      measureAscentMethod = env->GetMethodID(paintClass, "ascent", "()F");
      measureDescentMethod = env->GetMethodID(paintClass, "descent", "()F");
      measureTextMethod = env->GetMethodID(paintClass, "measureText", "(Ljava/lang/String;)F");
      setAlphaMethod = env->GetMethodID(paintClass, "setAlpha", "(I)V");
      setTypefaceMethod = env->GetMethodID(paintClass, "setTypeface", "(Landroid/graphics/Typeface;)Landroid/graphics/Typeface;");
      typefaceCreator = env->GetStaticMethodID(typefaceClass, "create", "(Ljava/lang/String;I)Landroid/graphics/Typeface;");
      managerOpenMethod = env->GetMethodID(assetManagerClass, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
      bitmapCreateMethod = env->GetStaticMethodID(bitmapClass, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
      bitmapCreateMethod2 = env->GetStaticMethodID(bitmapClass, "createBitmap", "([IIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
      textAlignMethod = env->GetMethodID(paintClass, "setTextAlign", "(Landroid/graphics/Paint$Align;)V");
      paintSetColorMethod = env->GetMethodID(paintClass, "setColor", "(I)V");
      paintSetStyleMethod = env->GetMethodID(paintClass, "setStyle", "(Landroid/graphics/Paint$Style;)V");
      paintSetStrokeWidthMethod = env->GetMethodID(paintClass, "setStrokeWidth", "(F)V");
      paintSetStrokeJoinMethod = env->GetMethodID(paintClass, "setStrokeJoin", "(Landroid/graphics/Paint$Join;)V");
      canvasConstructor = env->GetMethodID(canvasClass, "<init>", "(Landroid/graphics/Bitmap;)V");
      factoryDecodeMethod = env->GetStaticMethodID(factoryClass, "decodeStream", "(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");
      factoryDecodeMethod2 = env->GetStaticMethodID(factoryClass, "decodeStream", "(Ljava/io/InputStream;Landroid/graphics/Rect;Landroid/graphics/BitmapFactory$Options;)Landroid/graphics/Bitmap;");
      bitmapCopyMethod = env->GetMethodID(bitmapClass, "copy", "(Landroid/graphics/Bitmap$Config;Z)Landroid/graphics/Bitmap;");
      paintConstructor = env->GetMethodID(paintClass, "<init>", "()V");
      paintSetAntiAliasMethod = env->GetMethodID(paintClass, "setAntiAlias", "(Z)V");
      pathMoveToMethod = env->GetMethodID(pathClass, "moveTo", "(FF)V");
      pathConstructor = env->GetMethodID(pathClass, "<init>", "()V");
      textAlignMethod = env->GetMethodID(paintClass, "setTextAlign", "(Landroid/graphics/Paint$Align;)V");
      canvasTextDrawMethod = env->GetMethodID(canvasClass, "drawText", "(Ljava/lang/String;FFLandroid/graphics/Paint;)V");
      pathLineToMethod = env->GetMethodID(pathClass, "lineTo", "(FF)V");
      pathCloseMethod = env->GetMethodID(pathClass, "close", "()V");
      canvasPathDrawMethod = env->GetMethodID(canvasClass, "drawPath", "(Landroid/graphics/Path;Landroid/graphics/Paint;)V");
      rectFConstructor = env->GetMethodID(rectFClass, "<init>", "(FFFF)V");
      rectConstructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");
      paintSetShadowMethod = env->GetMethodID(paintClass, "setShadowLayer", "(FFFI)V");
      canvasBitmapDrawMethod = env->GetMethodID(canvasClass, "drawBitmap", "(Landroid/graphics/Bitmap;FFLandroid/graphics/Paint;)V");
      canvasBitmapDrawMethod2 = env->GetMethodID(canvasClass, "drawBitmap", "(Landroid/graphics/Bitmap;Landroid/graphics/Rect;Landroid/graphics/RectF;Landroid/graphics/Paint;)V");
      factoryDecodeByteMethod = env->GetStaticMethodID(factoryClass, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
      bitmapGetWidthMethod = env->GetMethodID(bitmapClass, "getWidth", "()I");
      bitmapGetHeightMethod = env->GetMethodID(bitmapClass, "getHeight", "()I");
      bitmapOptionsConstructor = env->GetMethodID(bitmapOptionsClass, "<init>", "()V");
      fileConstructor = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;)V");
      fileInputStreamConstructor = env->GetMethodID(fileInputStreamClass, "<init>", "(Ljava/io/File;)V");

      optionsMutableField = env->GetFieldID(bitmapOptionsClass, "inMutable", "Z");
      alignEnumRight = env->GetStaticFieldID(alignClass, "RIGHT", "Landroid/graphics/Paint$Align;");
      alignEnumLeft = env->GetStaticFieldID(alignClass, "LEFT", "Landroid/graphics/Paint$Align;");
      alignEnumCenter = env->GetStaticFieldID(alignClass, "CENTER", "Landroid/graphics/Paint$Align;");

      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache java successfully initialized");
    }

  }

  bool isValid() const {
    return is_valid;
  }

  JNIEnv * getJNIEnv() {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Canvas getJNIENv called");

    if (javaVM == NULL) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "VM is null");
    }

    JNIEnv *Myenv = NULL;
//    javaVM->GetEnv((void**) &Myenv, JNI_VERSION_1_6);

    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6; // choose your JNI version
    args.name = NULL; // you might want to give the java thread a name
    args.group = NULL; // you might want to assign the java thread to a ThreadGroup
    javaVM->AttachCurrentThread(&Myenv, &args);

    if (Myenv == NULL) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Env is null");
    }

    return Myenv;
  }
  jobject & getAssetManager() {
    return assetManager;
  }

  jmethodID paintSetStyleMethod;
  jmethodID paintSetStrokeWidthMethod;
  jmethodID paintSetStrokeJoinMethod;
  jmethodID textAlignMethod;
  jmethodID paintSetColorMethod;
  jmethodID bitmapCreateMethod;
  jmethodID bitmapCreateMethod2;
  jmethodID canvasConstructor;
  jmethodID managerOpenMethod;
  jmethodID factoryDecodeMethod;
  jmethodID factoryDecodeMethod2;
  jmethodID bitmapCopyMethod;
  jmethodID paintConstructor;
  jmethodID paintSetAntiAliasMethod;
  jmethodID pathMoveToMethod;
  jmethodID pathConstructor;
  jmethodID canvasTextDrawMethod;
  jmethodID pathLineToMethod;
  jmethodID pathCloseMethod;
  jmethodID canvasPathDrawMethod;
  jmethodID rectFConstructor;
  jmethodID rectConstructor;
  jmethodID paintSetShadowMethod;
  jmethodID canvasBitmapDrawMethod;
  jmethodID canvasBitmapDrawMethod2;
  jmethodID factoryDecodeByteMethod;
  jmethodID bitmapCreateScaledMethod;
  jmethodID bitmapGetWidthMethod;
  jmethodID bitmapGetHeightMethod;
  jmethodID bitmapOptionsConstructor;
  jmethodID typefaceCreator;
  jmethodID setTypefaceMethod;
  jmethodID setAlphaMethod;
  jmethodID measureTextMethod;
  jmethodID measureDescentMethod;
  jmethodID measureAscentMethod;
  jmethodID fileConstructor;
  jmethodID fileInputStreamConstructor;

  jclass typefaceClass;
  jclass rectFClass;
  jclass rectClass;
  jclass canvasClass;
  jclass paintClass;
  jclass pathClass;
  jclass bitmapClass;
  jclass assetManagerClass;
  jclass factoryClass;
  jclass paintStyleClass;
  jclass alignClass;
  jclass bitmapConfigClass;
  jclass bitmapOptionsClass;
  jclass fileClass;
  jclass fileInputStreamClass;

  jfieldID field_argb_8888;
  jfieldID field_rgb_565;
  jfieldID optionsMutableField;
  jfieldID field_alpha_8;
  jfieldID alignEnumRight;
  jfieldID alignEnumLeft;
  jfieldID alignEnumCenter;



private:
  JNIEnv * env;
  jobject assetManager;
  bool is_valid = false;
  bool javaInitialized = false;
  JavaVM * javaVM;
};

class AndroidPaint {
public:
  AndroidPaint(AndroidCache * _cache) : cache(_cache) {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidPaint created");
  }

  ~AndroidPaint() {
    if (is_valid) {
      cache->getJNIEnv()->DeleteGlobalRef(obj);
    }
  }

  void setRenderMode(RenderMode mode) {
    create();
    auto * env = cache->getJNIEnv();
    switch (mode) {
    case STROKE:
      env->CallVoidMethod(obj, cache->paintSetStyleMethod, env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "STROKE", "Landroid/graphics/Paint$Style;")));
      break;
    case FILL:
      env->CallVoidMethod(obj, cache->paintSetStyleMethod, env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "FILL", "Landroid/graphics/Paint$Style;")));
      break;
    }
  }

  void setLineWidth(float lineWidth) {
    create();
    cache->getJNIEnv()->CallVoidMethod(obj, cache->paintSetStrokeWidthMethod, lineWidth);
  }

  void setStyle(const Style & style) {
    create();
    JNIEnv * env = cache->getJNIEnv();
    env->CallVoidMethod(obj, cache->paintSetColorMethod, getAndroidColor(style.color, globalAlpha));
  }

  void setGlobalAlpha(float alpha) {
    if (alpha != globalAlpha) {
      create();
      globalAlpha = alpha;
      cache->getJNIEnv()->CallVoidMethod(obj, cache->setAlphaMethod, (int) (255 * globalAlpha));
    }
  }

  void setShadow(float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor) {
    create();
    cache->getJNIEnv()->CallVoidMethod(getObject(), cache->paintSetShadowMethod, shadowBlur, shadowOffsetX, shadowOffsetY, getAndroidColor(shadowColor, globalAlpha));
  }

  void setFont(const Font & font) {
    create();
    JNIEnv * env = cache->getJNIEnv();

    if (font.size != current_font_size) {
      current_font_size = font.size;
      env->CallVoidMethod(obj, env->GetMethodID(cache->paintClass, "setTextSize", "(F)V"), font.size);
    }

    int textProperty = 0;
    if (font.weight.isBold()) {
      if (font.style == Font::Style::ITALIC || font.style == Font::Style::OBLIQUE) {
          textProperty = 3;
      } else {
        textProperty = 1;
      }
    } else if (font.style == Font::Style::ITALIC || font.style == Font::Style::OBLIQUE) {
      textProperty = 2;
    }

    if (font.family != current_font_family || textProperty != current_text_property) {
      current_font_family = font.family;
      current_text_property = textProperty;
      
      jobject typef = env->CallStaticObjectMethod(cache->typefaceClass, cache->typefaceCreator, env->NewStringUTF(font.family.c_str()), textProperty);
      env->CallObjectMethod(obj, cache->setTypefaceMethod, typef);
    }    
  }

  void setTextAlign(TextAlign textAlign) {
    if (textAlign != currentTextAlign) {
      create();
      currentTextAlign = textAlign;
      JNIEnv * env = cache->getJNIEnv();
      switch (textAlign) {
      case ALIGN_LEFT:
        env->CallVoidMethod(obj, cache->textAlignMethod, cache->getJNIEnv()->GetStaticObjectField(cache->alignClass, cache->alignEnumLeft));
        break;
      case ALIGN_RIGHT:
        env->CallVoidMethod(obj, cache->textAlignMethod, cache->getJNIEnv()->GetStaticObjectField(cache->alignClass, cache->alignEnumRight));
        break;
      case ALIGN_CENTER:
        env->CallVoidMethod(obj, cache->textAlignMethod, env->GetStaticObjectField(cache->alignClass, cache->alignEnumCenter));
      default:
        break;
      }
    }
  }

  float measureText(const std::string & text) {    
    create();
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "1");
    return cache->getJNIEnv()->CallFloatMethod(obj, cache->measureTextMethod, cache->getJNIEnv()->NewStringUTF(text.c_str()));
  }
  float getTextDescent() {
    create();
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "2");
    return cache->getJNIEnv()->CallFloatMethod(obj, cache->measureDescentMethod);
  }
  float getTextAscent() {
    create();
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "3");
    return cache->getJNIEnv()->CallFloatMethod(obj, cache->measureAscentMethod);
  }
  
  jobject & getObject() {
    create();
    return obj;
  }

 protected:
  void create() {
    if (!is_valid) {
      is_valid = true;
      cache->initJava();
      JNIEnv * env = cache->getJNIEnv();
      obj = (jobject) env->NewGlobalRef(env->NewObject(cache->paintClass, cache->paintConstructor));
      env->CallVoidMethod(obj, cache->paintSetAntiAliasMethod, JNI_TRUE);
      env->CallVoidMethod(obj, cache->paintSetStrokeJoinMethod, env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Join"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Join"), "ROUND", "Landroid/graphics/Paint$Join;")));
    }
  }
  
  static int getAndroidColor(const Color & color, float globalAlpha = 1.0f) {
    return (int(color.alpha * globalAlpha * 0xff) << 24) | (int(color.red * 0xff) << 16) | (int(color.green * 0xff) << 8) | int(color.blue * 0xff);
    return (int(color.alpha * globalAlpha * 0xff) << 24) | (int(color.red * 0xff) << 16) | (int(color.green * 0xff) << 8) | int(color.blue * 0xff);
  }

 private:
   AndroidCache * cache;
   jobject obj;
   float globalAlpha = 1.0f;
   float current_font_size = 0;
   int current_font_property = 0;
   std::string current_font_family;
   int current_text_property;
   TextAlign currentTextAlign = ALIGN_LEFT;
   bool is_valid = false;
 };

class AndroidSurface: public Surface {
public:
  friend class ContextAndroid;

  AndroidSurface(AndroidCache * _cache, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format);
  AndroidSurface(AndroidCache * _cache, const Image & image);
  AndroidSurface(AndroidCache * _cache, const std::string & filename);
  AndroidSurface(AndroidCache * _cache, const unsigned char * buffer, size_t size); // Create a bitmap from bytearray

  ~AndroidSurface() {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Destructor on ContextAndroid");
    //check if these have been made global references and delete if they are.

    JNIEnv * env = cache->getJNIEnv();

    if (env->GetObjectRefType(bitmap) == 2) {
      env->DeleteGlobalRef(bitmap);
    }

    if (canvasCreated) {
      if (env->GetObjectRefType(canvas) == 2) {
        env->DeleteGlobalRef(canvas);
      }
    }
  }

  void * lockMemory(bool write_access = false) override {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "lockMemory called");

    JNIEnv * env = cache->getJNIEnv();

    uint32_t *pixels;
    AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "pixels = %p", pixels);
    return pixels;
  }

  void releaseMemory() override {
    // is there AndroidBitmap_releasePixels?
  }

  void renderPath(RenderMode mode, const Path2D & path, const Style & style, float lineWidth, Operator op, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) override {
    checkForCanvas();

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "renderPath called");
    paint.setRenderMode(mode);
    paint.setStyle(style);
    paint.setGlobalAlpha(globalAlpha);
    paint.setShadow(shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);       
    if (mode == STROKE) paint.setLineWidth(lineWidth);

    JNIEnv * env = cache->getJNIEnv();

#if 0
    // set font
    jobject typef = env->CallObjectMethod(cache->typefaceClass, cache->typefaceCreator, NULL, 0);
#endif

    jboolean copyBoolean = JNI_TRUE;
    jboolean falseBoolean = JNI_FALSE;

    jobject jpath = env->NewObject(cache->pathClass, cache->pathConstructor);

    for (auto pc : path.getData()) {
      switch (pc.type) {
      case PathComponent::MOVE_TO: {
        env->CallVoidMethod(jpath, cache->pathMoveToMethod, pc.x0, pc.y0);
      }
        break;
      case PathComponent::LINE_TO: {
        env->CallVoidMethod(jpath, cache->pathLineToMethod, pc.x0, pc.y0);
      }
        break;
      case PathComponent::ARC: {

        float span = 0;

        // FIXME: If ea = 0, and sa = 2 * M_PI => span = 0
        if (!pc.anticlockwise && (pc.ea < pc.sa)) {
          span += 2 * M_PI;
        } else if (pc.anticlockwise && (pc.sa < pc.ea)) {
          span -= 2 * M_PI;
        }

        span += pc.ea - pc.sa;
        float left = pc.x0 * displayScale - pc.radius * displayScale;
        float right = pc.x0 * displayScale + pc.radius * displayScale;
        float bottom = pc.y0 * displayScale + pc.radius * displayScale;
        float top = pc.y0 * displayScale - pc.radius * displayScale;

        jobject jrect = env->NewObject(cache->rectFClass, cache->rectFConstructor, left, top, right, bottom);

        jmethodID pathArcToMethod = env->GetMethodID(cache->pathClass, "arcTo", "(Landroid/graphics/RectF;FF)V");

        env->CallVoidMethod(jpath, pathArcToMethod, jrect, (float) (pc.sa / M_PI * 180), (float) (span / M_PI * 180));
      }
        break;
      case PathComponent::CLOSE: {
        env->CallVoidMethod(jpath, cache->pathCloseMethod);
      }
        break;
      }
    }

    // Draw path to canvas
    env->CallVoidMethod(canvas, cache->canvasPathDrawMethod, jpath, paint.getObject());
  }

  void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat format) override {
    Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, format);
    // do resize the surface and discard the old data

    JNIEnv * env = cache->getJNIEnv();

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize called");
    if (canvasCreated){
      env->DeleteGlobalRef(canvas);
      canvasCreated = false;
    }
    env->DeleteGlobalRef(bitmap);
    bitmap = (jobject) env->NewGlobalRef(env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateScaledMethod, bitmap, _logical_width, _logical_height, JNI_FALSE));
  }

  void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, const Point & p, float lineWidth, Operator op, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) override {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "RenderText called");
    checkForCanvas();

    paint.setRenderMode(mode);
    paint.setFont(font);
    paint.setStyle(style);
    paint.setGlobalAlpha(globalAlpha);
    paint.setShadow(shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);
    paint.setTextAlign(textAlign);
    if (mode == STROKE) paint.setLineWidth(lineWidth);

    JNIEnv * env = cache->getJNIEnv();

    if (textBaseline == TextBaseline::MIDDLE || textBaseline == TextBaseline::TOP) {
      float descent = paint.getTextDescent();
      float ascent = paint.getTextAscent();
      
      if (textBaseline == TextBaseline::MIDDLE) {
        env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), p.x, p.y - (descent + ascent) / 2, paint.getObject());
      } else if (textBaseline == TextBaseline::TOP) {
        env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), p.x, p.y - (descent + ascent), paint.getObject());
      }
    } else {
      env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), p.x, p.y, paint.getObject());
    }

  }

  TextMetrics measureText(const Font & font, const std::string & text, TextBaseline textBaseline, float displayScale) override {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Measuring text");

    paint.setFont(font);
    
    float textWidth = paint.measureText(text);   
    float descent = paint.getTextDescent();
    float ascent = paint.getTextAscent();

    //Change ascent and descent according to baseline
    float baseline = 0;
    if (textBaseline == TextBaseline::MIDDLE) {
      baseline = (ascent + descent) / 2;
    } else if (textBaseline == TextBaseline::TOP) {
      baseline = (ascent + descent);
    }

    ascent -= baseline;
    descent -= baseline;

    return TextMetrics(textWidth, descent, ascent);
  }

  void drawImage(Surface & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled = true) override {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (Surface) called");
    AndroidSurface * native_surface = dynamic_cast<canvas::AndroidSurface *>(&_img);
    if (native_surface) {
      checkForCanvas();
      paint.setGlobalAlpha(globalAlpha);
      paint.setShadow(shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);

      JNIEnv * env = cache->getJNIEnv();
      jobject dstRect = env->NewObject(cache->rectFClass, cache->rectFConstructor, displayScale * p.x, displayScale * p.y, displayScale * (p.x + w), displayScale * (p.y + h));
      env->CallVoidMethod(canvas, cache->canvasBitmapDrawMethod2, native_surface->getBitmap(), NULL, dstRect, paint.getObject());
    } else {
      auto img = native_surface->createImage();
      drawImage(*img, p, w, h, displayScale, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath, imageSmoothingEnabled);
    }
  }

  void drawImage(const Image & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled = true) override {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (Image) called");

    checkForCanvas();

    paint.setGlobalAlpha(globalAlpha);
    paint.setShadow(shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);
    
    jobject drawableBitmap = imageToBitmap(_img);

    // Create new Canvas from the mutable bitmap
    JNIEnv * env = cache->getJNIEnv();
    jobject dstRect = env->NewObject(cache->rectFClass, cache->rectFConstructor, displayScale * p.x, displayScale * p.y, displayScale * (p.x + w), displayScale * (p.y + h));
    env->CallVoidMethod(canvas, cache->canvasBitmapDrawMethod2, drawableBitmap, NULL, dstRect, paint.getObject());
  }

  jobject imageToBitmap(const Image & _img);
  
  void checkForCanvas() {
    if (!canvasCreated) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "creating canvas");
      //Create new Canvas from the mutable bitmap
      JNIEnv * env = cache->getJNIEnv();
      canvas = (jobject) env->NewGlobalRef(env->NewObject(cache->canvasClass, cache->canvasConstructor, bitmap));
      canvasCreated = true;
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "canvas created");
    }
  }

  jobject getBitmap() {
    return bitmap;
  }
  jobject getCanvas() {
    return canvas;
  }

private:
  jobject bitmap;
  jobject canvas;

  bool canvasCreated = false;

  AndroidCache * cache;
  AndroidPaint paint;
};

class ContextAndroid: public Context {
public:
  ContextAndroid(AndroidCache * _cache, unsigned int _width, unsigned int _height, InternalFormat format, float _displayScale) :
      Context(_displayScale), cache(_cache), default_surface(_cache, _width, _height, (unsigned int) (_width * _displayScale), (unsigned int) (_height * _displayScale), format) {
  }

  std::shared_ptr<Surface> createSurface(const Image & image) override {
    return std::shared_ptr<Surface>(new AndroidSurface(cache, image));
  }
  std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, InternalFormat _format) override {
    return std::shared_ptr<Surface>(new AndroidSurface(cache, _width, _height, (unsigned int) (_width * getDisplayScale()), (unsigned int) (_height * getDisplayScale()), _format));
  }
  std::shared_ptr<Surface> createSurface(const std::string & filename) override {
    return std::shared_ptr<Surface>(new AndroidSurface(cache, filename));
  }

  Surface & getDefaultSurface() override {
    return default_surface;
  }
  const Surface & getDefaultSurface() const override {
    return default_surface;
  }

protected:
  bool hasNativeShadows() const override {
    return true;
  }

private:
  AndroidCache * cache;
  AndroidSurface default_surface;
};

class AndroidContextFactory: public ContextFactory {
public:
  AndroidContextFactory(JNIEnv * _env, jobject _assetManager, AndroidCache _cache, float _displayScale = 1.0f) :
      ContextFactory(_displayScale), cache(_cache) {
  }
  std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, InternalFormat format, bool apply_scaling = false) override {
    std::shared_ptr<Context> ptr(new ContextAndroid(&cache, width, height, format, apply_scaling ? getDisplayScale() : 1.0f));
    return ptr;
  }
  std::shared_ptr<Surface> createSurface(const std::string & filename) override {
    return std::shared_ptr<Surface>(new AndroidSurface(&cache, filename));
  }
  std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, InternalFormat format, bool apply_scaling) override {
    unsigned int aw = apply_scaling ? width * getDisplayScale() : width;
    unsigned int ah = apply_scaling ? height * getDisplayScale() : height;
    std::shared_ptr<Surface> ptr(new AndroidSurface(&cache, width, height, aw, ah, format));
    return ptr;
  }
  std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) override {
    std::shared_ptr<Surface> ptr(new AndroidSurface(&cache, buffer, size));
    return ptr;
  }

private:
  AndroidCache cache;
};
}
;

#endif
