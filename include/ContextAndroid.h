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
  AndroidCache(JNIEnv * _env, jobject _mgr) :
      env(_env) {
    mgr = env->NewGlobalRef(_mgr);
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache created");

    //	javaInitialized = false;

    //arcToMethod = blablah();
    //if (arcToMethod && pathConstructor) {
    //	is_valid = true;
    //}
  }

  void initJava() {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache initJava called");

    if (!javaInitialized) {
      javaInitialized = true;

      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache java is being initialized");

      typefaceClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Typeface"));
      canvasClass = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/Canvas"));
      mgrClass = (jclass) env->NewGlobalRef(env->FindClass("android/content/res/AssetManager"));
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

      measureAscentMethod = env->GetMethodID(paintClass, "ascent", "()F");
      measureDescentMethod = env->GetMethodID(paintClass, "descent", "()F");
      measureTextMethod = env->GetMethodID(paintClass, "measureText", "(Ljava/lang/String;)F");
      setAlphaMethod = env->GetMethodID(paintClass, "setAlpha", "(I)V");
      setTypefaceMethod = env->GetMethodID(paintClass, "setTypeface", "(Landroid/graphics/Typeface;)Landroid/graphics/Typeface;");
      typefaceCreator = env->GetStaticMethodID(typefaceClass, "create", "(Ljava/lang/String;I)Landroid/graphics/Typeface;");
      managerOpenMethod = env->GetMethodID(mgrClass, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
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
    return env;
  }
  jobject & getMgr() {
    return mgr;
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

  jclass typefaceClass;
  jclass rectFClass;
  jclass rectClass;
  jclass canvasClass;
  jclass paintClass;
  jclass pathClass;
  jclass bitmapClass;
  jclass mgrClass;
  jclass factoryClass;
  jclass paintStyleClass;
  jclass alignClass;
  jclass bitmapConfigClass;
  jclass bitmapOptionsClass;

  jfieldID field_argb_8888;
  jfieldID field_rgb_565;
  jfieldID optionsMutableField;
  jfieldID field_alpha_8;
  jfieldID alignEnumRight;
  jfieldID alignEnumLeft;
  jfieldID alignEnumCenter;

private:
  JNIEnv * env;
  jobject mgr;
  bool is_valid = false;
  bool javaInitialized = false;
};
class AndroidSurface: public Surface {
public:
  friend class ContextAndroid;

  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format) :
      Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format), cache(_cache), env(_env), mgr(_mgr) {
    // creates an empty canvas

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidSurface widthheight constructor called");

    cache->initJava();

    //set bitmap config according to internalformat
    jobject argbObject;

    if (_format == LUMINANCE_ALPHA) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "setting imageformat to alpha 8");
      argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_alpha_8);
    } else if (_format == RGB565) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "setting imageformat to argb565");
      argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_rgb_565);
    } else {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "setting imageformat to argb8888");
      argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, env->GetStaticFieldID(cache->bitmapConfigClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;"));
    }

    bitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod, _actual_width, _actual_height, argbObject);

  }

  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const Image & image) :
      Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), RGBA8), cache(_cache), env(_env), mgr(_mgr) {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface Image constructor");

    // creates a surface with width, height and contents from image
    bitmap = imageToBitmap(image);
  }

  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const std::string & filename) :
      Surface(0, 0, 0, 0, RGBA8), cache(_cache), env(_env), mgr(_mgr) {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface filename constructor");

    cache->initJava();

    //Get inputStream from the picture(filename)
    jobject inputStream = env->CallObjectMethod(mgr, cache->managerOpenMethod, env->NewStringUTF(filename.c_str()));

    //Create BitmapFactory options to make the created bitmap mutable straight away
    jobject factoryOptions = env->NewObject(cache->bitmapOptionsClass, cache->bitmapOptionsConstructor);
    env->SetBooleanField(factoryOptions, cache->optionsMutableField, JNI_TRUE);

    //Create a bitmap from the inputStream
    bitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryDecodeMethod2, inputStream, NULL, factoryOptions);

    int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
    int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
    Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);
  }

  //Create a bitmap from bytearray
  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const unsigned char * buffer, size_t size) :
      Surface(0, 0, 0, 0, RGBA8), cache(_cache), env(_env), mgr(_mgr) {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndrodiSurface constructor (buffer)  called");

    cache->initJava();

    int arraySize = size;

    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "size = %i", size);

    jbyteArray array = env->NewByteArray(arraySize);
    env->SetByteArrayRegion(array, 0, arraySize, (const jbyte*) buffer);
    jclass thisClass = env->FindClass("android/graphics/BitmapFactory");
    jmethodID thisMethod = env->GetStaticMethodID(env->FindClass("android/graphics/BitmapFactory"), "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
    jobject firstBitmap = env->CallStaticObjectMethod(thisClass, thisMethod, array, 0, arraySize);

    //make this with factory options instead
    jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
    bitmap = env->CallObjectMethod(firstBitmap, cache->bitmapCopyMethod, argbObject, JNI_TRUE);

    int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
    int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
    Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndrodiSurface constructor (buffer)  called");

  }

  ~AndroidSurface() {
    // remember to free canvas and bitmap
  }

  void * lockMemory(bool write_access = false) override {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "canvas making is...");
    uint32_t *pixels;
    AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "pixels = %p", pixels);
    return pixels;
  }

  void releaseMemory() override {
    // is there AndroidBitmap_releasePixels?
  }

  jobject createJavaPaint(RenderMode mode, const Font & font, const Style & style, float lineWidth, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor) {

    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "LineWiPdth = %f", lineWidth);
    //create paint
    jobject jpaint = env->NewObject(cache->paintClass, cache->paintConstructor);

    //Paint.setColor;
    env->CallVoidMethod(jpaint, cache->paintSetAntiAliasMethod, JNI_TRUE);
    ;

    //Paint Set Style
    switch (mode) {
    case STROKE:
      env->CallVoidMethod(jpaint, cache->paintSetStyleMethod, env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "STROKE", "Landroid/graphics/Paint$Style;")));
      //Paint Set Stroke Width
      env->CallVoidMethod(jpaint, cache->paintSetStrokeWidthMethod, lineWidth);
      //Paint set StrokeJoin
      env->CallVoidMethod(jpaint, cache->paintSetStrokeJoinMethod, env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Join"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Join"), "ROUND", "Landroid/graphics/Paint$Join;")));
      break;
    case FILL:
      env->CallVoidMethod(jpaint, cache->paintSetStyleMethod, env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "FILL", "Landroid/graphics/Paint$Style;")));

      break;
    }
    //Paint set Color
    env->CallVoidMethod(jpaint, cache->paintSetColorMethod, getAndroidColor(style.color, globalAlpha));

    //Set alpha
    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "Globalalhpa = %f", globalAlpha);
    env->CallVoidMethod(jpaint, cache->setAlphaMethod, (int) (255 * globalAlpha));

    //Set shadow
    env->CallVoidMethod(jpaint, cache->paintSetShadowMethod, shadowBlur, shadowOffsetX, shadowOffsetY, getAndroidColor(shadowColor, globalAlpha));
    //set paint text size.
    env->CallVoidMethod(jpaint, env->GetMethodID(cache->paintClass, "setTextSize", "(F)V"), font.size);

    //Set Text Font and properties
    int textProperty = 0;
    if (font.style == Font::Style::ITALIC || font.style == Font::Style::OBLIQUE)
      textProperty = 2;
    if (font.weight == Font::Weight::BOLD) {
      if (font.style == Font::Style::ITALIC || font.style == Font::Style::OBLIQUE)
	textProperty = 3;
      else
	textProperty = 1;
    }
    jobject typef = env->CallObjectMethod(cache->typefaceClass, cache->typefaceCreator, env->NewStringUTF(font.family.c_str()), textProperty);
    env->CallObjectMethod(jpaint, cache->setTypefaceMethod, typef);

    return jpaint;
  }

  void renderPath(RenderMode mode, const Path2D & path, const Style & style, float lineWidth, Operator op, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) override {

    checkForCanvas();

    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "LineWidth = %f", lineWidth);

    jobject jpaint = createJavaPaint(mode, NULL, style, lineWidth, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);

    //set font
    jobject typef = env->CallObjectMethod(cache->typefaceClass, cache->typefaceCreator, NULL, 0);

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

    //Draw path to canvas
    env->CallVoidMethod(canvas, cache->canvasPathDrawMethod, jpath, jpaint);

  }

  void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat format) override {
    Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, format);
    // do resize the surface and discard the old data

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize called");

    bitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateScaledMethod, bitmap, _logical_width, _logical_height, JNI_FALSE);

  }

  void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, const Point & p, float lineWidth, Operator op, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath) override {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "RenderText called");
    checkForCanvas();

    jobject jpaint = createJavaPaint(mode, font, style, lineWidth, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);

    switch (textAlign) {
    case ALIGN_LEFT:
      env->CallVoidMethod(jpaint, cache->textAlignMethod, env->GetStaticObjectField(cache->alignClass, cache->alignEnumLeft));
      break;
    case ALIGN_RIGHT:
      env->CallVoidMethod(jpaint, cache->textAlignMethod, env->GetStaticObjectField(cache->alignClass, cache->alignEnumRight));
      break;
    case ALIGN_CENTER:
      env->CallVoidMethod(jpaint, cache->textAlignMethod, env->GetStaticObjectField(cache->alignClass, cache->alignEnumCenter));
    default:
      break;
    }

    if (textBaseline == TextBaseline::MIDDLE || textBaseline == TextBaseline::TOP) {
      float descent = env->CallFloatMethod(jpaint, cache->measureDescentMethod);
      float ascent = env->CallFloatMethod(jpaint, cache->measureAscentMethod);
      if (textBaseline == TextBaseline::MIDDLE) {
	env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), p.x, p.y - (descent + ascent) / 2, jpaint);
      } else if (textBaseline == TextBaseline::TOP) {
	env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), p.x, p.y - (descent + ascent), jpaint);
      }
    } else {
      env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), p.x, p.y, jpaint);
    }

  }

  TextMetrics measureText(const Font & font, const std::string & text, TextBaseline textBaseline, float displayScale) override {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Measuring text");
    jobject jpaint = createJavaPaint(RenderMode::STROKE, font, NULL, NULL, 1.0f, 0.0f, 0.0f, 0.0f, Color::BLACK);

    float textWidth = env->CallFloatMethod(jpaint, cache->measureTextMethod, env->NewStringUTF(text.c_str()));
    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "Measured text width = %f", textWidth);
    float descent = env->CallFloatMethod(jpaint, cache->measureDescentMethod);
    float ascent = env->CallFloatMethod(jpaint, cache->measureAscentMethod);
    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "MeasureText Descent = %f", descent);
    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "MeasureText Ascent = %f", ascent);

    //Change ascent and descent according to baseline
    float baseline = 0;
    if (textBaseline == TextBaseline::MIDDLE) {
      baseline = (ascent + descent) / 2;
      __android_log_print(ANDROID_LOG_INFO, "Sometrik", "measure text baseline - middle = %f", baseline);
    } else if (textBaseline == TextBaseline::TOP) {
      baseline = (ascent + descent);
      __android_log_print(ANDROID_LOG_INFO, "Sometrik", "measure text baseline - top = %f", baseline);
    }

    ascent = ascent - baseline;
    descent = descent - baseline;

    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "MeasureText Descent = %f", descent);
    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "MeasureText Ascent = %f", ascent);

    return TextMetrics(textWidth, descent, ascent);
  }

  void drawImage(Surface & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled = true) override {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (Surface) called");
    AndroidSurface * native_surface = dynamic_cast<canvas::AndroidSurface *>(&_img);
    if (native_surface) {
      checkForCanvas();
      jobject jpaint = createJavaPaint(RenderMode::STROKE, NULL, NULL, NULL, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);
      jobject dstRect = env->NewObject(cache->rectFClass, cache->rectFConstructor, displayScale * p.x, displayScale * p.y, displayScale * (p.x + w), displayScale * (p.y + h));
      env->CallVoidMethod(canvas, cache->canvasBitmapDrawMethod2, native_surface->getBitmap(), NULL, dstRect, jpaint);
    } else {
      auto img = native_surface->createImage();
      drawImage(*img, p, w, h, displayScale, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath, imageSmoothingEnabled);
    }
  }

  void drawImage(const Image & _img, const Point & p, double w, double h, float displayScale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path2D & clipPath, bool imageSmoothingEnabled = true) override {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (Image) called");

    checkForCanvas();

    createJavaPaint(RenderMode::STROKE, NULL, NULL, NULL, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor);

    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "width = %f", w);
    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "height = %f", h);

    jobject drawableBitmap = imageToBitmap(_img);

    //Create new Canvas from the mutable bitmap
    jobject dstRect = env->NewObject(cache->rectFClass, cache->rectFConstructor, displayScale * p.x, displayScale * p.y, displayScale * (p.x + w), displayScale * (p.y + h));
    env->CallVoidMethod(canvas, cache->canvasBitmapDrawMethod2, drawableBitmap, NULL, dstRect, NULL);

  }

  jobject imageToBitmap(const Image & _img) {

    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", " ImageToBitmap called");

    const unsigned char* buf = _img.getData();

    int length;
    if (_img.getImageFormat() == ImageFormat::RGB24 || _img.getImageFormat() == ImageFormat::RGB32 || _img.getImageFormat() == ImageFormat::RGB565) {
      length = _img.getWidth() * _img.getHeight() * 3;
    } else {
      length = _img.getWidth() * _img.getHeight() * 4;
    }

    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "length = %i", length);

    jbyteArray jarray = env->NewByteArray(length);
    env->SetByteArrayRegion(jarray, 0, length, (jbyte*) (buf));

    jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
    jobject drawableBitmap = env->CallObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod2, jarray, _img.getWidth(), _img.getHeight(), argbObject);

    return drawableBitmap;
  }

  void checkForCanvas() {
    if (!canvasCreated) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "canvas created");
      //Create new Canvas from the mutable bitmap
      canvas = env->NewObject(cache->canvasClass, cache->canvasConstructor, bitmap);
      canvasCreated = true;
    }
  }

  jobject getBitmap() {
    return bitmap;
  }
  jobject getCanvas() {
    return canvas;
  }

