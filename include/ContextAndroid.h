#ifndef _CANVAS_CONTEXTANDROID_H_
#define _CANVAS_CONTEXTANDROID_H_

#include "Context.h"

#include <sstream>
#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace canvas {
class AndroidCache {
public:
  AndroidCache(JNIEnv * _env, jobject _assetManager);

  ~AndroidCache() {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Destructor of AndroidCache on ContextAndroid");
    JNIEnv * env = getJNIEnv();
    env->DeleteGlobalRef(assetManager);
    env->DeleteGlobalRef(typefaceClass);
    env->DeleteGlobalRef(rectFClass);
    env->DeleteGlobalRef(rectClass);
    env->DeleteGlobalRef(canvasClass);
    env->DeleteGlobalRef(paintClass);
    env->DeleteGlobalRef(pathClass);
    env->DeleteGlobalRef(bitmapClass);
    env->DeleteGlobalRef(assetManagerClass);
    env->DeleteGlobalRef(factoryClass);
    env->DeleteGlobalRef(paintStyleClass);
    env->DeleteGlobalRef(alignClass);
    env->DeleteGlobalRef(bitmapConfigClass);
    env->DeleteGlobalRef(bitmapOptionsClass);
    env->DeleteGlobalRef(fileClass);
    env->DeleteGlobalRef(fileInputStreamClass);
    env->DeleteGlobalRef(stringClass);
    env->DeleteGlobalRef(charsetString);
    env->DeleteGlobalRef(linearGradientClass);
  }

  JNIEnv * getJNIEnv() {

    JNIEnv *Myenv = NULL;

    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6; // choose your JNI version
    args.name = NULL; // you might want to give the java thread a name
    args.group = NULL; // you might want to assign the java thread to a ThreadGroup
    javaVM->AttachCurrentThread(&Myenv, &args);

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
  jmethodID pathArcToMethod;
  jmethodID canvasPathDrawMethod;
  jmethodID rectFConstructor;
  jmethodID rectConstructor;
  jmethodID paintSetShadowMethod;
  jmethodID paintSetTextSizeMethod;
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
  jmethodID stringConstructor;
  jmethodID stringConstructor2;
  jmethodID stringGetBytesMethod;
  jmethodID stringByteConstructor;
  jmethodID errorMethod;
  jmethodID getStackTraceMethod;
  jmethodID factoryByteDecodeMethod;
  jmethodID paintSetShaderMethod;
  jmethodID linearGradientConstructor;

  jclass frameClass;
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
  jclass stringClass;
  jstring charsetString;
  jclass throwableClass;
  jclass linearGradientClass;
  jclass shaderTileModeClass;
  
  jfieldID field_argb_8888;
  jfieldID field_rgb_565;
  jfieldID optionsMutableField;
  jfieldID field_alpha_8;
  jfieldID alignEnumRight;
  jfieldID alignEnumLeft;
  jfieldID alignEnumCenter;
  jfieldID paintStyleEnumStroke;
  jfieldID paintStyleEnumFill;
  jfieldID shaderTileModeMirrorField;

private:
  jobject assetManager;
  JavaVM * javaVM;
};

class AndroidPaint {
 public:
 AndroidPaint(AndroidCache * _cache) : cache(_cache) { }
  
  ~AndroidPaint() {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Destructor on AndroidPaint");
    if (is_valid) {
      cache->getJNIEnv()->DeleteGlobalRef(obj);
    }
  }
  
  void setRenderMode(RenderMode mode) {
    create();
    auto * env = cache->getJNIEnv();
    switch (mode) {
    case STROKE: {
      jobject styleObject = env->GetStaticObjectField(cache->paintStyleClass, cache->paintStyleEnumStroke);
      env->CallVoidMethod(obj, cache->paintSetStyleMethod, styleObject);
      env->DeleteLocalRef(styleObject);
      break;
    }
    case FILL: {
      jobject styleObject = env->GetStaticObjectField(cache->paintStyleClass, cache->paintStyleEnumFill);
      env->CallVoidMethod(obj, cache->paintSetStyleMethod, styleObject);
      env->DeleteLocalRef(styleObject);
      break;
    }
    }
  }

