#include "ContextAndroid.h"

#include <errno.h>
#include <cassert>

using namespace std;
using namespace canvas;

AndroidCache::AndroidCache(JNIEnv * _env, jobject _assetManager) : myEnv(_env){
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache java is being initialized");

  myEnv->GetJavaVM(&javaVM);

  assetManager = myEnv->NewGlobalRef(_assetManager);

  frameClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("com/sometrik/framework/FrameWork"));
  typefaceClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Typeface"));
  canvasClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Canvas"));
  assetManagerClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/content/res/AssetManager"));
  factoryClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/BitmapFactory"));
  bitmapClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Bitmap"));
  paintClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Paint"));
  pathClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Path"));
  paintStyleClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Paint$Style"));
  alignClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Paint$Align"));
  bitmapConfigClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Bitmap$Config"));
  field_argb_8888 = myEnv->GetStaticFieldID(bitmapConfigClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
  field_rgb_565 = myEnv->GetStaticFieldID(bitmapConfigClass, "RGB_565", "Landroid/graphics/Bitmap$Config;");
  field_alpha_8 = myEnv->GetStaticFieldID(bitmapConfigClass, "ALPHA_8", "Landroid/graphics/Bitmap$Config;");
  rectFClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/RectF"));
  rectClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Rect"));
  bitmapOptionsClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/BitmapFactory$Options"));
  fileClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("java/io/File"));
  fileInputStreamClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("java/io/FileInputStream"));
  stringClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("java/lang/String"));
  charsetString = (jstring) myEnv->NewGlobalRef(myEnv->NewStringUTF("UTF-8"));
  throwableClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("java/lang/Throwable"));
  linearGradientClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/LinearGradient"));
  shaderTileModeClass = (jclass) myEnv->NewGlobalRef(myEnv->FindClass("android/graphics/Shader$TileMode"));
  shaderTileModeMirrorField = myEnv->GetStaticFieldID(shaderTileModeClass, "MIRROR", "Landroid/graphics/Shader$TileMode;");

  measureAscentMethod = myEnv->GetMethodID(paintClass, "ascent", "()F");
  measureDescentMethod = myEnv->GetMethodID(paintClass, "descent", "()F");
  measureTextMethod = myEnv->GetMethodID(paintClass, "measureText", "(Ljava/lang/String;)F");
  setAlphaMethod = myEnv->GetMethodID(paintClass, "setAlpha", "(I)V");
  setTypefaceMethod = myEnv->GetMethodID(paintClass, "setTypeface", "(Landroid/graphics/Typeface;)Landroid/graphics/Typeface;");
  typefaceCreator = myEnv->GetStaticMethodID(typefaceClass, "create", "(Ljava/lang/String;I)Landroid/graphics/Typeface;");
  managerOpenMethod = myEnv->GetMethodID(assetManagerClass, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
  bitmapCreateMethod = myEnv->GetStaticMethodID(bitmapClass, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
  bitmapCreateMethod2 = myEnv->GetStaticMethodID(bitmapClass, "createBitmap", "([IIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
  bitmapCreateScaledMethod = myEnv->GetStaticMethodID(bitmapClass, "createScaledBitmap", "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;");
  textAlignMethod = myEnv->GetMethodID(paintClass, "setTextAlign", "(Landroid/graphics/Paint$Align;)V");
  paintSetColorMethod = myEnv->GetMethodID(paintClass, "setColor", "(I)V");
  paintSetStyleMethod = myEnv->GetMethodID(paintClass, "setStyle", "(Landroid/graphics/Paint$Style;)V");
  paintSetStrokeWidthMethod = myEnv->GetMethodID(paintClass, "setStrokeWidth", "(F)V");
  paintSetStrokeJoinMethod = myEnv->GetMethodID(paintClass, "setStrokeJoin", "(Landroid/graphics/Paint$Join;)V");
  canvasConstructor = myEnv->GetMethodID(canvasClass, "<init>", "(Landroid/graphics/Bitmap;)V");
  factoryDecodeMethod = myEnv->GetStaticMethodID(factoryClass, "decodeStream", "(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");
  factoryByteDecodeMethod = myEnv->GetStaticMethodID(factoryClass, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
  factoryDecodeMethod2 = myEnv->GetStaticMethodID(factoryClass, "decodeStream", "(Ljava/io/InputStream;Landroid/graphics/Rect;Landroid/graphics/BitmapFactory$Options;)Landroid/graphics/Bitmap;");
  bitmapCopyMethod = myEnv->GetMethodID(bitmapClass, "copy", "(Landroid/graphics/Bitmap$Config;Z)Landroid/graphics/Bitmap;");
  paintConstructor = myEnv->GetMethodID(paintClass, "<init>", "()V");
  paintSetAntiAliasMethod = myEnv->GetMethodID(paintClass, "setAntiAlias", "(Z)V");
  pathMoveToMethod = myEnv->GetMethodID(pathClass, "moveTo", "(FF)V");
  pathConstructor = myEnv->GetMethodID(pathClass, "<init>", "()V");
  textAlignMethod = myEnv->GetMethodID(paintClass, "setTextAlign", "(Landroid/graphics/Paint$Align;)V");
  canvasTextDrawMethod = myEnv->GetMethodID(canvasClass, "drawText", "(Ljava/lang/String;FFLandroid/graphics/Paint;)V");
  pathLineToMethod = myEnv->GetMethodID(pathClass, "lineTo", "(FF)V");
  pathCloseMethod = myEnv->GetMethodID(pathClass, "close", "()V");
  pathArcToMethod = myEnv->GetMethodID(pathClass, "arcTo", "(Landroid/graphics/RectF;FF)V");
  canvasPathDrawMethod = myEnv->GetMethodID(canvasClass, "drawPath", "(Landroid/graphics/Path;Landroid/graphics/Paint;)V");
  rectFConstructor = myEnv->GetMethodID(rectFClass, "<init>", "(FFFF)V");
  rectConstructor = myEnv->GetMethodID(rectClass, "<init>", "(IIII)V");
  paintSetShadowMethod = myEnv->GetMethodID(paintClass, "setShadowLayer", "(FFFI)V");
  paintSetTextSizeMethod = myEnv->GetMethodID(paintClass, "setTextSize", "(F)V");
  canvasBitmapDrawMethod = myEnv->GetMethodID(canvasClass, "drawBitmap", "(Landroid/graphics/Bitmap;FFLandroid/graphics/Paint;)V");
  canvasBitmapDrawMethod2 = myEnv->GetMethodID(canvasClass, "drawBitmap", "(Landroid/graphics/Bitmap;Landroid/graphics/Rect;Landroid/graphics/RectF;Landroid/graphics/Paint;)V");
  factoryDecodeByteMethod = myEnv->GetStaticMethodID(factoryClass, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
  bitmapGetWidthMethod = myEnv->GetMethodID(bitmapClass, "getWidth", "()I");
  bitmapGetHeightMethod = myEnv->GetMethodID(bitmapClass, "getHeight", "()I");
  bitmapOptionsConstructor = myEnv->GetMethodID(bitmapOptionsClass, "<init>", "()V");
  fileConstructor = myEnv->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;)V");
  fileInputStreamConstructor = myEnv->GetMethodID(fileInputStreamClass, "<init>", "(Ljava/io/File;)V");
  stringConstructor = myEnv->GetMethodID(stringClass, "<init>", "([BLjava/lang/String;)V");
  stringGetBytesMethod = myEnv->GetMethodID(stringClass, "getBytes", "()[B");
  stringConstructor2 = myEnv->GetMethodID(stringClass, "<init>", "()V");
  stringByteConstructor = myEnv->GetMethodID(stringClass, "<init>", "([BLjava/lang/String;)V");
  errorMethod = myEnv->GetStaticMethodID(frameClass, "handleNativeException", "(Ljava/lang/Throwable;)V");
  getStackTraceMethod = myEnv->GetMethodID(throwableClass, "printStackTrace", "()V");
  paintSetShaderMethod = myEnv->GetMethodID(paintClass, "setShader", "(Landroid/graphics/Shader;)Landroid/graphics/Shader;");
  linearGradientConstructor = myEnv->GetMethodID(linearGradientClass, "<init>", "(FFFFIILandroid/graphics/Shader$TileMode;)V");

  optionsMutableField = myEnv->GetFieldID(bitmapOptionsClass, "inMutable", "Z");
  alignEnumRight = myEnv->GetStaticFieldID(alignClass, "RIGHT", "Landroid/graphics/Paint$Align;");
  alignEnumLeft = myEnv->GetStaticFieldID(alignClass, "LEFT", "Landroid/graphics/Paint$Align;");
  alignEnumCenter = myEnv->GetStaticFieldID(alignClass, "CENTER", "Landroid/graphics/Paint$Align;");
  factoryOptions = myEnv->NewGlobalRef(myEnv->NewObject(bitmapOptionsClass, bitmapOptionsConstructor));
  myEnv->SetBooleanField(factoryOptions, optionsMutableField, JNI_TRUE);

  paintStyleEnumStroke = myEnv->GetStaticFieldID(myEnv->FindClass("android/graphics/Paint$Style"), "STROKE", "Landroid/graphics/Paint$Style;");
  paintStyleEnumFill = myEnv->GetStaticFieldID(myEnv->FindClass("android/graphics/Paint$Style"), "FILL", "Landroid/graphics/Paint$Style;");

  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidCache java successfully initialized");
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format)
  : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format), cache(_cache), paint(_cache) {
  // creates an empty canvas

  if (_actual_width && _actual_height) {
    __android_log_print(ANDROID_LOG_INFO, "Sometrik", "AndroidSurface widthheight constructor called with width : height %u : %u", _actual_width, _actual_height);
    JNIEnv * env = getEnv();

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

    jobject localBitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod, _actual_width, _actual_height, argbObject);
    if (localBitmap) {
      bitmap = (jobject) env->NewGlobalRef(localBitmap);
      env->DeleteLocalRef(localBitmap);
    } else {
      bitmap = 0;
    }
    env->DeleteLocalRef(argbObject);
  }
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, const ImageData & image)
  : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), RGBA8), cache(_cache), paint(_cache) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface Image constructor");

  JNIEnv * env = getEnv();

  // creates a surface with width, height and contents from image
  jobject localBitmap = (jobject) env->NewGlobalRef(imageToBitmap(image));
  if (localBitmap) {
    bitmap = (jobject) env->NewGlobalRef(imageToBitmap(image));
    env->DeleteLocalRef(localBitmap);
  }
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, const std::string & filename)
  : Surface(0, 0, 0, 0, RGBA8), cache(_cache), paint(_cache) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface filename constructor");

  JNIEnv * env = getEnv();

  //Get inputStream from the picture(filename)
  jstring jfilename = env->NewStringUTF(filename.c_str());
  jobject inputStream = env->CallObjectMethod(cache->getAssetManager(), cache->managerOpenMethod, jfilename);
  env->DeleteLocalRef(jfilename);
  
  //Create a bitmap from the inputStream
  jobject localBitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryDecodeMethod2, inputStream, NULL, cache->getFactoryOptions());
  bitmap = (jobject) env->NewGlobalRef(localBitmap);
  env->DeleteLocalRef(localBitmap);
  
  int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
  int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
  Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);

  env->DeleteLocalRef(inputStream);
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, const unsigned char * buffer, size_t size)
  : Surface(0, 0, 0, 0, RGBA8), cache(_cache), paint(_cache) {

  JNIEnv * env = getEnv();
  int arraySize = size;
  
  jbyteArray array = env->NewByteArray(arraySize);
  env->SetByteArrayRegion(array, 0, arraySize, (const jbyte*) buffer);
  jobject firstBitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryDecodeByteMethod, array, 0, arraySize);
  
  //make this with factory options instead
  jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
  jobject localBitmap = env->CallObjectMethod(firstBitmap, cache->bitmapCopyMethod, argbObject, JNI_TRUE);
  bitmap = (jobject) env->NewGlobalRef(localBitmap);
  env->DeleteLocalRef(localBitmap);
  
  int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
  int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
  Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);
  
  env->DeleteLocalRef(argbObject);
  env->DeleteLocalRef(firstBitmap);
  env->DeleteLocalRef(array);
}

