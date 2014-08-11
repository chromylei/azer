
#include "ui/gfx/geometry/rect.h"
#include "base/message_loop/message_loop.h"
#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include "base/base.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font_list.h"

#include "SkCanvas.h"
#include "SkBitmap.h"
#include "SkPaint.h"
#include "SkImageEncoder.h"

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  gfx::FontList::SetDefaultFontDescription("Droid Sans serif, Sans serif, 14px");
  ::base::MessageLoop message_loop(::base::MessageLoop::TYPE_UI);
  std::shared_ptr<gfx::Canvas> canvas(new gfx::Canvas(gfx::Size(400, 300),
                                                      1.0, false));

  /*
  canvas->FillRect(gfx::Rect(0, 0, 400, 300),
                   SkColorSetARGBInline(0, 0, 0, 0), SkXfermode::kClear_Mode);
  */
  canvas->FillRect(gfx::Rect(0, 0, 400, 300),
                   SkColorSetARGBInline(255, 255, 0, 0), SkXfermode::kSrc_Mode);
  canvas->FillRect(gfx::Rect(0, 0, 400, 300),
                   SkColorSetARGBInline(28, 128, 128, 0), SkXfermode::kSrc_Mode);

  gfx::FontList fontlist("Courier New, Sans serif, Sans serif, 14px");
  canvas->DrawStringRectWithHalo(TEXT("Hello, gfx::Canvas"),
                                 fontlist,
                                 SK_ColorDKGRAY,
                                 SK_ColorWHITE,
                                 gfx::Rect(0, 0, 300, 30),
                                 gfx::Canvas::TEXT_ALIGN_LEFT);

  canvas->DrawStringRectWithFlags(TEXT("Hello, gfx::Canvas"),
                                  fontlist,
                                  SkColorSetARGBInline(255, 0, 0, 0),
                                  gfx::Rect(0, 30, 300, 50),
                                  gfx::Canvas::TEXT_ALIGN_LEFT);

  SkBitmap bitmap;
  bitmap.setConfig(SkBitmap::kARGB_8888_Config, 400, 300);
  CHECK(canvas->sk_canvas()->readPixels(&bitmap, 0, 0));

  std::string utf8path = base::WideToUTF8(TEXT("out/gfxpng.png"));
  SkImageEncoder::EncodeFile(utf8path.c_str(), bitmap,
                             SkImageEncoder::kPNG_Type, 100);
  return 0;
}