  void setLineWidth(float lineWidth) {
    create();
    cache->getJNIEnv()->CallVoidMethod(obj, cache->paintSetStrokeWidthMethod, lineWidth);
  }

  void setStyle(const Style & style, float displayScale) {
    create();

    JNIEnv * env = cache->getJNIEnv();
    switch (style.getType()) {
    case Style::SOLID:
      env->CallVoidMethod(obj, cache->paintSetColorMethod, getAndroidColor(style.color, globalAlpha));
      break;
    case Style::LINEAR_GRADIENT:
      const std::map<float, Color> & colors = style.getColors();
      if (!colors.empty()) {
        std::map<float, Color>::const_iterator it0 = colors.begin(), it1 = colors.end();
        it1--;

        int colorOne = getAndroidColor(it0->second);
        int colorTwo = getAndroidColor(it1->second);
	float x0 = style.x0 * displayScale;
	float y0 = style.y0 * displayScale;
	float x1 = style.x1 * displayScale;
	float y1 = style.y1 * displayScale;
        jobject tileFieldObject = env->GetStaticObjectField(cache->shaderTileModeClass, cache->shaderTileModeMirrorField);
        jobject linearGradient = env->NewObject(cache->linearGradientClass, cache->linearGradientConstructor, x0, y0, x1, y1, colorOne, colorTwo, tileFieldObject);
        jobject resultGradient = env->CallObjectMethod(obj, cache->paintSetShaderMethod, linearGradient);
        env->DeleteLocalRef(tileFieldObject);
        env->DeleteLocalRef(linearGradient);
        env->DeleteLocalRef(resultGradient);
      }
      break;
    }
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

  void setFont(const Font & font, float displayScale) {
    create();
    JNIEnv * env = cache->getJNIEnv();

    if (font.size != current_font_size) {
      current_font_size = font.size;
      env->CallVoidMethod(obj, cache->paintSetTextSizeMethod, font.size * displayScale);
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
      jstring jfamily = env->NewStringUTF(font.family.c_str());
      jobject typef = env->CallStaticObjectMethod(cache->typefaceClass, cache->typefaceCreator, jfamily, textProperty);
      jobject extratypef = env->CallObjectMethod(obj, cache->setTypefaceMethod, typef);
      env->DeleteLocalRef(typef);
      env->DeleteLocalRef(jfamily);
      env->DeleteLocalRef(extratypef);
    }    
  }

  void setTextAlign(TextAlign textAlign) {
    if (textAlign != currentTextAlign) {
      create();
      currentTextAlign = textAlign;
      JNIEnv * env = cache->getJNIEnv();
      switch (textAlign) {
      case ALIGN_LEFT: {
        jobject alignObject =  env->GetStaticObjectField(cache->alignClass, cache->alignEnumLeft);
        env->CallVoidMethod(obj, cache->textAlignMethod, alignObject);
        env->DeleteLocalRef(alignObject);
        break;
      }
      case ALIGN_RIGHT: {
        jobject alignObject = env->GetStaticObjectField(cache->alignClass, cache->alignEnumRight);
        env->CallVoidMethod(obj, cache->textAlignMethod, alignObject);
        env->DeleteLocalRef(alignObject);
        break;
      }
      case ALIGN_CENTER: {
        jobject alignObject = env->GetStaticObjectField(cache->alignClass, cache->alignEnumCenter);
        env->CallVoidMethod(obj, cache->textAlignMethod, alignObject);
        env->DeleteLocalRef(alignObject);
      }
      default:
        break;
      }
    }
  }

  float measureText(const std::string & text) {    
    create();

    JNIEnv * env = cache->getJNIEnv();

    char tab2[1024];
    strcpy(tab2, text.c_str());

    int size = 16;
    jbyteArray array = env->NewByteArray(size);
    env->SetByteArrayRegion(array, 0, size, (const jbyte*) tab2);


    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "creating string from bytes");
    jstring convertableString = (jstring) env->NewObject(cache->stringClass, cache->stringByteConstructor, array, cache->charsetString);
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "created");

//    jstring convertableString = env->NewStringUTF(text.c_str());
    jobject bytes = env->CallObjectMethod(convertableString, cache->stringGetBytesMethod);
    jobject jtext = env->NewObject(cache->stringClass, cache->stringConstructor, bytes, cache->charsetString);
    float measure = env->CallFloatMethod(obj, cache->measureTextMethod, convertableString);
    env->DeleteLocalRef(convertableString);
    env->DeleteLocalRef(bytes);
    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(jbyteArray);
    return measure;
  }
  float getTextDescent() {
    create();
    return cache->getJNIEnv()->CallFloatMethod(obj, cache->measureDescentMethod);
  }
  float getTextAscent() {
    create();
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
      JNIEnv * env = cache->getJNIEnv();
      obj = (jobject) env->NewGlobalRef(env->NewObject(cache->paintClass, cache->paintConstructor));
      env->CallVoidMethod(obj, cache->paintSetAntiAliasMethod, JNI_TRUE);
      jclass joinClass = env->FindClass("android/graphics/Paint$Join");
      jobject joinFieldObject = env->GetStaticObjectField(joinClass, env->GetStaticFieldID(joinClass, "ROUND", "Landroid/graphics/Paint$Join;"));
      env->CallVoidMethod(obj, cache->paintSetStrokeJoinMethod, joinFieldObject);
      env->DeleteLocalRef(joinClass);
      env->DeleteLocalRef(joinFieldObject);
    }
  }
  
  static int getAndroidColor(const Color & color, float globalAlpha = 1.0f) {
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
  AndroidSurface(AndroidCache * _cache, const ImageData & image);
  AndroidSurface(AndroidCache * _cache, const std::string & filename);
  AndroidSurface(AndroidCache * _cache, const unsigned char * buffer, size_t size); // Create a bitmap from bytearray

  ~AndroidSurface() {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Destructor on ContextAndroid");
    //check if these have been made global references and delete if they are.

    JNIEnv * env = cache->getJNIEnv();

    switch (env->GetObjectRefType(bitmap)){
    case 0:
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "invalid ref type on bitmap deletion");
      break;
    case 1:
      env->DeleteLocalRef(bitmap);
      break;
    case 2:
      env->DeleteGlobalRef(bitmap);
      break;
    case 3:
      env->DeleteWeakGlobalRef(bitmap);
      break;
    }


    switch (env->GetObjectRefType(canvas)){
    case 0:
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "invalid ref type on canvas deletion");
      break;
    case 1:
      env->DeleteLocalRef(canvas);
      break;
    case 2:
      env->DeleteGlobalRef(canvas);
      break;
    case 3:
      env->DeleteWeakGlobalRef(canvas);
      break;
    }

    paint = NULL;
  }

  void renderPath(RenderMode mode, const Path2D & path, const Style & style, float lineWidth, Operator op, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) override {

    JNIEnv * env = cache->getJNIEnv();

    checkForCanvas();

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "renderPath called");
    paint.setRenderMode(mode);
    paint.setStyle(style, displayScale);
    paint.setGlobalAlpha(globalAlpha);
    paint.setShadow(shadowBlur * displayScale, shadowOffsetX * displayScale, shadowOffsetY * displayScale, shadowColor);       
    if (mode == STROKE) paint.setLineWidth(lineWidth);


