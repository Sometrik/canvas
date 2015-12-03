#ifndef _CANVAS_CONTEXTANDROID_H_
#define _CANVAS_CONTEXTANDROID_H_

#include "Context.h"

#include <cassert>
#include <sstream>
#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>

namespace canvas { 
  class AndroidCache {
  public:
  	AndroidCache(JNIEnv * _env, jobject _mgr) : env(_env), mgr(_mgr) {

  		initJavaClasses();
  		initJavaMethods();


  		//arcToMethod = blablah();
  		//if (arcToMethod && pathConstructor) {
  		//	is_valid = true;
  		//}
  	}

  	void initJavaMethods(){

  	managerOpenMethod = env->GetMethodID(mgrClass, "open","(Ljava/lang/String;)Ljava/io/InputStream;");
		bitmapCreateMethod = env->GetStaticMethodID(bitmapClass, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
		textAlignMethod = env->GetMethodID(paintClass, "setTextAlign", "(Landroid/graphics/Paint$Align;)V");
		paintSetColorMethod = env->GetMethodID(paintClass, "setColor", "(I)V");
		paintSetStyleMethod = env->GetMethodID(paintClass, "setStyle", "(Landroid/graphics/Paint$Style;)V");
		paintSetStrokeWidthMethod = env->GetMethodID(paintClass, "setStrokeWidth", "(F)V");
		paintSetStrokeJoinMethod = env->GetMethodID(paintClass, "setStrokeJoin", "(Landroid/graphics/Paint$Join;)V");
		canvasConstructor = env->GetMethodID(canvasClass, "<init>", "(Landroid/graphics/Bitmap;)V");
		factoryDecodeMethod = env->GetStaticMethodID(factoryClass, "decodeStream","(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");
		bitmapCopyMethod = env->GetMethodID(bitmapClass, "copy", "(Landroid/graphics/Bitmap$Config;Z)Landroid/graphics/Bitmap;");
		paintConstructor = env->GetMethodID(paintClass, "<init>", "()V");
		paintSetAntiAliasMethod = env->GetMethodID(paintClass, "setAntiAlias", "(Z)V");
		pathMoveToMethod = env->GetMethodID(pathClass, "moveTo", "(FF)V");
		pathConstructor = env->GetMethodID(pathClass, "<init>", "()V");
		textAlignMethod = env->GetMethodID(paintClass, "setTextAlign", "(Landroid/graphics/Paint$Align;)V");
		canvasTextDrawMethod = env->GetMethodID(canvasClass, "drawText", "(Ljava/lang/String;FFLandroid/graphics/Paint;)V");
		pathLineToMethod = env->GetMethodID(pathClass, "lineTo", "(FF)V");
		pathCloseMethod = env->GetMethodID(pathClass, "close","()V");
		canvasPathDrawMethod = env->GetMethodID(canvasClass, "drawPath", "(Landroid/graphics/Path;Landroid/graphics/Paint;)V");
		rectConstructor = env->GetMethodID(rectClass, "<init>","(FFFF)V");

  	}

    void initJavaClasses(){

		canvasClass = env->FindClass("android/graphics/Canvas");
		mgrClass = env->FindClass("android/content/res/AssetManager");
		factoryClass = env->FindClass("android/graphics/BitmapFactory");
		bitmapClass = env->FindClass("android/graphics/Bitmap");
		paintClass = env->FindClass("android/graphics/Paint");
		pathClass = env->FindClass("android/graphics/Path");
		paintStyleClass = env->FindClass("android/graphics/Paint$Style");
		alignClass = env->FindClass("android/graphics/Paint$Align");
		clSTATUS  = env->FindClass("android/graphics/Bitmap$Config");
		fidONE  = env->GetStaticFieldID(clSTATUS , "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
		rectClass  = env->FindClass("android/graphics/RectF");

      }

  	bool isValid() const { return is_valid; }

  	//JNIEnv * env;
  	//blab * mgr;
  	//Jnimethodid arcToMethod;
  	jmethodID paintSetStyleMethod;
  	jmethodID paintSetStrokeWidthMethod;
  	jmethodID paintSetStrokeJoinMethod;
  	jmethodID textAlignMethod;
  	jmethodID paintSetColorMethod;
  	jmethodID bitmapCreateMethod;
  	jmethodID canvasConstructor;
  	jmethodID managerOpenMethod;
  	jmethodID factoryDecodeMethod;
  	jmethodID bitmapCopyMethod;
  	jmethodID paintConstructor;
  	jmethodID paintSetAntiAliasMethod;
  	jmethodID pathMoveToMethod;
  	jmethodID pathConstructor;
  	jmethodID canvasTextDrawMethod;
  	jmethodID pathLineToMethod;
  	jmethodID pathCloseMethod;
  	jmethodID canvasPathDrawMethod;
  	jmethodID rectConstructor;

  	jclass rectClass;
    jclass canvasClass;
    jclass paintClass;
    jclass pathClass;
    jclass bitmapClass;
    jclass mgrClass;
    jclass factoryClass;
    jclass paintStyleClass;
    jclass alignClass;
    jclass clSTATUS;
    jfieldID fidONE;


  private:
	JNIEnv * env;
    jobject mgr;
  	bool is_valid = false;
  };
  class AndroidSurface : public Surface {
  public:
    friend class ContextAndroid;



  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, const ImageFormat & _format)
    : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format.hasAlpha()) , cache(_cache), env(_env), mgr(_mgr) {
	  // creates an empty canvas

	  // Bitmap.Config conf = Bitmap.Config.ARGB_8888;
	  // Bitmap bmp = Bitmap.createBitmap(w, h, conf);
	  // Canvas canvas = new Canvas(bmp);

	 jobject STATUS_ONE = env->GetStaticObjectField(cache->clSTATUS, cache->fidONE);

	  //Not Tested
	  bitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod, _actual_width, _actual_height, STATUS_ONE);

