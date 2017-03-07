#include "ContextAndroid.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <errno.h>
#include <cassert>

using namespace std;
using namespace canvas;

AndroidCache::AndroidCache(JNIEnv * _env, jobject _assetManager) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache java is being initialized");

  _env->GetJavaVM(&javaVM);
  
  JNIEnv * env = getJNIEnv();

  assetManager = _env->NewGlobalRef(_assetManager);

  frameClass = (jclass) env->NewGlobalRef(env->FindClass("com/sometrik/framework/FrameWork"));
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
  stringClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/String"));
  charsetString = (jstring) env->NewGlobalRef(env->NewStringUTF("UTF-8"));
  throwableClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Throwable"));

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
  factoryByteDecodeMethod = env->GetStaticMethodID(factoryClass, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
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
  pathArcToMethod = env->GetMethodID(pathClass, "arcTo", "(Landroid/graphics/RectF;FF)V");
  canvasPathDrawMethod = env->GetMethodID(canvasClass, "drawPath", "(Landroid/graphics/Path;Landroid/graphics/Paint;)V");
  rectFConstructor = env->GetMethodID(rectFClass, "<init>", "(FFFF)V");
  rectConstructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");
  paintSetShadowMethod = env->GetMethodID(paintClass, "setShadowLayer", "(FFFI)V");
  paintSetTextSizeMethod = env->GetMethodID(paintClass, "setTextSize", "(F)V");
  canvasBitmapDrawMethod = env->GetMethodID(canvasClass, "drawBitmap", "(Landroid/graphics/Bitmap;FFLandroid/graphics/Paint;)V");
  canvasBitmapDrawMethod2 = env->GetMethodID(canvasClass, "drawBitmap", "(Landroid/graphics/Bitmap;Landroid/graphics/Rect;Landroid/graphics/RectF;Landroid/graphics/Paint;)V");
  factoryDecodeByteMethod = env->GetStaticMethodID(factoryClass, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
  bitmapGetWidthMethod = env->GetMethodID(bitmapClass, "getWidth", "()I");
  bitmapGetHeightMethod = env->GetMethodID(bitmapClass, "getHeight", "()I");
  bitmapOptionsConstructor = env->GetMethodID(bitmapOptionsClass, "<init>", "()V");
  fileConstructor = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;)V");
  fileInputStreamConstructor = env->GetMethodID(fileInputStreamClass, "<init>", "(Ljava/io/File;)V");
  stringConstructor = env->GetMethodID(stringClass, "<init>", "([BLjava/lang/String;)V");
  stringGetBytesMethod = env->GetMethodID(stringClass, "getBytes", "()[B");
  stringConstructor2 = env->GetMethodID(stringClass, "<init>", "()V");
  errorMethod = env->GetStaticMethodID(frameClass, "handleNativeException", "(Ljava/lang/Throwable;)V");
  getStackTraceMethod = env->GetMethodID(throwableClass, "printStackTrace", "()V");

  optionsMutableField = env->GetFieldID(bitmapOptionsClass, "inMutable", "Z");
  alignEnumRight = env->GetStaticFieldID(alignClass, "RIGHT", "Landroid/graphics/Paint$Align;");
  alignEnumLeft = env->GetStaticFieldID(alignClass, "LEFT", "Landroid/graphics/Paint$Align;");
  alignEnumCenter = env->GetStaticFieldID(alignClass, "CENTER", "Landroid/graphics/Paint$Align;");

  paintStyleEnumStroke = env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "STROKE", "Landroid/graphics/Paint$Style;");
  paintStyleEnumFill = env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "FILL", "Landroid/graphics/Paint$Style;");

  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache java successfully initialized");
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format)
  : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format), cache(_cache), paint(_cache) {
  // creates an empty canvas
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidSurface widthheight constructor called");
  JNIEnv * env = cache->getJNIEnv();

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
    argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
  }

  bitmap = (jobject) env->NewGlobalRef(env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod, _actual_width, _actual_height, argbObject));
  env->DeleteLocalRef(argbObject);
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, const ImageData & image)
  : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), RGBA8), cache(_cache), paint(_cache) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface Image constructor");

  JNIEnv * env = cache->getJNIEnv();

  // creates a surface with width, height and contents from image
  bitmap = (jobject) env->NewGlobalRef(imageToBitmap(image));
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, const std::string & filename)
  : Surface(0, 0, 0, 0, RGBA8), cache(_cache), paint(_cache) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface filename constructor");

  JNIEnv * env = cache->getJNIEnv();

  //Get inputStream from the picture(filename)
  jobject inputStream = env->CallObjectMethod(cache->getAssetManager(), cache->managerOpenMethod, env->NewStringUTF(filename.c_str()));
  
  //Create BitmapFactory options to make the created bitmap mutable straight away
  jobject factoryOptions = env->NewObject(cache->bitmapOptionsClass, cache->bitmapOptionsConstructor);
  env->SetBooleanField(factoryOptions, cache->optionsMutableField, JNI_TRUE);
  
  //Create a bitmap from the inputStream
  bitmap = (jobject) env->NewGlobalRef(env->CallStaticObjectMethod(cache->factoryClass, cache->factoryDecodeMethod2, inputStream, NULL, factoryOptions));
  
  int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
  int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
  Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);

  env->DeleteLocalRef(inputStream);
  env->DeleteLocalRef(factoryOptions);
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, const unsigned char * buffer, size_t size)
  : Surface(0, 0, 0, 0, RGBA8), cache(_cache), paint(_cache) {

  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndrodiSurface constructor (buffer)  called");

  JNIEnv * env = cache->getJNIEnv();
  int arraySize = size;
  
  __android_log_print(ANDROID_LOG_INFO, "Sometrik", "size = %i", size);
  
  jbyteArray array = env->NewByteArray(arraySize);
  env->SetByteArrayRegion(array, 0, arraySize, (const jbyte*) buffer);
  jobject firstBitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryDecodeByteMethod, array, 0, arraySize);
  
  //make this with factory options instead
  jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
  bitmap = (jobject) env->NewGlobalRef(env->CallObjectMethod(firstBitmap, cache->bitmapCopyMethod, argbObject, JNI_TRUE));
  
  int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
  int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
  Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);
  
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndrodiSurface constructor (buffer)  called");  

  env->DeleteLocalRef(argbObject);
  env->DeleteLocalRef(firstBitmap);
  env->DeleteLocalRef(array);
}