#if 0
    // set font
    jobject typef = env->CallObjectMethod(cache->typefaceClass, cache->typefaceCreator, NULL, 0);
#endif

    jobject jpath = env->NewObject(cache->pathClass, cache->pathConstructor);

    for (auto pc : path.getData()) {
      switch (pc.type) {
      case PathComponent::MOVE_TO: {
        env->CallVoidMethod(jpath, cache->pathMoveToMethod, pc.x0 * displayScale, pc.y0 * displayScale);
      }
        break;
      case PathComponent::LINE_TO: {
        env->CallVoidMethod(jpath, cache->pathLineToMethod, pc.x0 * displayScale, pc.y0 * displayScale);
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

        env->CallVoidMethod(jpath, cache->pathArcToMethod, jrect, (float) (pc.sa / M_PI * 180), (float) (span / M_PI * 180));
        env->DeleteLocalRef(jrect);
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
    env->DeleteLocalRef(jpath);

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "renderPath done");
  }

  void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat format) override {
    Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, format);
    // do resize the surface and discard the old data

    JNIEnv * env = cache->getJNIEnv();

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize called Start");
    if (canvasCreated) {
      env->DeleteGlobalRef(canvas);
      canvasCreated = false;
    }
    if (bitmap != 0) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize called");
      jobject localBitmap = bitmap;
      if (localBitmap == NULL || localBitmap == 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "localBitmap is null");
      }
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize called");
      bitmap = (jobject) env->NewGlobalRef(env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateScaledMethod, localBitmap, _actual_width, _actual_height, JNI_FALSE));
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize called");
      env->DeleteLocalRef(localBitmap);
    } else {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "creating new bitmap on resize");
      jobject argbObject;
      if (format == LUMINANCE_ALPHA) {
        __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "setting imageformat to alpha 8");
        argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_alpha_8);
      } else if (format == RGB565) {
        __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "setting imageformat to argb565");
        argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_rgb_565);
      } else {
        __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "setting imageformat to argb8888");
        argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
      }

      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "creating new bitmap on resize 2");
      jobject localBitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod, _actual_width, _actual_height, argbObject);
      if (localBitmap) {
        bitmap = (jobject) env->NewGlobalRef(localBitmap);
        env->DeleteLocalRef(localBitmap);
      } else {
        bitmap = 0;
      }
      env->DeleteLocalRef(argbObject);
    }

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize done");
  }

  void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, const Point & p, float lineWidth, Operator op, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) override {


    JNIEnv * env = cache->getJNIEnv();

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "RenderText called");
    checkForCanvas();

    paint.setRenderMode(mode);
    paint.setFont(font, displayScale);
    paint.setStyle(style, displayScale);
    paint.setGlobalAlpha(globalAlpha);
    paint.setShadow(shadowBlur * displayScale, shadowOffsetX * displayScale, shadowOffsetY * displayScale, shadowColor);
    paint.setTextAlign(textAlign);
    if (mode == STROKE) paint.setLineWidth(lineWidth);

    jstring convertableString = env->NewStringUTF(text.c_str());
    jobject bytes = env->CallObjectMethod(convertableString, cache->stringGetBytesMethod);
    jobject jtext = env->NewObject(cache->stringClass, cache->stringConstructor, bytes, cache->charsetString);
    env->DeleteLocalRef(convertableString);
    env->DeleteLocalRef(bytes);

    float descent = paint.getTextDescent();
    float ascent = paint.getTextAscent();
    double x = p.x * displayScale, y = p.y * displayScale;

    if (textBaseline == TextBaseline::MIDDLE || textBaseline == TextBaseline::TOP) {
      if (textBaseline == TextBaseline::MIDDLE) {
        env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, jtext, x, y - (descent + ascent) / 2, paint.getObject());
      } else if (textBaseline == TextBaseline::TOP) {
        env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, jtext, x, y - (descent + ascent), paint.getObject());
      }
    } else {
      env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, jtext, x, y, paint.getObject());
    }

    env->DeleteLocalRef(jtext);
  }

  TextMetrics measureText(const Font & font, const std::string & text, TextBaseline textBaseline, float displayScale) override {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Measuring text");

    paint.setFont(font, displayScale);
    
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

    return TextMetrics(textWidth, descent / displayScale, ascent / displayScale);
  }

  void drawImage(Surface & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled = true) override {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (Surface) called");
    AndroidSurface * native_surface = dynamic_cast<canvas::AndroidSurface *>(&_img);
    if (native_surface) {
      JNIEnv * env = cache->getJNIEnv();
      checkForCanvas();
      paint.setGlobalAlpha(globalAlpha);
      paint.setShadow(shadowBlur * displayScale, shadowOffsetX * displayScale, shadowOffsetY * displayScale, shadowColor);

      jobject dstRect = env->NewObject(cache->rectFClass, cache->rectFConstructor, displayScale * p.x, displayScale * p.y, displayScale * (p.x + w), displayScale * (p.y + h));
      env->CallVoidMethod(canvas, cache->canvasBitmapDrawMethod2, native_surface->bitmap, NULL, dstRect, paint.getObject());
      env->DeleteLocalRef(dstRect);
    } else {
      auto img = native_surface->createImage(displayScale);
      drawImage(img->getData(), p, w, h, displayScale, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath, imageSmoothingEnabled);
    }
  }

  void drawImage(const ImageData & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled = true) override {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (Image) called");

    JNIEnv * env = cache->getJNIEnv();

    checkForCanvas();

    paint.setGlobalAlpha(globalAlpha);
    paint.setShadow(shadowBlur * displayScale, shadowOffsetX * displayScale, shadowOffsetY * displayScale, shadowColor);
    


    if (env->ExceptionCheck()) {
      jthrowable error = env->ExceptionOccurred();
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "error1");
      env->CallStaticVoidMethod(cache->frameClass, cache->errorMethod, error);
      env->DeleteLocalRef(error);
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "clearing");
      env->ExceptionClear();
      return;
    }


    jobject drawableBitmap = imageToBitmap(_img);

    if (!drawableBitmap){
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "null bitmap");
      return;
    }

    // Create new Canvas from the mutable bitmap
    jobject srcRect = env->NewObject(cache->rectClass, cache->rectConstructor, 0, 0, _img.getWidth(), _img.getHeight());
    jobject dstRect = env->NewObject(cache->rectFClass, cache->rectFConstructor, displayScale * p.x, displayScale * p.y, displayScale * (p.x + w), displayScale * (p.y + h));

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "drawing the bitmap");
    env->CallVoidMethod(canvas, cache->canvasBitmapDrawMethod2, drawableBitmap, srcRect, dstRect, paint.getObject());
    env->DeleteLocalRef(drawableBitmap);
    env->DeleteLocalRef(srcRect);
    env->DeleteLocalRef(dstRect);
  }

  std::unique_ptr<Image> createImage(float display_scale) override;

 protected:
  void * lockMemory(bool write_access = false) override {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "lockMemory called");

    JNIEnv * env = cache->getJNIEnv();

    uint32_t *pixels = 0;
    AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "pixels = %p", pixels);
    return pixels;
  }

  void releaseMemory() override {
    JNIEnv * env = cache->getJNIEnv();
    AndroidBitmap_unlockPixels(env, bitmap);
  }

  jobject imageToBitmap(const ImageData & _img);
  
  void checkForCanvas() {
    if (!canvasCreated && bitmap != 0) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "creating canvas");
      //Create new Canvas from the mutable bitmap
      JNIEnv * env = cache->getJNIEnv();
      jobject localCanvas = env->NewObject(cache->canvasClass, cache->canvasConstructor, bitmap);
      canvas = (jobject) env->NewGlobalRef(localCanvas);
      env->DeleteLocalRef(localCanvas);
      canvasCreated = true;
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "canvas created");
    }
  }

