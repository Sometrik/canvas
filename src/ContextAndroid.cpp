#include "ContextAndroid.h"

using namespace std;
using namespace canvas;

AndroidSurface::AndroidSurface(AndroidCache * _cache, JNIEnv * _env, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat _format)
  : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format), cache(_cache), env(_env), paint(_cache) {
  // creates an empty canvas
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndroidSurface widthheight constructor called");

  cache->resetCache();
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
    argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
  }
  
  bitmap = (jobject) env->NewGlobalRef(env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod, _actual_width, _actual_height, argbObject));
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, JNIEnv * _env, const Image & image)
  : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), RGBA8), cache(_cache), env(_env), paint(_cache) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface Image constructor");

  cache->resetCache();
  cache->initJava();

  // creates a surface with width, height and contents from image
  bitmap = (jobject) env->NewGlobalRef(imageToBitmap(image));
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, JNIEnv * _env, const std::string & filename)
  : Surface(0, 0, 0, 0, RGBA8), cache(_cache), env(_env), paint(_cache) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "Surface filename constructor");
  
  cache->resetCache();
  cache->initJava();
  
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
}

AndroidSurface::AndroidSurface(AndroidCache * _cache, JNIEnv * _env, const unsigned char * buffer, size_t size)
  : Surface(0, 0, 0, 0, RGBA8), cache(_cache), env(_env), paint(_cache) {

  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndrodiSurface constructor (buffer)  called");
  
  cache->resetCache();
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
  bitmap = (jobject) env->NewGlobalRef(env->CallObjectMethod(firstBitmap, cache->bitmapCopyMethod, argbObject, JNI_TRUE));
  
  int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
  int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
  Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);
  
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndrodiSurface constructor (buffer)  called");  
}

jobject
ContextAndroid::imageToBitmap(const Image & _img) {
  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", " ImageToBitmap called");
  
  const unsigned char * buf = _img.getData();
  
  int length;
  if (_img.getImageFormat() == ImageFormat::RGB565) {
    length = _img.getWidth() * _img.getHeight() * 2;
  } else if (_img.getImageFormat() == ImageFormat::RGB24 || _img.getImageFormat() == ImageFormat::RGB32) {
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