protected:
  static int getAndroidColor(const Color & color, float globalAlpha = 1.0f) {
    return (int(color.alpha * globalAlpha * 0xff) << 24) | (int(color.red * 0xff) << 16) | (int(color.green * 0xff) << 8) | int(color.blue * 0xff);
    return (int(color.alpha * globalAlpha * 0xff) << 24) | (int(color.red * 0xff) << 16) | (int(color.green * 0xff) << 8) | int(color.blue * 0xff);
  }

private:
  jobject bitmap;
  jobject canvas;

  bool canvasCreated = false;

  AndroidCache * cache;
  JNIEnv * env;
  jobject mgr;
};

class ContextAndroid: public Context {
public:
  ContextAndroid(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, unsigned int _width, unsigned int _height, InternalFormat format, float _displayScale) :
      Context(_displayScale), cache(_cache), env(_env), mgr(_mgr), default_surface(_cache, _env, _mgr, _width, _height, (unsigned int) (_width * _displayScale), (unsigned int) (_height * _displayScale), format) {
  }

  std::shared_ptr<Surface> createSurface(const Image & image) override {
    return std::shared_ptr<Surface>(new AndroidSurface(cache, env, mgr, image));
  }
  std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, InternalFormat _format) override {
    return std::shared_ptr<Surface>(new AndroidSurface(cache, env, mgr, _width, _height, (unsigned int) (_width * getDisplayScale()), (unsigned int) (_height * getDisplayScale()), _format));
  }
  std::shared_ptr<Surface> createSurface(const std::string & filename) override {
    return std::shared_ptr<Surface>(new AndroidSurface(cache, env, mgr, filename));
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
  JNIEnv * env;
  jobject mgr;
  AndroidSurface default_surface;
};