jobject
AndroidSurface::imageToBitmap(const ImageData & _img) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", " ImageToBitmap called");

  JNIEnv * env = cache->getJNIEnv();

  const unsigned char * buf = _img.getData();
  int length = _img.calculateOffset(1);

  __android_log_print(ANDROID_LOG_INFO, "Sometrik", "length = %i", length);

  jbyteArray jarray = env->NewByteArray(length);
  env->SetByteArrayRegion(jarray, 0, length, (jbyte*) (buf));

  jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
//  jobject drawableBitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod2, jarray, _img.getWidth(), _img.getHeight(), argbObject);
  jobject drawableBitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryByteDecodeMethod, jarray, length, env->GetArrayLength(jarray));

  if (env->ExceptionCheck()) {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "exception on imageToBitmap");
    jthrowable error = env->ExceptionOccurred();
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "printing trace");
    env->CallVoidMethod(error, cache->getStackTraceMethod);
    throw(error);
  }
  env->CallStaticVoidMethod(cache->frameClass, cache->errorMethod, error);
  env->DeleteLocalRef(error);
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "clearing");
  env->ExceptionClear();
}
  
  env->DeleteLocalRef(argbObject);
  env->DeleteLocalRef(jarray);

  return drawableBitmap;
}

static int android_read(void* cookie, char* buf, int size) {
  return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
  return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
  return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
  AAsset_close((AAsset*)cookie);
  return 0;
}

class AndroidImage : public Image {
public:
  AndroidImage(AAssetManager * _asset_manager, float _display_scale)
    : Image(_display_scale), asset_manager(_asset_manager) { }

    AndroidImage(AAssetManager * _asset_manager, const std::string & filename, float _display_scale)
      : Image(filename, _display_scale), asset_manager(_asset_manager) { }

  AndroidImage(AAssetManager * _asset_manager, const unsigned char * _data, InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels, short _quality, float _display_scale) : Image(_data, _format, _width, _height, _levels, _quality, _display_scale), asset_manager(_asset_manager) { }

protected:
  void loadFile() override {
    AAsset * asset = AAssetManager_open(asset_manager, getFilename().c_str(), 0);
    std::unique_ptr<ImageData> data;
    if (asset) {
      FILE * in = funopen(asset, android_read, android_write, android_seek, android_close);

      basic_string<unsigned char> s;
      while (!feof(in)) {
        unsigned char b[256];
        size_t n = fread(b, 1, 256, in);
        s += basic_string<unsigned char>(b, n);
      }

      data = loadFromMemory(s.data(), s.size());
    } else {
      data = std::unique_ptr<ImageData>(nullptr);
    }
    if (!data.get()) {
      getFilename().clear();
    }
  }

private:
  AndroidCache * cache;
};

std::unique_ptr<Image>
AndroidContextFactory::loadImage(const std::string & filename) {
  return std::unique_ptr<Image>(new AndroidImage(cache.get(), filename, getDisplayScale()));
}

std::unique_ptr<Image>
AndroidContextFactory::createImage() {
  return std::unique_ptr<Image>(new AndroidImage(cache.get(), getDisplayScale()));
}

std::unique_ptr<Image>
AndroidContextFactory::createImage(const unsigned char * _data, InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels, short _quality) {
  return std::unique_ptr<Image>(new AndroidImage(cache.get(), _data, _format, _width, _height, _levels, _quality, getDisplayScale()));
}

std::unique_ptr<Image>
AndroidSurface::createImage(float display_scale) {
  unsigned char * buffer = (unsigned char *)lockMemory(false);
  assert(buffer);

  auto image = std::unique_ptr<Image>(new AndroidImage(cache, buffer, getFormat(), getActualWidth(), getActualHeight(), 1, 0, display_scale));
  releaseMemory();
  
  return image;
}
