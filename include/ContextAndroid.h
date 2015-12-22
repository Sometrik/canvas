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
	AndroidCache(JNIEnv * _env, jobject _mgr) :
			env(_env), mgr(_mgr) {

		initJavaClasses();
		initJavaMethods();

		//arcToMethod = blablah();
		//if (arcToMethod && pathConstructor) {
		//	is_valid = true;
		//}
	}

	void initJavaMethods() {

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
		rectConstructor = env->GetMethodID(rectClass, "<init>", "(FFFF)V");
		paintSetShadowMethod = env->GetMethodID(paintClass, "setShadowLayer", "(FFFI)V");
		canvasBitmapDrawMethod = env->GetMethodID(canvasClass, "drawBitmap", "(Landroid/graphics/Bitmap;FFLandroid/graphics/Paint;)V");
		factoryDecodeByteMethod = env->GetStaticMethodID(factoryClass, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
		bitmapCreateScaledMethod = env->GetStaticMethodID(bitmapClass, "createScaledBitmap", "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;");
		bitmapGetWidthMethod = env->GetMethodID(bitmapClass, "getWidth", "()I");
		bitmapGetHeightMethod = env->GetMethodID(bitmapClass, "getHeight", "()I");
		bitmapOptionsConstructor = env->GetMethodID(bitmapOptionsClass, "<init>", "()V");

		optionsMutableField = env->GetFieldID(bitmapOptionsClass, "inMutable", "Z");

		//drawBitmap(Bitmap bitmap, float left, float top, Paint paint)

	}

	void initJavaClasses() {

		canvasClass = env->FindClass("android/graphics/Canvas");
		mgrClass = env->FindClass("android/content/res/AssetManager");
		factoryClass = env->FindClass("android/graphics/BitmapFactory");
		bitmapClass = env->FindClass("android/graphics/Bitmap");
		paintClass = env->FindClass("android/graphics/Paint");
		pathClass = env->FindClass("android/graphics/Path");
		paintStyleClass = env->FindClass("android/graphics/Paint$Style");
		alignClass = env->FindClass("android/graphics/Paint$Align");
		bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
		field_argb_8888 = env->GetStaticFieldID(bitmapConfigClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
		rectClass = env->FindClass("android/graphics/RectF");
		bitmapOptionsClass = env->FindClass("android/graphics/BitmapFactory$Options");

	}

	bool isValid() const {
		return is_valid;
	}

	//JNIEnv * env;
	//blab * mgr;
	//Jnimethodid arcToMethod;
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
	jmethodID paintSetShadowMethod;
	jmethodID canvasBitmapDrawMethod;
	jmethodID factoryDecodeByteMethod;
	jmethodID bitmapCreateScaledMethod;
	jmethodID bitmapGetWidthMethod;
	jmethodID bitmapGetHeightMethod;
	jmethodID bitmapOptionsConstructor;

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
	jfieldID optionsMutableField;

private:
	JNIEnv * env;
	jobject mgr;
	bool is_valid = false;
};
class AndroidSurface: public Surface {
public:
	friend class ContextAndroid;

	AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, const InternalFormat & _format) :
			Surface(_logical_width, _logical_height, _actual_width, _actual_height, _format), cache(_cache), env(_env), mgr(_mgr) {
		// creates an empty canvas

		// Bitmap.Config conf = Bitmap.Config.ARGB_8888;
		// Bitmap bmp = Bitmap.createBitmap(w, h, conf);
		// Canvas canvas = new Canvas(bmp);

		jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);

		//Not Tested
		bitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod, _actual_width, _actual_height, argbObject);

		//Create new Canvas from the mutable bitmap
		canvas = env->NewObject(cache->canvasClass, cache->canvasConstructor, bitmap);

		//testCode();
	}

	AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const Image & image) :
			Surface(image.getWidth(), image.getHeight(), image.getWidth(), image.getHeight(), InternalFormat::RGBA8), cache(_cache), env(_env), mgr(_mgr) {
		// creates a surface with width, height and contents from image
	}

	AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const std::string & filename) :
			Surface(0, 0, 0, 0, InternalFormat::RGBA8), cache(_cache), env(_env), mgr(_mgr) {

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "...Surface filename constructor");

		//Get inputStream from the picture(filename)
		jobject inputStream = env->CallObjectMethod(mgr, cache->managerOpenMethod, env->NewStringUTF(filename.c_str()));

		//Create a bitmap from the inputStream
		jobject firstBitmap = env->CallStaticObjectMethod(cache->factoryClass, cache->factoryDecodeMethod, inputStream);

		//Make bitmap mutable by calling Copy Method with setting isMutable() to true
		jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
		bitmap = env->CallObjectMethod(firstBitmap, cache->bitmapCopyMethod, argbObject, JNI_TRUE);

		//Create new Canvas from the mutable bitmap
		jobject canvas = env->NewObject(cache->canvasClass, cache->canvasConstructor, bitmap);

		int bitmapWidth = env->CallIntMethod(bitmap, cache->bitmapGetWidthMethod);
		int bitmapHeigth = env->CallIntMethod(bitmap, cache->bitmapGetHeightMethod);
		Surface::resize(bitmapWidth, bitmapHeigth, bitmapWidth, bitmapHeigth, RGBA8);

	}

	AndroidSurface(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, const unsigned char * buffer, size_t size) :
			Surface(0, 0, 0, 0, InternalFormat::RGBA8), cache(_cache), env(_env), mgr(_mgr) {
		// creates a surface from raw data
		// use this: decodeByteArray(byte[] data, int offset, int length)
		// make some wizardry: convert C byte array buffer to Java byte array data
		// BitmapFactory.decoreByteArray
		// create canvas

		//Create a bitmap from bytearray
		//Will propably fatal error 11, size probaly needs to be converted for java.

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "AndrodiSurface constructor (buffer)  called");

		int arraySize = size;

		jbyteArray array = env->NewByteArray(arraySize);
		env->SetByteArrayRegion(array, 0, arraySize, reinterpret_cast<jbyte*>(*buffer));
		bitmap = env->CallStaticObjectMethod(env->FindClass("android/graphics/BitmapFactory"), env->GetStaticMethodID(env->FindClass("android/graphics/BitmapFactory"), "decodeByteArray", "([BII)Landroid/graphics/Bitmap;"), array, 0, arraySize);

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

	//ADD SHADOW EFFECTS
	jobject createJavaPaint(RenderMode mode, const Style & style, float lineWidth, float globalAlpha) {

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

		//Set more Paint things here------<

		return jpaint;
	}

	void renderPath(RenderMode mode, const Path & path, const Style & style, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path & clipPath) override {

		__android_log_print(ANDROID_LOG_INFO, "Sometrik", "LineWidth = %f", lineWidth);

		jobject jpaint = createJavaPaint(mode, style, lineWidth, globalAlpha);

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "This is...");
		env->CallVoidMethod(jpaint, cache->paintSetShadowMethod, shadowBlur, shadowOffsetX, shadowOffsetY, getAndroidColor(shadowColor, globalAlpha));
		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "...not a problem");

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

				//public void arcTo (float left, float top, float right, float bottom, float startAngle, float sweepAngle, boolean forceMoveTo)

				span += pc.ea - pc.sa;
				float left = pc.x0 * display_scale - pc.radius * display_scale;
				float right = pc.x0 * display_scale + pc.radius * display_scale;
				float bottom = pc.y0 * display_scale + pc.radius * display_scale;
				float top = pc.y0 * display_scale - pc.radius * display_scale;

				jobject jrect = env->NewObject(cache->rectClass, cache->rectConstructor, left, top, right, bottom);

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

		// debug for prints. Will be removed later
		jclass debugClass = env->FindClass("com/example/work/MyGLSurfaceView");
		jmethodID debugMethod = env->GetStaticMethodID(debugClass, "pathDebug", "(Landroid/graphics/Path;Landroid/graphics/Paint;Landroid/graphics/Canvas;)V");
		env->CallStaticVoidMethod(debugClass, debugMethod, jpath, jpaint, canvas);

	}

	void resize(unsigned int _logical_width, unsigned int _logical_height, unsigned int _actual_width, unsigned int _actual_height, InternalFormat format) override {
		Surface::resize(_logical_width, _logical_height, _actual_width, _actual_height, format);
		// do resize the surface and discard the old data

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "resize called");

		bitmap = env->CallStaticObjectMethod(cache->bitmapClass, cache->bitmapCreateScaledMethod, bitmap, _logical_width, _logical_height, JNI_FALSE);

	}

	void renderText(RenderMode mode, const Font & font, const Style & style, TextBaseline textBaseline, TextAlign textAlign, const std::string & text, double x, double y, float lineWidth, Operator op, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path & clipPath) override {

		jobject jpaint = createJavaPaint(mode, style, lineWidth, globalAlpha);
		env->CallVoidMethod(jpaint, cache->paintSetShadowMethod, shadowBlur, shadowOffsetX, shadowOffsetY, getAndroidColor(shadowColor, globalAlpha));

		jfieldID alignEnumRight = env->GetStaticFieldID(cache->alignClass, "RIGHT", "Landroid/graphics/Paint$Align;");
		jfieldID alignEnumLeft = env->GetStaticFieldID(cache->alignClass, "LEFT", "Landroid/graphics/Paint$Align;");
		jfieldID alignEnumCenter = env->GetStaticFieldID(cache->alignClass, "CENTER", "Landroid/graphics/Paint$Align;");
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
		default:
			break;
		}

		env->CallVoidMethod(canvas, cache->canvasTextDrawMethod, env->NewStringUTF(text.c_str()), x, y, jpaint);

	}

	TextMetrics measureText(const Font & font, const std::string & text, float display_scale) override {
		// measure width of text
		return TextMetrics(0);
	}

	void drawImage(Surface & _img, double x, double y, double w, double h, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path & clipPath, bool imageSmoothingEnabled = true) override {

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (surface) called");

		auto image = _img.createImage();
		drawImage(*image, x, y, w, h, display_scale, globalAlpha, shadowBlur, shadowOffsetX, shadowOffsetY, shadowColor, clipPath, true);

	}

	void drawImage(const Image & _img, double x, double y, double w, double h, float display_scale, float globalAlpha, float shadowBlur, float shadowOffsetX, float shadowOffsetY, const Color & shadowColor, const Path & clipPath, bool imageSmoothingEnabled = true) override {
		//_img.getWidth() _img.getHeight()

		__android_log_print(ANDROID_LOG_VERBOSE, "Sometrik", "DrawImage (Image) called");

		__android_log_print(ANDROID_LOG_INFO, "Sometrik", "width = %f", w);
		__android_log_print(ANDROID_LOG_INFO, "Sometrik", "height = %f", h);

		const unsigned char* buf = _img.getData();

		//int length = _img->calculateSize();
		//int length = 10000;

		int length = _img.getWidth() * _img.getHeight() * 4;

		__android_log_print(ANDROID_LOG_INFO, "Sometrik", "length = %i", length);

		jbyteArray jarray = env->NewByteArray(length);
		env->SetByteArrayRegion(jarray, 0, length, (jbyte*) (buf));

		jobject argbObject = env->GetStaticObjectField(cache->bitmapConfigClass, cache->field_argb_8888);
		jobject drawableBitmap = env->CallObjectMethod(cache->bitmapClass, cache->bitmapCreateMethod2, jarray, _img.getWidth(), _img.getHeight(), argbObject);

		// make this paint through createJavaPaint() function
		jobject jpaint = env->NewObject(cache->paintClass, cache->paintConstructor);

		env->CallVoidMethod(jpaint, cache->paintSetAntiAliasMethod, JNI_TRUE);
		env->CallVoidMethod(jpaint, cache->paintSetStyleMethod, env->GetStaticObjectField(env->FindClass("android/graphics/Paint$Style"), env->GetStaticFieldID(env->FindClass("android/graphics/Paint$Style"), "STROKE", "Landroid/graphics/Paint$Style;")));

		//env->CallVoidMethod(jpaint, cache->paintSetColorMethod, getAndroidColor(Color::BLACK, globalAlpha));

		env->CallVoidMethod(canvas, cache->canvasBitmapDrawMethod, drawableBitmap, 0.0f, 0.0f, jpaint);

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
	}

