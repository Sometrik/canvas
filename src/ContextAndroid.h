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
        
  AndroidSurface(JNIEnv * _env, jobject _mgr, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, const ImageFormat & _format)
    : Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format.hasAlpha()), env(_env), mgr(_mgr) {
	  // creates an empty canvas

	  // Bitmap.Config conf = Bitmap.Config.ARGB_8888;
	  // Bitmap bmp = Bitmap.createBitmap(w, h, conf);
	  // Canvas canvas = new Canvas(bmp);
  }
  
  AndroidSurface(JNIEnv * _env, jobject _mgr, const Image & image)
    : Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), image.getFormat().hasAlpha()), env(_env), mgr(_mgr) {
	  // creates a surface with width, height and contents from image
    }
    
  AndroidSurface(JNIEnv * _env, jobject _mgr, const std::string & filename) : Surface(0, 0, 0, 0, false), env(_env), mgr(_mgr) {


		//AAssetManager *mgr2 = AAssetManager_fromJava(env, mgr);

	  jclass clsObj = env->GetObjectClass(mgr);
	  jmethodID methodRef2 = env->GetMethodID(clsObj,
	  						"openFd",
	  						"(Ljava/lang/Object;)Ljava/lang/String;");

	  jobject fd = env->CallObjectMethod(mgr, methodRef2, filename.c_str());
      // bitmap = BitmapFactory.decoreResourse(fd);

		AndroidBitmapInfo info;

		AndroidBitmap_getInfo(env, bitmap, &info);

  	    bool has_alpha = info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGBA_4444;
		Surface::resize(info.width, info.height, info.width, info.height, has_alpha);

		// Canvas canvas = new Canvas(bmp);

		//jclass cls = env->FindClass("com/example/work/MyGLSurfaceView");
		//		jmethodID methodRef = env->GetStaticMethodID(cls,
		//				"LoadImage",
		//				"(Lcom/example/work/MyGLSurfaceView;I)V");

				//env->CallStaticVoidMethod(cls, methodRef, *framework, message);




    }
    
  AndroidSurface(JNIEnv * _env, jobject _mgr, const unsigned char * buffer, size_t size) : Surface(0, 0, 0, 0, false), env(_env), mgr(_mgr) {
	  // creates a surface from raw data
	  // use this: decodeByteArray(byte[] data, int offset, int length)
	  // make some wizardry: convert C byte array buffer to Java byte array data
	  // BitmapFactory.decoreByteArray
	  // create canvas
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

    void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha) override {
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
      // render some text
    }

    TextMetrics measureText(const Font & font, const std::string & text, float display_scale) override {
      // measure width of text
      return TextMetrics(0);
    }

    void drawImage(Surface & surface, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) override {
    	// draw surface as image
    }
    
    void drawImage(const Image & _img, double x, double y, double w, double h, float alpha = 1.0f, bool imageSmoothingEnabled = true) override {
    
    }
    void clip(const Path & path, float display_scale) override {
       // restrict drawing operations to path
    }
    void resetClip() override {
    }
    void save() override {
    }
    void restore() override {
    }

    jobject getBitmap() { return bitmap; }
    jobject getCanvas() { return canvas; }

  protected:
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
      
    Context & drawImage(const Image & img, double x, double y, double w, double h) override {
      return *this;
    }
    
    Context & drawImage(Surface & img, double x, double y, double w, double h) override {
      return *this;
    }
    
  protected:
    Context & renderText(RenderMode mode, const Style & style, const std::string & text, double x, double y, Operator op) override {

    	return *this;
    }
    
    Context & renderPath(RenderMode mode, const Path & path, const Style & style, Operator op) override {

    	return *this;
    }
    
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
