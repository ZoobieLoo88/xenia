group("third_party")
project("imgui")
  uuid("ed9271c4-b0a1-42ef-8403-067b11bf49d0")
  kind("StaticLib")
  language("C++")
  links({
  })
  defines({
    "_LIB",
  })
  includedirs({
    "imgui",
  })
  files({
    "imgui/imconfig.h",
    "imgui/imgui.cpp",
    "imgui/imgui.h",
    "imgui/imgui_draw.cpp",
    "imgui/imgui_demo.cpp",
    "imgui/imgui_internal.h",
    "imgui/stb_rect_pack.h",
    "imgui/stb_textedit.h",
    "imgui/stb_truetype.h",
  })
  buildoptions({
    "/wd4312",  -- Ugh.
  })
