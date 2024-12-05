project "testbed"
    kind            "ConsoleApp"
    language        "C"
    cdialect        "C17"
    systemversion   "latest"
    warnings        "extra"
    -- 
    location        (".")
    targetdir       (OUTPUT_BIN_DIR_PATH)
    objdir          (OUTPUT_OBJ_DIR_PATH)
    -- 
    files {
        "src/**.h",
        "src/**.c",
    }
    -- 
    includedirs {
        PROJECTS_DIR_PATH .. "vanec/include",
    }
    -- 
    links {
        "vanec"
    }
    -- 
    debugargs { 
        "--print_tokens",
        "lex",
        ("\"" .. EXAMPLES_DIR_PATH .. "tokens.vn" .. "\"")
    }