jobject
AndroidSurface::imageToBitmap(const ImageData & _img) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", " ImageToBitmap called");

  if (_img.getInternalFormat() != InternalFormat::RGBA4 && _img.getInternalFormat() != InternalFormat::RGBA8 && _img.getInternalFormat() != InternalFormat::RGBA_DXT5){
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Wrong internalFormat detected. Converting");
    _img.convert(InternalFormat::RGBA8);
  }

  JNIEnv * env = getEnv();

  const unsigned char * buf = _img.getData();
  int length = _img.calculateOffset(1);

  __android_log_print(ANDROID_LOG_INFO, "Sometrik", "Image Width = %u", _img.getWidth());
  __android_log_print(ANDROID_LOG_INFO, "Sometrik", "Image height = %u", _img.getHeight());
  __android_log_print(ANDROID_LOG_INFO, "Sometrik", "length = %i", length);
  __android_log_print(ANDROID_LOG_INFO, "Sometrik", "internalFormat = %i", _img.getInternalFormat());

  jintArray jarray = env->NewIntArray(length / sizeof(int));
  env->SetIntArrayRegion(jarray, 0, length / sizeof(int), (jint*) (buf));
  jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
  jobject drawableBitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod2, jarray, _img.getWidth(), _img.getHeight(), argbObject);
