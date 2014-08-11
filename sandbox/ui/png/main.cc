
#include "azer/ui/gfx/gfx.h"

int main(int argc, char* argv[]) {
  gfx::GfxAutoInit __autoinit_gfx__;
  std::shared_ptr<gfx::Canvas> canvas(gfx::Canvas::create(400, 300));

  /*
  canvas->FillRect(gfx::Rect(0, 0, 400, 300),
                   SkColorSetARGBInline(0, 0, 0, 0), SkXfermode::kClear_Mode);
  */
  canvas->FillRect(gfx::Rect(0, 0, 400, 300),
                   SkColorSetARGBInline(255, 255, 0, 0), SkXfermode::kSrc_Mode);
  canvas->FillRect(gfx::Rect(0, 0, 400, 300),
                   SkColorSetARGBInline(28, 128, 128, 0), SkXfermode::kSrc_Mode);
  canvas->FillRect(gfx::Rect(0, 50, 200, 150),
                   SkColorSetARGBInline(64, 128, 255, 0));
  canvas->drawText("Hello, gfx", 14, 14);
  canvas->savePNG(base::FilePath(TEXT("out/gfxpng.png")));
  return 0;
}
