
#include <Skbitmap.h>
#include <SkDevice.h>
#include <SkPaint.h>
#include <SkRect.h>

int main(int argc, char* argv[]) {
  SkBitmap bitmap;
  bitmap.setConfig(SkBitmap::KARGB_8888_Config, 100, 100);
  bitmap.allocPixels();
  SkDevice device(bitmap);
  SkCanvas canvas(&device);

  SkPaint paint;
  SkRect r;
  paint.setARGB(255, 255, 255, 255);
  r.set(10, 10, 20, 20);
  canvas.drawRect(r, paint);

  paint.setARGB(255, 255, 0, 0);
  r.offset(5, 5);
  canvas.drawRect(r, paint);

  paint.setARGB(255, 0, 0, 255);
  r.offset(5, 5);
  canvas.drawRect(r, paint);

  {
    SkAutoLockPixels image_lock(bitmap);
    
  }

  return 0;
}