//  jobject drawableBitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryByteDecodeMethod, jarray, 0, length);

  if (env->ExceptionCheck()) {
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "exception on imageToBitmap");
    jthrowable error = env->ExceptionOccurred();
    __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "printing trace");
    env->ExceptionClear();
//    env->CallVoidMethod(error, cache->getStackTraceMethod);
//    throw(error);
  env->CallStaticVoidMethod(cache->frameClass, cache->errorMethod, error);
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "clearing");
  env->DeleteLocalRef(error);
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
    if (asset_manager) {
      AAsset * asset = AAssetManager_open(asset_manager, getFilename().c_str(), 0);
      if (asset) {
        FILE * in = funopen(asset, android_read, android_write, android_seek, android_close);

        basic_string<unsigned char> s;
        while (!feof(in)) {
          unsigned char b[256];
          size_t n = fread(b, 1, 256, in);
          s += basic_string<unsigned char>(b, n);
        }
        fclose(in);

        __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "image %s loaded successfully: %d", getFilename().c_str(), int(s.size()));

        data = loadFromMemory(s.data(), s.size());
        __android_log_print(ANDROID_LOG_INFO, "Sometrik", "Image Width = %u", data->getWidth());
        __android_log_print(ANDROID_LOG_INFO, "Sometrik", "Image height = %u", data->getHeight());
      }
    }
    if (!data.get()) {
      __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "image %s loading FAILED", getFilename().c_str());
      getFilename().clear();
    }
  }

private:
  AAssetManager * asset_manager;
};

std::unique_ptr<Image>
AndroidContextFactory::loadImage(const std::string & filename) {
  return std::unique_ptr<Image>(new AndroidImage(asset_manager, filename, getDisplayScale()));
}

std::unique_ptr<Image>
AndroidContextFactory::createImage() {
  return std::unique_ptr<Image>(new AndroidImage(asset_manager, getDisplayScale()));
}

std::unique_ptr<Image>
AndroidContextFactory::createImage(const unsigned char * _data, InternalFormat _format, unsigned int _width, unsigned int _height, unsigned int _levels, short _quality) {
  return std::unique_ptr<Image>(new AndroidImage(0, _data, _format, _width, _height, _levels, _quality, getDisplayScale()));
}

std::unique_ptr<Image>
AndroidSurface::createImage(float display_scale) {
  std::unique_ptr<Image> image;
  unsigned char * buffer = (unsigned char *)lockMemory(false);
  assert(buffer);
  if (buffer) {
    image = std::unique_ptr<Image>(new AndroidImage(0, buffer, getFormat(), getActualWidth(), getActualHeight(), 1, 0, display_scale));
    releaseMemory();
  }
  return image;
}