private:
	jobject bitmap;
	jobject canvas;

	AndroidCache * cache;
	JNIEnv * env;
	jobject mgr;
};

class ContextAndroid: public Context {
public:
	ContextAndroid(AndroidCache * _cache, JNIEnv * _env, jobject _mgr, unsigned int _width, unsigned int _height, const InternalFormat & format, float _display_scale) :
			Context(_display_scale), cache(_cache), env(_env), mgr(_mgr), default_surface(_cache, _env, _mgr, _width, _height, (unsigned int) (_width * _display_scale), (unsigned int) (_height * _display_scale), format) {
	}

	std::shared_ptr<Surface> createSurface(const Image & image) override {
		return std::shared_ptr < Surface > (new AndroidSurface(cache, env, mgr, image));
	}
	std::shared_ptr<Surface> createSurface(unsigned int _width, unsigned int _height, const InternalFormat & _format) override {
		return std::shared_ptr < Surface > (new AndroidSurface(cache, env, mgr, _width, _height, (unsigned int) (_width * getDisplayScale()), (unsigned int) (_height * getDisplayScale()), _format));
	}
	std::shared_ptr<Surface> createSurface(const std::string & filename) override {
		return std::shared_ptr < Surface > (new AndroidSurface(cache, env, mgr, filename));
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
	AndroidContextFactory(JNIEnv * _env, jobject _mgr, float _display_scale = 1.0f) :
			ContextFactory(_display_scale), cache(_env, _mgr), env(_env), mgr(_mgr) {
	}
	std::shared_ptr<Context> createContext(unsigned int width, unsigned int height, const InternalFormat & format, bool apply_scaling = false) override {
		std::shared_ptr<Context> ptr(new ContextAndroid(&cache, env, mgr, width, height, format, apply_scaling ? getDisplayScale() : 1.0f));
		return ptr;
	}
	std::shared_ptr<Surface> createSurface(const std::string & filename) override {
		return std::shared_ptr < Surface > (new AndroidSurface(&cache, env, mgr, filename));
	}
	std::shared_ptr<Surface> createSurface(unsigned int width, unsigned int height, const InternalFormat & format, bool apply_scaling) override {
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
}
;

#endif