#if 0
  jobject getBitmap() {
    return bitmap;
  }
  jobject getCanvas() {
    return canvas;
  }
#endif

private:
  jobject bitmap = 0;
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

  std::unique_ptr<Surface> createSurface(const ImageData & image) override {
    return std::unique_ptr<Surface>(new AndroidSurface(cache, image));
  }
  std::unique_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, InternalFormat _format) override {
    return std::unique_ptr<Surface>(new AndroidSurface(cache, _width, _height, (unsigned int) (_width * getDisplayScale()), (unsigned int) (_height * getDisplayScale()), _format));
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
 AndroidContextFactory(jobject assetManager, const std::shared_ptr<AndroidCache> & _cache, float _displayScale) :
      ContextFactory(_displayScale), cache(_cache) {	
    JNIEnv * env = cache->getJNIEnv();
    asset_manager = AAssetManager_fromJava(env, assetManager);
  }
  std::unique_ptr<Context> createContext(unsigned int width, unsigned int height, InternalFormat format) override {
    return std::unique_ptr<Context>(new ContextAndroid(cache.get(), width, height, format, getDisplayScale()));
  }
  std::unique_ptr<Surface> createSurface(unsigned int width, unsigned int height, InternalFormat format) override {
    unsigned int aw = width * getDisplayScale(), ah = height * getDisplayScale();
    return std::unique_ptr<Surface>(new AndroidSurface(cache.get(), width, height, aw, ah, format));
  }

  std::unique_ptr<Image> loadImage(const std::string & filename) override;
  std::unique_ptr<Image> createImage() override;
  std::unique_ptr<Image> createImage(const unsigned char * _data, InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels, short _quality) override;

private:
  std::shared_ptr<AndroidCache> cache;
  AAssetManager * asset_manager;
};
}
;

#endif
