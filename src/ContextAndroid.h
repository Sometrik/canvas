#ifndef _CANVAS_CONTEXTANDROID_H_
#define _CANVAS_CONTEXTANDROID_H_

#include "Context.h"

#include <cassert>
#include <sstream>
#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>

namespace canvas { 
  class AndroidSurface : public Surface {
  public:
    friend class ContextAndroid;

    void testCode() {

    	jboolean copyBoolean = JNI_TRUE;

  	  //RenderPath debug
  	  //create paint
  	  //jmethodID paintConstructor = env->GetMethodID(paintClass, "<init>", "()V");
  	  jobject jpaint = env->NewObject(paintClass, env->GetMethodID(paintClass, "<init>", "()V"));

  	  //Paint.setColor;
  	  env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setAntiAlias", "(Z)V"), copyBoolean);

  	  //Paint Set Style
  	  env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setStyle", "(Landroid/graphics/Paint$Style;)V"),
  			  env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"),
  					  env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style") , "STROKE", "Landroid/graphics/Paint$Style;")));

  	  //Paint Set Stroke Width
  	  env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setStrokeWidth", "(F)V"), 5.0f);
  	  //Paint set Color
  	  env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setColor", "(I)V"), -16776961);
  	  //Paint set StrokeJoin
  	  env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setStrokeJoin", "(Landroid/graphics/Paint$Join;)V"),
  			  env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Join"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Join"),
  					  "ROUND", "Landroid/graphics/Paint$Join;")));

  	  //Set more Paint things here------<

  	  jclass pathClass = env->FindClass("android/graphics/Path");
  	  //jmethodID pathConstructor = env->GetMethodID(pathClass, "<init>", "()V");
  	  	 		  jobject jpath = env->NewObject(pathClass, env->GetMethodID(pathClass, "<init>", "()V"));

  	  	 		  // if op == 1 example   Move Path
  		// jmethodID pathMoveMethod = env->GetMethodID(pathClass, "moveTo", "(FF)V");
  			env->CallVoidMethod(jpath, env->GetMethodID(pathClass, "moveTo", "(FF)V"), 100.0f,100.0f);

  			//Path Line to debug
  		//jmethodID pathLineToMethod = env->GetMethodID(pathClass, "lineTo", "(FF)V");
  		env->CallVoidMethod(jpath, env->GetMethodID(pathClass, "lineTo", "(FF)V"), 300.0f,300.0f);


  		  //Draw path to canvas
  		  //jmethodID canvasPathDraw = env->GetMethodID(canvasClass, "drawPath", "(Landroid/graphics/Path;Landroid/graphics/Paint;)V");
  		  env->CallVoidMethod(canvas, env->GetMethodID(canvasClass, "drawPath", "(Landroid/graphics/Path;Landroid/graphics/Paint;)V"), jpath, jpaint);


  			//path close debug
  		//jmethodID pathCloseMethod = env->GetMethodID(pathClass, "close", "()V");
  		env->CallVoidMethod(jpath, env->GetMethodID(pathClass, "close", "()V"));

  		  //DEBUG call java debug method to check path, paint or canvas
  		jclass debugClass = env->FindClass("com/example/work/MyGLSurfaceView");
  		  jmethodID debugMethod = env->GetStaticMethodID(debugClass, "pathDebug", "(Landroid/graphics/Path;Landroid/graphics/Paint;Landroid/graphics/Canvas;)V");
  			env->CallStaticVoidMethod(debugClass, debugMethod, jpath, jpaint, canvas);



  #if 0
  		//AndroidBitmapInfo info;

  		//AndroidBitmap_getInfo(env, bitmap, &info);

  		//bool has_alpha = info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGBA_4444;
  		//	Surface::resize(info.width, info.height, info.width, info.height, has_alpha);


  	  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "This is...");
  	  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "...not a problem");

  #endif

    }

  AndroidSurface(JNIEnv * _env, jobject _mgr, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, const ImageFormat & _format)
    : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format.hasAlpha()), env(_env), mgr(_mgr) {
	  // creates an empty canvas

	  // Bitmap.Config conf = Bitmap.Config.ARGB_8888;
	  // Bitmap bmp = Bitmap.createBitmap(w, h, conf);
	  // Canvas canvas = new Canvas(bmp);

	  initJavaClasses();

	  jclass clSTATUS    = env->FindClass("android/graphics/Bitmap$Config");
	  jfieldID fidONE    = env->GetStaticFieldID(clSTATUS , "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
	  jobject STATUS_ONE = env->GetStaticObjectField(clSTATUS, fidONE);

	  //Not Tested
	  jmethodID bitmapCreate = env->GetStaticMethodID(bitmapClass, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
	  bitmap = env->CallStaticObjectMethod(bitmapClass, bitmapCreate, _actual_width, _actual_height, STATUS_ONE);

	  //Create new Canvas from the mutable bitmap
	  jmethodID canvasConstructor = env->GetMethodID(canvasClass, "<init>", "(Landroid/graphics/Bitmap;)V");
	  canvas = env->NewObject(canvasClass, canvasConstructor, bitmap);

	  testCode();
  }
  
  AndroidSurface(JNIEnv * _env, jobject _mgr, const Image & image)
    : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getFormat().hasAlpha()), env(_env), mgr(_mgr) {
	  // creates a surface with width, height and contents from image
    }
    
  AndroidSurface(JNIEnv * _env, jobject _mgr, const std::string & filename) : Surface(0, 0, 0, 0, false), env(_env), mgr(_mgr) {


	  initJavaClasses();
	  //Get inputStream from the picture(filename)
	  //jmethodID streamingMethod = env->GetMethodID(mgrClass, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
	  jobject inputStream = env->CallObjectMethod(mgr, env->GetMethodID(mgrClass, "open",
			  "(Ljava/lang/String;)Ljava/io/InputStream;"), env->NewStringUTF(filename.c_str()));

	  //Create a bitmap from the inputStream
	  jmethodID factoryMethod = env->GetStaticMethodID(factoryClass, "decodeStream","(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");
	  jobject firstBitmap = env->CallStaticObjectMethod(factoryClass, factoryMethod, inputStream);

	  //Make bitmap mutable by calling Copy Method with setting isMutable() to true
	  jmethodID bitmapCopyMethod = env->GetMethodID(bitmapClass, "copy", "(Landroid/graphics/Bitmap$Config;Z)Landroid/graphics/Bitmap;");
	  jclass clSTATUS    = env->FindClass("android/graphics/Bitmap$Config");
	  jfieldID fidONE    = env->GetStaticFieldID(clSTATUS , "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
	  jobject STATUS_ONE = env->GetStaticObjectField(clSTATUS, fidONE);
	  jboolean copyBoolean = JNI_TRUE;
	  bitmap = env->CallObjectMethod(firstBitmap, bitmapCopyMethod, STATUS_ONE, copyBoolean);

	  //Create new Canvas from the mutable bitmap
	  jmethodID canvasConstructor = env->GetMethodID(canvasClass, "<init>", "(Landroid/graphics/Bitmap;)V");
	  canvas = env->NewObject(canvasClass, canvasConstructor, bitmap);

	  testCode();
    }
    
  AndroidSurface(JNIEnv * _env, jobject _mgr, const unsigned char * buffer, size_t size) : Surface(0, 0, 0, 0, false), env(_env), mgr(_mgr) {
	  // creates a surface from raw data
	  // use this: decodeByteArray(byte[] data, int offset, int length)
	  // make some wizardry: convert C byte array buffer to Java byte array data
	  // BitmapFactory.decoreByteArray
	  // create canvas

	  //Create a bitmap from bytearray
      //Will propably fatal error 11, size probaly needs to be converted for java.

	//  jbyteArray array = env->NewByteArray (size);
	//      env->SetByteArrayRegion (array, 0, size, reinterpret_cast<jbyte*>(buffer));
	//      bitmap = env->CallStaticObjectMethod(env->FindClass("android/graphics/BitmapFactory"),
	//    		  env->GetStaticMethodID(env->FindClass("android/graphics/BitmapFactory"), "decodeByteArray", "([BII)Landroid/graphics/Bitmap;"), array, 0, size);

    }
    
    ~AndroidSurface() {
       // remember to free canvas and bitmap
    }

    void * lockMemory(bool write_access = false) override {

		uint32_t *pixels;
    	AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));

      return pixels;
    }
    
    void releaseMemory() override {
    	// is there AndroidBitmap_releasePixels?
    }

    void initJavaClasses(){

     canvasClass = env->FindClass("android/graphics/Canvas");
  	  mgrClass = env->FindClass("android/content/res/AssetManager");
	  factoryClass = env->FindClass("android/graphics/BitmapFactory");
	   bitmapClass = env->FindClass("android/graphics/Bitmap");
	   paintClass = env->FindClass("android/graphics/Paint");
	   pathClass = env->FindClass("android/graphics/Path");

    }

    jobject createJavaPaint(RenderMode mode, const Style & style, float lineWidth, float globalAlpha){

    	 //create paint

  	  jboolean copyBoolean = JNI_TRUE;

    jobject jpaint = env->NewObject(paintClass, env->GetMethodID(paintClass, "<init>", "()V"));

     //Paint.setColor;
    env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setAntiAlias", "(Z)V"), copyBoolean);

     //Paint Set Style
     switch (mode) {
     case STROKE:
      env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setStyle", "(Landroid/graphics/Paint$Style;)V"),
      env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"),
      env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style") , "STROKE", "Landroid/graphics/Paint$Style;")));
      	 //Paint Set Stroke Width
     env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setStrokeWidth", "(F)V"), lineWidth);
      	//Paint set StrokeJoin
     env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setStrokeJoin", "(Landroid/graphics/Paint$Join;)V"),
   env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Join"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Join"),
    	    		  	   "ROUND", "Landroid/graphics/Paint$Join;")));
    	break;
     case FILL:
    env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setStyle", "(Landroid/graphics/Paint$Style;)V"),
    	    		 env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"),
    	    		env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style") , "FILL", "Landroid/graphics/Paint$Style;")));

    	break;
    	  }
    	  //Paint set Color
    	env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setColor", "(I)V"), getAndroidColor(style.color, globalAlpha));

    	//Set more Paint things here------<
    	return jpaint;
    }

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha) override {

    	jobject jpaint = createJavaPaint(mode, style, lineWidth, globalAlpha);

    	  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "This is...");
    	  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "...not a problem");
	  //debug of renderpath-----------------------
	 	 // renderPath(1, , 1, 10, 1, 10, 10)

  	  jboolean copyBoolean = JNI_TRUE;

	 	//RenderPath debug
  	  jmethodID pathConstructor = env->GetMethodID(pathClass, "<init>", "()V");
  	  jobject jpath = env->NewObject(pathClass, pathConstructor);


    	for (auto pc : path.getData()) {
    		switch (pc.type) {
    		case PathComponent::MOVE_TO:{
    			jmethodID pathMoveMethod = env->GetMethodID(pathClass, "moveTo", "(FF)V");
    			env->CallVoidMethod(jpath, pathMoveMethod, pc.x0, pc.y0);
    		}
    		break;
    		case PathComponent::LINE_TO:{
    			jmethodID pathLineToMethod = env->GetMethodID(pathClass, "lineTo", "(FF)V");
    			env->CallVoidMethod(jpath, pathLineToMethod, pc.x0, pc.y0);
    		 }
    		break;
    		case PathComponent::ARC: {//CGContextAddArc(gc, pc.x0 * scale + 0.5, pc.y0 * scale + 0.5, pc.radius * scale, pc.sa, pc.ea, pc.anticlockwise);

    		}
    		break;
    		case PathComponent::CLOSE:{
    			jmethodID pathCloseMethod = env->GetMethodID(pathClass, "close", "()V");
    			env->CallVoidMethod(jpath, pathCloseMethod);
    		}
    		break;
    		}
    	 }

    	//Draw path to canvas
    	jmethodID canvasPathDraw = env->GetMethodID(canvasClass, "drawPath", "(Landroid/graphics/Path;Landroid/graphics/Paint;)V");
    	env->CallVoidMethod(canvas, canvasPathDraw, jpath, jpaint);


    	jclass debugClass = env->FindClass("com/example/work/MyGLSurfaceView");
    	jmethodID debugMethod = env->GetStaticMethodID(debugClass, "pathDebug", "(Landroid/graphics/Path;Landroid/graphics/Paint;Landroid/graphics/Canvas;)V");
    	env->CallStaticVoidMethod(debugClass, debugMethod, jpath, jpaint, canvas);

	  	// render path with style
    	// Paint paint = createPaintFromStyle(mode, style, globalAlpha, lineWidth);
    	// jobject android_path = createAndroidPath(path);
    	// drawPath(android_path, paint)
    }

    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool _has_alpha) override {
      Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _has_alpha);
      // do resize the surface and discard the old data
    }
    
    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, Operator op, float display_scale, float globalAlpha) override {

    jobject jpaint = createJavaPaint(mode, style, lineWidth, globalAlpha);

    jclass canvasClass = env->FindClass("android/graphics/Canvas");

  	jmethodID canvasTextDraw = env->GetMethodID(canvasClass, "drawText", "(Ljava/lang/String;FFLandroid/graphics/Paint;)V");
	env->CallVoidMethod(canvas, canvasTextDraw, env->NewStringUTF(text.c_str()), x, y, jpaint);

    }

    TextMetrics measureText(const Font & font, const std::string & text, float display_scale) override {
      // measure width of text
      return TextMetrics(0);
    }

    void drawImage(Surface & _surface, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) override {
    	AndroidSurface & surface = dynamic_cast<AndroidSurface&>(_surface);
    	// draw surface as image
    	// surface.getBitmap();
    }
    
    void drawImage(const Image & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) override {
    	// _img.getWidth() _img.getHeight()
        // static Bitmap 	createBitmap(DisplayMetrics display, int[] colors, int width, int height, Bitmap.Config config)
    }
    void clip(const Path & path, float display_scale) override {
       // restrict drawing operations to path
    }
    void resetClip() override {
    }
    void save() override {
    	// forget this
    }
    void restore() override {
    	// forget this
    }

    jobject getBitmap() { return bitmap; }
    jobject getCanvas() { return canvas; }

  protected:
    static int getAndroidColor(const Color & color, float globalAlpha = 1.0f) {
    	return (int(color.alpha * globalAlpha * 0xff) << 24) | (int(color.red * 0xff) << 16) | (int(color.green * 0xff) << 8) | int(color.blue * 0xff);
    }

    // static jobject createPaintFromStyle(RenderMode mode, const Style & style, float globalAlpha, float lineWidth) {
    //	mPaint = new Paint();
    //		mPaint.setAntiAlias(true); // always
    //		mPaint.setColor(Color.BLUE); // style.color.red, style.color.green, style
    //		mPaint.setStrokeJoin(Paint.Join.ROUND);
    //		mPaint.setStrokeWidth(3f);
    //		// mPaint.setTextSize(40f);
    //      if (mode == FILL) {
    //		  mPaint.setStyle(Paint.Style.FILL);
    //      } else if (mode == STROKE) {
    //		  mPaint.setStyle(Paint.Style.STROKE);
    //      }
    // }

    static jobject createAndroidPath(const Path & path) {
    	// jobject android_path = new Path();
    	for (auto pc : path.getData()) {
	    switch (pc.type) {
	    case PathComponent::MOVE_TO:
	    	// esimerkki: CGContextMoveToPoint(gc, pc.x0 * scale + 0.5, pc.y0 * scale + 0.5);
	    	// android_path.moveTo(float x, float y)
	        break;
	    case PathComponent::LINE_TO:
	    	// esimerkki: CGContextAddLineToPoint(gc, pc.x0 * scale + 0.5, pc.y0 * scale + 0.5);
	    	// android_path.lineTo(float x, float y)
	    	break;
	    case PathComponent::ARC:
	    	// eximerkki: CGContextAddArc(gc, pc.x0 * scale + 0.5, pc.y0 * scale + 0.5, pc.radius * scale, pc.sa, pc.ea, pc.anticlockwise); break;
	     	// android_path.arcTo(float left, float top, float right, float bottom, float startAngle, float sweepAngle, boolean forceMoveTo)
	    	break;
	    case PathComponent::CLOSE:
	    	// android_path.close();
	    	break;
	    }
    	}
    }

  private:
    JNIEnv * env;
    jobject mgr;
    jobject bitmap;
    jobject canvas;
    jclass canvasClass;
    jclass paintClass;
    jclass pathClass;
    jclass bitmapClass;
    jclass mgrClass;
    jclass factoryClass;
  };

  class ContextAndroid : public Context {
  public:
  ContextAndroid(JNIEnv * _env, jobject _mgr, unsigned int _width, unsigned int _height, const ImageFormat & format, float _display_scale)
    : Context(_display_scale), env(_env), mgr(_mgr),
      default_surface(_env, _mgr,  _width, _height, (unsigned int)(_width * _display_scale), (unsigned int)(_height * _display_scale), format)
      {
      }

    std::shared_ptr<Surface> createSurface(const Image & image) override {
      return std::shared_ptr<Surface>(new AndroidSurface(env, mgr, image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const ImageFormat & _format) override {
      return std::shared_ptr<Surface>(new AndroidSurface(env, mgr, _width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), _format));
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      return std::shared_ptr<Surface>(new AndroidSurface(env, mgr, filename));
    }
        
    Surface & getDefaultSurface() override { return default_surface; }
    const Surface & getDefaultSurface() const override { return default_surface; }
      
//    Context & drawImage(const Image & img, double x, double y, double w, double h) override {
//      return *this;
//    }
//
//    Context & drawImage(Surface & img, double x, double y, double w, double h) override {
//      return *this;
//    }
    
  protected:
 //   Context & renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op) override {
//
//   	return *this;
//    }
 //
//    Context & renderPath(RenderMode mode, const Path & path, const Style & style, Operator op) override {
//
 //   	return *this;
 //   }
    
  private:
    JNIEnv * env;
    jobject mgr;
    AndroidSurface default_surface;
  };

  class AndroidContextFactory : public ContextFactory {
  public:
    AndroidContextFactory(JNIEnv * _env, jobject _mgr, float _display_scale = 1.0f) : ContextFactory(_display_scale), env(_env), mgr(_mgr) { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling = false) override {
      std::shared_ptr<Context> ptr(new ContextAndroid(env, mgr, width, height, format, apply_scaling ? getDisplayScale() : 1.0f));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      return std::shared_ptr<Surface>(new AndroidSurface(env, mgr, filename));
    }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling) override {
      unsigned int aw = apply_scaling ? width * getDisplayScale() : width;
      unsigned int ah = apply_scaling ? height * getDisplayScale() : height;
      std::shared_ptr<Surface> ptr(new AndroidSurface(env, mgr, width, height, aw, ah, format));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) override {
      std::shared_ptr<Surface> ptr(new AndroidSurface(env, mgr, buffer, size));
      return ptr;
    }
    
  private:
    JNIEnv * env;
    jobject mgr;
  };
};

#endif
