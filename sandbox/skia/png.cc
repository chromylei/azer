#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkImageEncoder.h"
#include "SkString.h"
#include "SkTemplates.h"

int main(int argc, char* argv[]) {
  SkAutoGraphics ag;
  SkString path("out/sk.png");
  SkString text("Hello, Skia World");
  SkPaint paint;
  paint.setARGB(255, 0, 0, 0);
  paint.setAntiAlias(true);
  paint.setTextSize(SkIntToScalar(30));
  SkScalar width = 800;
  SkScalar height = 600;


  SkBitmap bitmap;
  bitmap.setConfig(SkBitmap::kARGB_8888_Config, width, height);
  bitmap.allocPixels();

  SkCanvas canvas(bitmap);
  canvas.drawARGB(255, 255, 255, 255);
  SkScalar x = 80, y = 60;
  canvas.drawText(text.c_str(), text.size(), x, y, paint);
  paint.setStyle(SkPaint::kStroke_Style);
  paint.setStrokeWidth(10);


  SkRect rect;
  paint.setARGB(255, 0, 0, 0);
  rect.set(50, 100, 200, 200);
  canvas.drawRoundRect(rect, 20, 20, paint);

  canvas.drawLine(10, 300, 300, 300, paint);
  canvas.drawCircle(100, 400, 50, paint);
  SkImageEncoder::EncodeFile(path.c_str(), bitmap, SkImageEncoder::kPNG_Type, 100);
  return 0;
}