	  //Create new Canvas from the mutable bitmap
	   canvas = env->NewObject(cache->canvasClass, cache->canvasConstructor, bitmap);

	  //testCode();
  }
  
  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const Image & image)
    : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getFormat().hasAlpha()), cache(_cache), env(_env), mgr(_mgr) {
	  // creates a surface with width, height and contents from image
    }
    
  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const std::string & filename) : Surface(0, 0, 0, 0, false), cache(_cache), env(_env), mgr(_mgr) {


	  //Get inputStream from the picture(filename)
	  jobject inputStream = env->CallObjectMethod(mgr, cache->managerOpenMethod, env->NewStringUTF(filename.c_str()));

	  //Create a bitmap from the inputStream
	  jobject firstBitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryDecodeMethod, inputStream);

	  //Make bitmap mutable by calling Copy Method with setting isMutable() to true
	  jobject STATUS_ONE = env->GetStaticObjectField(cache->clSTATUS, cache->fidONE);
	  jboolean copyBoolean = JNI_TRUE;
	  bitmap = env->CallObjectMethod(firstBitmap, cache->bitmapCopyMethod, STATUS_ONE, copyBoolean);

	  //Create new Canvas from the mutable bitmap
	  canvas = env->NewObject(cache->canvasClass, cache->canvasConstructor, bitmap);

	  //testCode();
    }
    
  AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const unsigned char * buffer, size_t size) : Surface(0, 0, 0, 0, false), cache(_cache), env(_env), mgr(_mgr) {
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



    jobject createJavaPaint(RenderMode mode, const Style & style, float lineWidth, float globalAlpha){

  		__android_log_print(ANDROID_LOG_INFO, "Sometrik", "LineWiPdth = %f", lineWidth);
		//create paint
		jboolean copyBoolean = JNI_TRUE;
		jobject jpaint = env->NewObject(cache->paintClass, cache->paintConstructor);

		//Paint.setColor;
		env->CallVoidMethod(jpaint, cache->paintSetAntiAliasMethod, copyBoolean);

		//set paint text size. Not sure if linewidth should be used for this in renderText()
		//env->CallVoidMethod(jpaint, env->GetMethodID(paintClass, "setTextSize", "(F)V"), lineWidth);

		//Paint Set Style
		switch (mode) {
		case STROKE:
			env->CallVoidMethod(jpaint, cache->paintSetStyleMethod,
					env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"),
							env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "STROKE", "Landroid/graphics/Paint$Style;")));
			//Paint Set Stroke Width
			env->CallVoidMethod(jpaint, cache->paintSetStrokeWidthMethod, lineWidth);
			//Paint set StrokeJoin
			env->CallVoidMethod(jpaint, cache->paintSetStrokeJoinMethod,
					env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Join"),
							env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Join"), "ROUND", "Landroid/graphics/Paint$Join;")));
			break;
		case FILL:
			env->CallVoidMethod(jpaint, cache->paintSetStyleMethod,
					env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"),
							env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "FILL", "Landroid/graphics/Paint$Style;")));

			break;
		}
		//Paint set Color
		env->CallVoidMethod(jpaint, cache->paintSetColorMethod, getAndroidColor(style.color, globalAlpha));

		//Set more Paint things here------<

		return jpaint;
    }

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha) override {


  		__android_log_print(ANDROID_LOG_INFO, "Sometrik", "LineWidth = %f", lineWidth);

		jobject jpaint = createJavaPaint(mode, style, lineWidth, globalAlpha);

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "This is...");
		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "...not a problem");
		//debug of renderpath-----------------------
		// renderPath(1, , 1, 10, 1, 10, 10)

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
	  	  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "This is...");
	  	  __android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "...not a problem");

	  	  float span = 0;

	  	  // FIXME: If ea = 0, and sa = 2 * M_PI => span = 0
				if (!pc.anticlockwise && (pc.ea < pc.sa)) {
					span += 2 * M_PI;
				} else if (pc.anticlockwise && (pc.sa < pc.ea)) {
					span -= 2 * M_PI;
				}

				//public void arcTo (float left, float top, float right, float bottom, float startAngle, float sweepAngle, boolean forceMoveTo)

				span += pc.ea - pc.sa;
				float left = pc.x0 * display_scale - pc.radius * display_scale;
				float right =  pc.x0 * display_scale + pc.radius * display_scale;
				float bottom = pc.y0 * display_scale + pc.radius * display_scale;
				float top = pc.y0 * display_scale - pc.radius * display_scale;


			jobject jrect = env->NewObject(cache->rectClass, cache->rectConstructor, left, top, right, bottom);

			jmethodID pathArcToMethod = env->GetMethodID(cache->pathClass, "arcTo", "(Landroid/graphics/RectF;FF)V");

  	  env->CallVoidMethod(jpath, pathArcToMethod, jrect, (float)(pc.sa / M_PI * 180), (float)(span / M_PI * 180));

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



		// debug for prints. Will be removed later
		jclass debugClass = env->FindClass("com/example/work/MyGLSurfaceView");
		jmethodID debugMethod =
				env->GetStaticMethodID(debugClass, "pathDebug",
						"(Landroid/graphics/Path;Landroid/graphics/Paint;Landroid/graphics/Canvas;)V");
		env->CallStaticVoidMethod(debugClass, debugMethod, jpath, jpaint,
				canvas);

    }

    void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, bool _has_alpha) override {
      Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, _has_alpha);
      // do resize the surface and discard the old data
    }
    
    void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, Operator op, float display_scale, float globalAlpha) override {

		jobject jpaint = createJavaPaint(mode, style, lineWidth, globalAlpha);


		jfieldID alignEnumRight    = env->GetStaticFieldID(cache->alignClass , "RIGHT", "Landroid/graphics/Paint$Align;");
		jfieldID alignEnumLeft    = env->GetStaticFieldID(cache->alignClass , "LEFT", "Landroid/graphics/Paint$Align;");
		jfieldID alignEnumCenter    = env->GetStaticFieldID(cache->alignClass , "CENTER", "Landroid/graphics/Paint$Align;");
    	jobject alignRight = env->GetStaticObjectField(cache->alignClass, alignEnumRight);
    	jobject alignLeft = env->GetStaticObjectField(cache->alignClass, alignEnumLeft);
    	jobject alignCenter = env->GetStaticObjectField(cache->alignClass, alignEnumCenter);





		switch (textAlign.getType()) {
		        case TextAlign::LEFT:
		        	env->CallVoidMethod(jpaint, cache->textAlignMethod, alignLeft);

		        	break;
		        case TextAlign::CENTER:
		        	//x -= width / 2;
		        	break;
		        case TextAlign::RIGHT:
		        	env->CallVoidMethod(jpaint, cache->textAlignMethod, alignRight);
		        	//x -= width;
		        	break;
		        default: break;
		}

			env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), x, y, jpaint);

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

  private:
    jobject bitmap;
    jobject canvas;

    AndroidCache * cache;
	JNIEnv * env;
    jobject mgr;
    //AndroidCache _cache;
  };

  class ContextAndroid : public Context {
  public:
  ContextAndroid(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, unsigned int _width, unsigned int _height, const ImageFormat & format, float _display_scale)
    : Context(_display_scale), cache(_cache), env(_env), mgr(_mgr),
      default_surface(_cache, _env, _mgr,  _width, _height, (unsigned int)(_width * _display_scale), (unsigned int)(_height * _display_scale), format)
      {
      }

    std::shared_ptr<Surface> createSurface(const Image & image) override {
      return std::shared_ptr<Surface>(new AndroidSurface(cache, env, mgr, image));
    }
    std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const ImageFormat & _format) override {
      return std::shared_ptr<Surface>(new AndroidSurface(cache, env, mgr, _width, _height, (unsigned int)(_width * getDisplayScale()), (unsigned int)(_height * getDisplayScale()), _format));
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      return std::shared_ptr<Surface>(new AndroidSurface(cache, env, mgr, filename));
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
    AndroidCache * cache;
    JNIEnv * env;
    jobject mgr;
    AndroidSurface default_surface;
  };

  class AndroidContextFactory : public ContextFactory {
  public:
    AndroidContextFactory(JNIEnv * _env, jobject _mgr, float _display_scale = 1.0f) : ContextFactory(_display_scale), cache(_env, _mgr), env(_env), mgr(_mgr) { }
    std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling = false) override {
      std::shared_ptr<Context> ptr(new ContextAndroid(&cache, env, mgr, width, height, format, apply_scaling ? getDisplayScale() : 1.0f));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const std::string & filename) override {
      return std::shared_ptr<Surface>(new AndroidSurface(&cache, env, mgr, filename));
    }
    std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, const ImageFormat & format, bool apply_scaling) override {
      unsigned int aw = apply_scaling ? width * getDisplayScale() : width;
      unsigned int ah = apply_scaling ? height * getDisplayScale() : height;
      std::shared_ptr<Surface> ptr(new AndroidSurface(&cache, env, mgr, width, height, aw, ah, format));
      return ptr;
    }
    std::shared_ptr<Surface> createSurface(const unsigned char * buffer, size_t size) override {
      std::shared_ptr<Surface> ptr(new AndroidSurface(&cache, env, mgr, buffer, size));
      return ptr;
    }
    
  private:
    AndroidCache cache;
    JNIEnv * env;
    jobject mgr;
  };
};

#endif
