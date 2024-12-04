project ("vanec")
    kind            ("StaticLib")
    language        ("C")
    cdialect        ("C17")
    systemversion   ("latest")
    warnings        ("extra")
    -- 
    location        (".")
    targetdir       (OUTPUT_BIN_DIR_PATH)
    objdir          (OUTPUT_OBJ_DIR_PATH)
    -- 
    files {
        "src/**.c",
        "include/**.h",
        "include/**.def",
    }
    -- 
    includedirs {
        "include",
    }