class AndroidContextFactory: public ContextFactory {
public:
  AndroidContextFactory(JNIEnv * _env, jobject _mgr, float _displayScale = 1.0f) :
      ContextFactory(_displayScale), cache(_env, _mgr) {
  }
  std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, InternalFormat format, bool apply_scaling = false) override {
    std::shared_ptr<Context> ptr(new ContextAndroid(&cache, cache.getJNIEnv(), cache.getMgr(), width, height, format, apply_scaling ? getDisplayScale() : 1.0f));
    return ptr;
  }
  std::shared_ptr<Surface> createSurface(const std::string & filename) override {
    return std::shared_ptr<Surface>(new AndroidSurface(&cache, cache.getJNIEnv(), cache.getMgr(), filename));
  }
  std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, InternalFormat format, bool apply_scaling) override {
    unsigned int aw = apply_scaling ? width * getDisplayScale() : width;
    unsigned int ah = apply_scaling ? height * getDisplayScale() : height;
    std::shared_ptr<Surface> ptr(new AndroidSurface(&cache, cache.getJNIEnv(), cache.getMgr(), width, height, aw, ah, format));
    return ptr;
  }
  std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) override {
    std::shared_ptr<Surface> ptr(new AndroidSurface(&cache, cache.getJNIEnv(), cache.getMgr(), buffer, size));
    return ptr;
  }

private:
  AndroidCache cache;
};
}
;

#endif
